#ifndef NPRSPR_EVENT_MANAGER_H
#define NPRSPR_EVENT_MANAGER_H

#include "BaseEvent.h"
#include "Listener.h"

namespace events {

    std::vector<Listener*> listeners;

    void addListener(Listener* lstnr) {
        listeners.push_back(lstnr);
    }

    void fireEvent(BaseEvent* event) {
        for (auto listener : listeners) {
            listener->listen(event);
        }
    }
}

#endif //NPRSPR_EVENT_MANAGER_H
