#pragma once

#include <GLFW/glfw3.h>

#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "gizmos/bbox.h"

namespace caldera {
// this class is for all rendering related operations
// - shader initialization
// - shader uploading
// - vertex buffer uploading

class Renderer {
 public:
   Renderer();
   ~Renderer();
   void render(GLFWwindow *window, Scene &scene, Camera &camera);

   unsigned int vbo;
   unsigned int vao;

   unsigned int visible_count = 0;
   glm::vec3 last_cam_pos{};
   bool first_frame = true;

   std::unique_ptr<Shader> shader;
   std::unique_ptr<BoundingBoxGizmo> bbox_gizmo;
};
}  // namespace caldera
