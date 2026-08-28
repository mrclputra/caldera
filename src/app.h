#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "renderer.h"

namespace caldera {

void glfw_error_callback(int error, const char *description) {
   SPDLOG_ERROR("glfw error {}: {}", error, description);
}
void glfw_key_callback(GLFWwindow *window, int key, int, int action, int) {
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}
void glfw_framebuffer_size_callback(GLFWwindow *, int width, int height) {
   glViewport(0, 0, width, height);
   // SPDLOG_INFO("viewport changed {}.{}", width, height);
}

// basic app class for high-level application management and stuff
// events and bindings go here, callbacks should be sourced from here
class app {
 public:
   app() {
      glfwInit();
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      glfwSetErrorCallback(glfw_error_callback);

      window = glfwCreateWindow(1280, 720, "caldera", nullptr, nullptr);
      glfwMakeContextCurrent(window);
      gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

      glfwSetKeyCallback(window, glfw_key_callback);
      glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

      renderer_ = std::make_unique<renderer>();

      SPDLOG_INFO("application initialized");
   }

   void start() {
      while (!glfwWindowShouldClose(window)) {
         renderer_->render(window);
      }
      shutdown();
   }

 private:
   void shutdown() {
      glfwDestroyWindow(window);
      glfwTerminate();
      SPDLOG_INFO("application closed");
   }

   GLFWwindow *window;

   std::unique_ptr<renderer> renderer_;
};
}  // namespace caldera
