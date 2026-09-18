#pragma once

#include "gizmo.h"

namespace caldera {
class BoundingBoxGizmo : public Gizmo {
 public:
   BoundingBoxGizmo();

   void set_bounds(const glm::vec3 &min, const glm::vec3 &max);
   void draw(const glm::mat4 &view, const glm::mat4 &proj) override;

 private:
   glm::vec3 min{-0.5f}, max{0.5f};
};
}  // namespace caldera
