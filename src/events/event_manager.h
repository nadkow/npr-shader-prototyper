#ifndef NPRSPR_EVENT_MANAGER_H
#define NPRSPR_EVENT_MANAGER_H

#include "BaseEvent.h"
#include "Listener.h"

namespace events {

    std::vector<std::weak_ptr<Listener>> listeners;

    void addListener(std::shared_ptr<Listener> lstnr) {
        listeners.push_back(lstnr);
    }

    void removeListener(std::shared_ptr<Listener> lstnr) {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                [&](const std::weak_ptr<Listener>& weak_lstnr) {
                    auto shared_lstnr = weak_lstnr.lock();
                    return !shared_lstnr || shared_lstnr == lstnr;
                }
            ),
            listeners.end()
        );
    }

    void fireEvent(BaseEvent* event) {
        for (auto& weak_lstnr : listeners) {
            if (auto shared_lstnr = weak_lstnr.lock()) {
                shared_lstnr->listen(event);
            }
        }
    }
}

#endif //NPRSPR_EVENT_MANAGER_H
