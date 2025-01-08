#ifndef NPRSPR_GRAPHMANAGER_H
#define NPRSPR_GRAPHMANAGER_H

class GraphManager : public Listener {
public:
    void listen(BaseEvent* event) override {
        NodeEvent* nodeevent = dynamic_cast<NodeEvent*>(event);
        if (nodeevent) {
            onNodeChanged(static_cast<NodeInstance *>(nodeevent->sender));
        }
    }

private:

    void onNodeChanged(NodeInstance* node) {
        //update current node
        std::cout << "updating node" << std::endl;
        // update connected nodes downstream
        for (int i=0; i < node->getOutputCount(); i++) {
            std::cout << "check socket " << i << std::endl;
            for (auto connectedNode : node->outputs[i]) {
                std::cout << "updating child..." << std::endl;
                onNodeChanged(connectedNode);
            }
        }
    }
};

#endif //NPRSPR_GRAPHMANAGER_H
