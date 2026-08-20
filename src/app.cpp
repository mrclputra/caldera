#include "app.h"
#include <iostream>

namespace caldera {
// glfw callbacks
// TODO: we priobably want to setup some kind of event system for stuff like this; might be useful for more complex stuff like gui
static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
   glViewport(0, 0, width, height);
}
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

App::App() {
   // initialize glfw
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   handle = glfwCreateWindow(600, 480, "caldera", nullptr, nullptr);
   glfwMakeContextCurrent(handle);
   gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

   std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";
   std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

   // setup callbacks
   glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);
   glfwSetKeyCallback(handle, key_callback);

   // do stuff
   glViewport(0, 0, 600, 480);

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
}

void App::run() {
   // main execution below
   while (!glfwWindowShouldClose(handle)) {
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glfwSwapBuffers(handle);
      glfwPollEvents();
   }

   // cleanup
   if (handle) {
      glfwDestroyWindow(handle);
      handle = nullptr;
   }
   glfwTerminate();
}
}  // namespace caldera
