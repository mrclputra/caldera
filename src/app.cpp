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

App::App(int argc, char* argv[]) {
   // parse cli arguments
   SPDLOG_INFO("argc: {}", argc);
   SPDLOG_INFO("argv: {}", argv[1]); // todo: pass this into the loader

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
   scene = std::make_unique<Scene>();

   SPDLOG_INFO("application initialized");

   // testing function
   Loader loader;
   // loader.read_ply_file(*scene, "D:/datasets/clouds/winfield_test_14m.ply", true);
   // loader.read_ply_file(*scene, "C:/Users/Marcelino/Desktop/tests/clouds/flowerPoints.ply", true);
   loader.read_ply_file(argv[1], true);
   loader.upload(*scene);
   // todo: ask loader to upload to gpu
}

void App::start() {
   while (!glfwWindowShouldClose(window)) {
      renderer->render(window, *scene);
   }
   shutdown();
}

void App::shutdown() {
   glfwDestroyWindow(window);
   glfwTerminate();
   SPDLOG_INFO("application closed");
}

}  // namespace caldera
