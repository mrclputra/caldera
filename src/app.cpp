#include "app.h"

#include "loader.h"

namespace caldera {

void glfw_error_callback(int error, const char *description) {
   SPDLOG_ERROR("glfw error {}: {}", error, description);
}
void glfw_framebuffer_size_callback(GLFWwindow *, int width, int height) {
   glViewport(0, 0, width, height);
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

   glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   input = std::make_unique<Input>(window);
   camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, -10.0f));
   renderer = std::make_unique<Renderer>();
   scene = std::make_unique<Scene>();

   SPDLOG_INFO("application initialized");

   // testing function
   Loader loader;
   loader.read_ply_file(argv[1], true);
   loader.upload(*scene);
}

void App::start() {
   double last_time = glfwGetTime();
   while (!glfwWindowShouldClose(window)) {
      double now = glfwGetTime();
      float delta = now - last_time;
      last_time = now;

      renderer->render(window, *scene, *camera);

      if (input->is_key_down(GLFW_KEY_ESCAPE))
         glfwSetWindowShouldClose(window, true);  // exit program
      if (input->is_key_down(GLFW_KEY_W)) camera->move_forward(delta);
      if (input->is_key_down(GLFW_KEY_S)) camera->move_backward(delta);
      if (input->is_key_down(GLFW_KEY_D)) camera->move_right(delta);
      if (input->is_key_down(GLFW_KEY_A)) camera->move_left(delta);
      if (input->is_key_down(GLFW_KEY_E)) camera->move_up(delta);
      if (input->is_key_down(GLFW_KEY_Q)) camera->move_down(delta);
      if (input->is_mouse_down(GLFW_MOUSE_BUTTON_LEFT))
         camera->rotate(float(input->cursor_dx), float(input->cursor_dy));

      input->update();
   }
   shutdown();
}

void App::shutdown() {
   glfwDestroyWindow(window);
   glfwTerminate();
   SPDLOG_INFO("application closed");
}

}  // namespace caldera
