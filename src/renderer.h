#pragma once

#include <GLFW/glfw3.h>

#include "shader.h"

namespace caldera {
// this class is for all rendering related operations
// - shader initialization
// - shader uploading
// - vertex buffer uploading
class renderer {
 public:
   renderer() {
      glEnable(GL_DEPTH_TEST);
      // glEnable(GL_PROGRAM_POINT_SIZE); // to be configured, so i dont forget
   }
   void render(GLFWwindow *window) {
      glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   shader shader {"assets/vertex.glsl", "assets/fragment.glsl"};
};
}  // namespace caldera
