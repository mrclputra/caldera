#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "renderer.h"

namespace caldera {
// basic app class for high-level application management and stuff
// events and bindings go here, callbacks should be sourced from here
class app {
 public:
   app() {
      glfwInit();
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      window = glfwCreateWindow(1280, 720, "caldera", nullptr, nullptr);
      glfwMakeContextCurrent(window);
      gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
   }

   void start() {
      while (!glfwWindowShouldClose(window)) {
         renderer.render(window);
      }
      shutdown();
   }

 private:
   void shutdown() {
      glfwDestroyWindow(window);
      glfwTerminate();
   }

   GLFWwindow *window;
   renderer renderer;
};
}  // namespace caldera
