#ifndef NPRSPR_FILE_MANAGER_H
#define NPRSPR_FILE_MANAGER_H

#include <chrono>
#include <format>

namespace files {

    std::string modelExtensions[4] = {"obj", "fbx", "glb", "gltf"};
    std::vector<std::string> modelFiles; // to be read into at the start

    void readDirectory(const std::string& dirpath, std::string extensions[], std::vector<std::string> *result) {
        // This structure would distinguish a file from a directory
        struct stat sb;

        for (const auto& entry : std::filesystem::directory_iterator(dirpath)) {

            // convert path to const char*
            std::string pathstring = entry.path().string();
            const char* path = pathstring.c_str();

            // if item is a non-directory, save filename
            if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
                for (int i = 0; i < extensions->size(); i++){
                    if (pathstring.ends_with(extensions[i])) {
                        result->emplace_back(pathstring);
                        break;
                    }
                }
            } else readDirectory(pathstring, extensions, result);
        }
    }

    void readModelFiles() {
        readDirectory("res\\models", modelExtensions, &modelFiles);
    }

    void open(const std::string& filepath, Object * obj) {
        if (obj) {
            YAML::Node taskfile = YAML::LoadFile(filepath);
            // construct new graph and assign to obj
            GraphEditorDelegate delegate;
            std::string shaderfile = taskfile["shaderfile"].as<std::string>();
            int noofnodes = taskfile["noofnodes"].as<int>();
            int nooflinks = taskfile["nooflinks"].as<int>();
            for (int i = 0; i < noofnodes; i++) {
                nodeType type = static_cast<nodeType>(taskfile["nodes"][i]["type"].as<int>());
                float x = taskfile["nodes"][i]["x"].as<float>();
                float y = taskfile["nodes"][i]["y"].as<float>();
                if (type == FINAL) {
                    DrawFinal* df = new DrawFinal(&delegate.finalNodeTemplate);
                    df->filename = shaderfile;
                    df->compile();
                    delegate.finalNode = df;
                    delegate.mNodes.push_back(
                            {nodeNames[type], static_cast<GraphEditor::TemplateIndex>(type), x, y,
                             true, df});
                } else {
                    delegate.mNodes.push_back(
                            {nodeNames[type], static_cast<GraphEditor::TemplateIndex>(type), x, y,
                             true, instantiateNode(type)});
                    NodeInstance* currentnode = delegate.mNodes[i].instance;
                    for (int inputno = 0; inputno < currentnode->getInputCount(); inputno++) {
                        int index = taskfile["nodes"][i]["inputs"][inputno]["index"].as<int>();
                        switch(index) {
                            case 0:
                                *currentnode->currentInputs[inputno] = taskfile["nodes"][i]["inputs"][inputno]["value"].as<int>();
                                break;
                            case 1:
                                *currentnode->currentInputs[inputno] = taskfile["nodes"][i]["inputs"][inputno]["value"].as<float>();
                                break;
                            case 2:
                                std::get<ImVec2>(*currentnode->currentInputs[inputno]).x = taskfile["nodes"][i]["inputs"][inputno]["value"]["x"].as<float>();
                                std::get<ImVec2>(*currentnode->currentInputs[inputno]).y = taskfile["nodes"][i]["inputs"][inputno]["value"]["y"].as<float>();
                                break;
                            case 3:
                                std::get<ImVec4>(*currentnode->currentInputs[inputno]).x = taskfile["nodes"][i]["inputs"][inputno]["value"]["x"].as<float>();
                                std::get<ImVec4>(*currentnode->currentInputs[inputno]).y = taskfile["nodes"][i]["inputs"][inputno]["value"]["y"].as<float>();
                                std::get<ImVec4>(*currentnode->currentInputs[inputno]).z = taskfile["nodes"][i]["inputs"][inputno]["value"]["z"].as<float>();
                                std::get<ImVec4>(*currentnode->currentInputs[inputno]).w = taskfile["nodes"][i]["inputs"][inputno]["value"]["w"].as<float>();
                                break;
                        }
                    }
                    if (currentnode->outputType == SHADER) {
                        dynamic_cast<ShaderNodeInstance *>(currentnode)->recompile();
                    }
                }
            }
            for (int i = 0; i < nooflinks; i++) {
                delegate.AddLink(
                static_cast<size_t>(taskfile["links"][i]["InputNodeIndex"].as<int>()),
                static_cast<int8_t>(taskfile["links"][i]["InputSlotIndex"].as<int>()),
                static_cast<size_t>(taskfile["links"][i]["OutputNodeIndex"].as<int>()),
                static_cast<int8_t>(taskfile["links"][i]["OutputSlotIndex"].as<int>())
                                          );
            }
            obj->changeDelegate(&delegate);
            obj->delegate.finalNode->recompile();
        }
    }

    void save(Object * obj) {
        if (obj) {
            YAML::Node taskfile;
            // save the graph with default / current name
            std::string filename = "res/graphs/";
            filename.append(obj->name);
            auto now = std::chrono::system_clock::now();
            filename.append(std::format("{:%Y-%m-%d-%H%M%S}", now));
            filename.append(".ngraph");
            taskfile["shaderfile"] = obj->delegate.finalNode->filename;
            taskfile["noofnodes"] = obj->delegate.GetNodeCount();
            // save nodes
            for (int i = 0; i < obj->delegate.GetNodeCount(); i++) {
                taskfile["nodes"][i]["type"] = static_cast<int>(obj->delegate.mNodes[i].templateIndex);
                taskfile["nodes"][i]["x"] = obj->delegate.mNodes[i].x;
                taskfile["nodes"][i]["y"] = obj->delegate.mNodes[i].y;
                NodeInstance* currentnode = obj->delegate.mNodes[i].instance;
                if (currentnode->outputType != NONE){
                    for (int inputno = 0; inputno < currentnode->getInputCount(); inputno++) {
                        taskfile["nodes"][i]["inputs"][inputno]["index"] = currentnode->currentInputs[inputno]->index();
                        std::visit([&inputno, &taskfile, &i](auto &&value) {
                            using T = std::decay_t<decltype(value)>;

                            if constexpr (std::is_same_v<T, ImVec4>) {
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["x"] = value.x;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["y"] = value.y;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["z"] = value.z;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["w"] = value.w;
                            } else if constexpr (std::is_same_v<T, ImVec2>) {
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["x"] = value.x;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["y"] = value.y;
                            } else {
                                taskfile["nodes"][i]["inputs"][inputno]["value"] = value;
                            }
                        }, *currentnode->currentInputs[inputno]);
                    }
                }
            }
            taskfile["nooflinks"] = obj->delegate.GetLinkCount();
            // save links
            for (int i = 0; i < obj->delegate.GetLinkCount(); i++) {
                taskfile["links"][i]["InputNodeIndex"] = static_cast<int>(obj->delegate.mLinks[i].mInputNodeIndex);
                taskfile["links"][i]["InputSlotIndex"] = static_cast<int>(obj->delegate.mLinks[i].mInputSlotIndex);
                taskfile["links"][i]["OutputNodeIndex"] = static_cast<int>(obj->delegate.mLinks[i].mOutputNodeIndex);
                taskfile["links"][i]["OutputSlotIndex"] = static_cast<int>(obj->delegate.mLinks[i].mOutputSlotIndex);
            }
            // save all to file
            std::ofstream fout(filename);
            fout << taskfile;
            fout.close();
        }
    }

    void save_as(const std::string& filepath, Object * obj) {
        if (obj) {
            // save the graph at filepath
            YAML::Node taskfile;
            // save the graph with default / current name
            taskfile["shaderfile"] = obj->delegate.finalNode->filename;
            taskfile["noofnodes"] = obj->delegate.GetNodeCount();
            // save nodes
            for (int i = 0; i < obj->delegate.GetNodeCount(); i++) {
                taskfile["nodes"][i]["type"] = static_cast<int>(obj->delegate.mNodes[i].templateIndex);
                taskfile["nodes"][i]["x"] = obj->delegate.mNodes[i].x;
                taskfile["nodes"][i]["y"] = obj->delegate.mNodes[i].y;
                NodeInstance* currentnode = obj->delegate.mNodes[i].instance;
                if (currentnode->outputType != NONE) {
                    for (int inputno = 0; inputno < currentnode->getInputCount(); inputno++) {
                        taskfile["nodes"][i]["inputs"][inputno]["index"] = currentnode->currentInputs[inputno]->index();
                        std::visit([&inputno, &taskfile, &i](auto&& value) {
                            using T = std::decay_t<decltype(value)>;

                            if constexpr (std::is_same_v<T, ImVec4>) {
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["x"] = value.x;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["y"] = value.y;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["z"] = value.z;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["w"] = value.w;
                            } else if constexpr (std::is_same_v<T, ImVec2>) {
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["x"] = value.x;
                                taskfile["nodes"][i]["inputs"][inputno]["value"]["y"] = value.y;
                            } else {
                                taskfile["nodes"][i]["inputs"][inputno]["value"] = value;
                            }
                        }, *currentnode->currentInputs[inputno]);
                    }
                }
            }
            taskfile["nooflinks"] = obj->delegate.GetLinkCount();
            // save links
            for (int i = 0; i < obj->delegate.GetLinkCount(); i++) {
                taskfile["links"][i]["InputNodeIndex"] = static_cast<int>(obj->delegate.mLinks[i].mInputNodeIndex);
                taskfile["links"][i]["InputSlotIndex"] = static_cast<int>(obj->delegate.mLinks[i].mInputSlotIndex);
                taskfile["links"][i]["OutputNodeIndex"] = static_cast<int>(obj->delegate.mLinks[i].mOutputNodeIndex);
                taskfile["links"][i]["OutputSlotIndex"] = static_cast<int>(obj->delegate.mLinks[i].mOutputSlotIndex);
            }
            // save all to file
            std::ofstream fout(filepath);
            fout << taskfile;
            fout.close();
        }
    }
}

#endif //NPRSPR_FILE_MANAGER_H
