#pragma once

// #include <vector>
#include <memory>

#include "pointcloud.h"

namespace caldera {
// the idea behind this class is that it acts as a "central repository for objects in a scene"
// maybe ecs with entt would have been better but that is a problem for another time
class Scene {
   public:
      // std::vector<std::unique_ptr<PointCloud>> pointclouds; // tbd
      std::unique_ptr<PointCloud> pcd;
};
}  // namespace caldera
