#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

static void glfw_error_callback(int error, const char *description) {
   std::cout << "glfw error: " << description << "\n";
}
static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
   }
}

int main(int argc, char *argv[]) {
   std::cout << "Woohoo!! I am compiled with LLVM!!" << "\n";
   std::cout << "This is version " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << "\n";

   // init glfw
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit())
      return 1;
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
   // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow *window;
   window = glfwCreateWindow(640, 480, "caldera", NULL, NULL);
   if (!window) {
      glfwTerminate();
      return 1;
   }
   glfwMakeContextCurrent(window);

   // load opengl function interface
   // note that the glfw context needs to be active
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "failed to initialize GLAD" << "\n";
      glfwTerminate();
      return 1;
   }

   glfwSetKeyCallback(window, glfw_key_callback);
   while (!glfwWindowShouldClose(window)) {
      float ratio;
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      ratio = width / (float)height;
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
      glBegin(GL_TRIANGLES);
      glColor3f(1.f, 0.f, 0.f);
      glVertex3f(-0.6f, -0.4f, 0.f);
      glColor3f(0.f, 1.f, 0.f);
      glVertex3f(0.6f, -0.4f, 0.f);
      glColor3f(0.f, 0.f, 1.f);
      glVertex3f(0.f, 0.6f, 0.f);
      glEnd();
      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}
