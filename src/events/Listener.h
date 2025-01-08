#ifndef NPRSPR_LISTENER_H
#define NPRSPR_LISTENER_H

class Listener {
public:
    virtual void listen(BaseEvent* event) = 0;
};

#endif //NPRSPR_LISTENER_H
