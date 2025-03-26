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
        node->pullData();
        // update connected nodes downstream
        for (int i=0; i < node->getOutputCount(); i++) {
            for (auto connectedNode : node->outputs[i]) {
                onNodeChanged(connectedNode);
            }
        }
    }
};

#endif //NPRSPR_GRAPHMANAGER_H
