#pragma once

#include <GLFW/glfw3.h>

namespace caldera {
   // this class is for all rendering related operations
   // - shader initialization
   // - shader uploading
   // - vertex buffer uploading
class renderer {
 public:
   renderer() {
      // load shaders and stuff
   }
   void render(GLFWwindow *window) {
      glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }
};
}  // namespace caldera
