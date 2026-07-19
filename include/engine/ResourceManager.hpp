#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace voxel::engine {

struct ResourceHandle {
    std::string id;
    std::string path;
};

class ResourceManager {
public:
    void registerAsset(std::string id, std::string path);
    [[nodiscard]] std::optional<ResourceHandle> find(const std::string& id) const;

private:
    std::unordered_map<std::string, std::string> assets_;
};

}  // namespace voxel::engine
