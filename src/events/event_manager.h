#ifndef NPRSPR_EVENT_MANAGER_H
#define NPRSPR_EVENT_MANAGER_H

#include "BaseEvent.h"
#include "Listener.h"
#include <algorithm>

namespace events {

    struct ListenerEntry {
        std::weak_ptr<Listener> listener;
        int priority;  // Higher priority = processed later

        ListenerEntry(std::shared_ptr<Listener> lstnr, int prio) 
            : listener(lstnr), priority(prio) {}
    };

    std::vector<ListenerEntry> listeners;

    void addListener(std::shared_ptr<Listener> lstnr, int priority = 0) {
        listeners.emplace_back(lstnr, priority);
        // Sort by priority (ascending order)
        std::sort(listeners.begin(), listeners.end(), 
            [](const ListenerEntry& a, const ListenerEntry& b) {
                return a.priority < b.priority;
            });
    }

    void removeListener(std::shared_ptr<Listener> lstnr) {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                [&](const ListenerEntry& entry) {
                    auto shared_lstnr = entry.listener.lock();
                    return !shared_lstnr || shared_lstnr == lstnr;
                }
            ),
            listeners.end()
        );
    }

    void fireEvent(BaseEvent* event) {
        for (auto& entry : listeners) {
            if (auto shared_lstnr = entry.listener.lock()) {
                shared_lstnr->listen(event);
            }
        }
    }
}

#endif //NPRSPR_EVENT_MANAGER_H
