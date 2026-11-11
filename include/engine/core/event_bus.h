#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <memory>

namespace PJEngine {

class EventBus {
private:
    struct HandlerBase {
        virtual ~HandlerBase() = default;
    };

    template <typename T>
    struct Handler : public HandlerBase {
        std::function<void(const T&)> callback;
        explicit Handler(std::function<void(const T&)> cb) : callback(cb) {}
    };

    std::unordered_map<std::type_index, std::vector<std::shared_ptr<HandlerBase>>> handlers;

public:
    template <typename T>
    void subscribe(std::function<void(const T&)> callback) {
        std::type_index type = std::type_index(typeid(T));
        handlers[type].push_back(std::make_shared<Handler<T>>(callback));
    }

    template <typename T>
    void publish(const T& event) {
        std::type_index type = std::type_index(typeid(T));
        auto it = handlers.find(type);
        if (it != handlers.end()) {
            for (auto& handlerBase : it->second) {
                auto h = std::static_pointer_cast<Handler<T>>(handlerBase);
                h->callback(event);
            }
        }
    }

    void clear() {
        handlers.clear();
    }
};

} // namespace PJEngine