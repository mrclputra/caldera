#include "bbox.h"

#include <glm/gtc/matrix_transform.hpp>

namespace caldera {

namespace {
const std::vector<glm::vec3> CUBE_CORNERS = {
    {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f},
};
const std::vector<unsigned int> CUBE_EDGES = {
    0, 1, 1, 2, 2, 3, 3, 0,
    4, 5, 5, 6, 6, 7, 7, 4,
    0, 4, 1, 5, 2, 6, 3, 7,
};
}  // namespace

BoundingBoxGizmo::BoundingBoxGizmo() {
   set_lines(CUBE_CORNERS, CUBE_EDGES);  // static unit cube; only the model matrix moves per-frame
}

void BoundingBoxGizmo::set_bounds(const glm::vec3 &min_, const glm::vec3 &max_) {
   min = min_;
   max = max_;
}

void BoundingBoxGizmo::draw(const glm::mat4 &view, const glm::mat4 &proj) {
   if (!visible) return;
   glm::vec3 center = (min + max) * 0.5f;
   glm::vec3 extent = max - min;
   glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), extent);
   draw_lines(model, view, proj);
}

}  // namespace caldera
