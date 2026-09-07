#pragma once

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

namespace caldera {
// this is a wrapper class for imgui
class Gui {
 public:
   Gui(GLFWwindow *window);
   ~Gui();
   void render();  // draw the gui
 private:
   GLFWwindow *window;
};
}  // namespace caldera
