#pragma once

#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace voxel::engine {

using Entity = std::uint32_t;

class ECS {
public:
    Entity createEntity();
    template <typename T>
    void addComponent(Entity entity, T component) {
        storage_[std::type_index(typeid(T))][entity] = std::make_shared<Model<T>>(std::move(component));
    }

    template <typename T>
    T* getComponent(Entity entity) {
        auto sIt = storage_.find(std::type_index(typeid(T)));
        if (sIt == storage_.end()) {
            return nullptr;
        }
        auto cIt = sIt->second.find(entity);
        if (cIt == sIt->second.end()) {
            return nullptr;
        }
        return &static_cast<Model<T>*>(cIt->second.get())->value;
    }

private:
    struct Concept {
        virtual ~Concept() = default;
    };

    template <typename T>
    struct Model final : Concept {
        explicit Model(T val) : value(std::move(val)) {}
        T value;
    };

    Entity nextEntity_{1};
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<Concept>>> storage_;
};

}  // namespace voxel::engine
