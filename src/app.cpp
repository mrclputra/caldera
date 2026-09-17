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
   // setup logger
   auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
   g_ring_sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(20);  // decide how many lines to save here

   auto logger = std::make_shared<spdlog::logger>("main", spdlog::sinks_init_list{stdout_sink, g_ring_sink});
   spdlog::set_default_logger(logger);
   spdlog::set_pattern("[%H:%M:S] [%^%l%$] [%s:%#] %v");

   // parse cli arguments
   SPDLOG_INFO("argc: {}", argc);
   SPDLOG_INFO("argv: {}", argv[1]);

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
   gui = std::make_unique<Gui>(window, g_ring_sink);
   scene = std::make_unique<Scene>();

   SPDLOG_INFO("application initialized");

   // testing function
   Loader loader;
   loader.read_ply_file(argv[1], true);
   loader.upload(*scene);

   if (scene->pcd)
      camera->frame(scene->pcd->center, scene->pcd->radius);
}

void App::start() {
   SPDLOG_INFO("beginning main process");

   float last_time = static_cast<float>(glfwGetTime());
   while (!glfwWindowShouldClose(window)) {
      float now = static_cast<float>(glfwGetTime());
      delta_time = now - last_time;  // todo: make this module level?
      last_time = now;

      glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
      // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // render scene
      renderer->render(window, *scene, *camera);

      // read pixels
      // first time histogram, there is probably a better way to do this,
      //    but i just wanted to demonstrate the use of glreadpixels()
      // in this case we read both depth and color buffers--depth to mask out the background and color for a histogram
      int fb_w, fb_h;
      glfwGetFramebufferSize(window, &fb_w, &fb_h);
      std::vector<unsigned char> pixels(static_cast<size_t>(fb_w) * fb_h * 3);
      std::vector<float> depth(static_cast<size_t>(fb_w) * fb_h);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glReadPixels(0, 0, fb_w, fb_h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
      glReadPixels(0, 0, fb_w, fb_h, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());

      // render ui
      gui->render(delta_time * 1000.0, pixels.data(), depth.data(), fb_w, fb_h);

      // inputs
      ImGuiIO &io = ImGui::GetIO();
      if (!io.WantCaptureKeyboard || !io.WantCaptureMouse) {
         if (input->is_key_down(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);  // exit program
         if (input->is_key_down(GLFW_KEY_W)) camera->move_forward(delta_time);
         if (input->is_key_down(GLFW_KEY_S)) camera->move_backward(delta_time);
         if (input->is_key_down(GLFW_KEY_D)) camera->move_right(delta_time);
         if (input->is_key_down(GLFW_KEY_A)) camera->move_left(delta_time);
         if (input->is_key_down(GLFW_KEY_E)) camera->move_up(delta_time);
         if (input->is_key_down(GLFW_KEY_Q)) camera->move_down(delta_time);
         if (input->is_key_down(GLFW_KEY_LEFT_SHIFT))
            camera->mult = 3.0f;
         else
            camera->mult = 1.0f;
         if (input->is_mouse_down(GLFW_MOUSE_BUTTON_LEFT))
            camera->rotate(float(input->cursor_dx), float(input->cursor_dy));
      }
      input->update();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   shutdown();
}

void App::shutdown() {
   glfwDestroyWindow(window);
   glfwTerminate();
   SPDLOG_INFO("application closed");
}

}  // namespace caldera
