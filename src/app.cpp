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
void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y) {
   static double last_x = x, last_y = y;
   double dx = x - last_x, dy = y - last_y;
   last_x = x; last_y = y;

   App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
   app->camera->rotate(float(dx), float(dy));
}

App::App(int argc, char *argv[]) {
   // parse cli arguments
   SPDLOG_INFO("argc: {}", argc);
   SPDLOG_INFO("argv: {}", argv[1]);  // todo: pass this into the loader

   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   glfwSetErrorCallback(glfw_error_callback);

   window = glfwCreateWindow(1280, 720, "caldera", nullptr, nullptr);
   glfwMakeContextCurrent(window);
   gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
   glfwSetWindowUserPointer(window, this);

   glfwSetKeyCallback(window, glfw_key_callback);
   glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
   glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, -20.0f));
   renderer = std::make_unique<Renderer>();
   scene = std::make_unique<Scene>();

   SPDLOG_INFO("application initialized");

   // testing function
   Loader loader;
   // loader.read_ply_file(*scene, "D:/datasets/clouds/winfield_test_14m.ply", true);
   // loader.read_ply_file(*scene, "C:/Users/Marcelino/Desktop/tests/clouds/flowerPoints.ply", true);
   loader.read_ply_file(argv[1], true);
   loader.upload(*scene);
}

void App::start() {
   double last_time = glfwGetTime();
   while (!glfwWindowShouldClose(window)) {
      double now = glfwGetTime();
      float delta = now - last_time;
      last_time = now;

      // these inputs are continuous, so we cant use callbacks
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
         camera->move_forward(delta);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
         camera->move_backward(delta);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
         camera->move_right(delta);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
         camera->move_left(delta);
      if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
         camera->move_up(delta);
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
         camera->move_down(delta);

      renderer->render(window, *scene, *camera);
   }
   shutdown();
}

void App::shutdown() {
   glfwDestroyWindow(window);
   glfwTerminate();
   SPDLOG_INFO("application closed");
}

}  // namespace caldera
