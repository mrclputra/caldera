#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace caldera {
class App {
 public:
   App();

   void run();

   GLFWwindow *handle;  // glfw window handle
};
}  // namespace caldera
