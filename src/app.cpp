#include "app.h"

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

App::App() {
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

void App::start() {
   while (!glfwWindowShouldClose(window)) {
      renderer->render(window);
   }
   shutdown();
}

void App::shutdown() {
   glfwDestroyWindow(window);
   glfwTerminate();
   SPDLOG_INFO("application closed");
}

}  // namespace caldera
