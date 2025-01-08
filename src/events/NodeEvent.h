#ifndef NPRSPR_NODEEVENT_H
#define NPRSPR_NODEEVENT_H

class NodeEvent : public BaseEvent {
public:
    void* sender;

    NodeEvent(void* s) {
        sender = s;
    }

};

#endif //NPRSPR_NODEEVENT_H
