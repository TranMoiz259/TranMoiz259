#include "engine/ResourceManager.hpp"

namespace voxel::engine {

void ResourceManager::registerAsset(std::string id, std::string path) {
    assets_.insert_or_assign(std::move(id), std::move(path));
}

std::optional<ResourceHandle> ResourceManager::find(const std::string& id) const {
    auto it = assets_.find(id);
    if (it == assets_.end()) {
        return std::nullopt;
    }
    return ResourceHandle{it->first, it->second};
}

}  // namespace voxel::engine
