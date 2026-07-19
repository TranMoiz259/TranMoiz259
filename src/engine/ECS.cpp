#include "engine/ECS.hpp"

namespace voxel::engine {

Entity ECS::createEntity() {
    return nextEntity_++;
}

}  // namespace voxel::engine
