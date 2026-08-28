#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "renderer.h"
#include "loader.h"

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
}
// TODO: use event system to bind glfw actions

// basic app class for high-level application management and stuff
// events and bindings go here, callbacks should be sourced from here
class App {
 public:
   App() {
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

      renderer = std::make_unique<Renderer>();

      SPDLOG_INFO("application initialized");

      // testing function
      Loader loader;
      loader.read_ply_file("D:/datasets/clouds/Workshop_point_cloud.ply", true);
   }

   void start() {
      while (!glfwWindowShouldClose(window)) {
         renderer->render(window);
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

   std::unique_ptr<Renderer> renderer;
};
}  // namespace caldera
