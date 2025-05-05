#ifndef NPRSPR_FILE_MANAGER_H
#define NPRSPR_FILE_MANAGER_H

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
}

#endif //NPRSPR_FILE_MANAGER_H
