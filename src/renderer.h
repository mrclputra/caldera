#pragma once

#include <GLFW/glfw3.h>

#include "shader.h"
#include "scene.h"
#include "camera.h"

namespace caldera {
// this class is for all rendering related operations
// - shader initialization
// - shader uploading
// - vertex buffer uploading

class Renderer {
 public:
   Renderer();
   ~Renderer();
   void render(GLFWwindow *window, Scene & scene, Camera &camera);

   unsigned int vbo;
   unsigned int vao;

   std::unique_ptr<Shader> shader;
};
}  // namespace caldera
