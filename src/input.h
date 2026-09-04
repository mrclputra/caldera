#pragma once

#include <GLFW/glfw3.h>

namespace caldera {
class Input {
 public:
   Input(GLFWwindow *window);

   void update();

   bool is_key_down(int key) const;
   bool is_mouse_down(int key) const;

   double cursor_x = 0.0f;
   double cursor_y = 0.0f;
   double cursor_dx = 0.0f;
   double cursor_dy = 0.0f;
   double scroll_delta = 0.0f;

 private:
   static void key_callback(GLFWwindow *, int key, int, int action, int);
   static void cursor_pos_callback(GLFWwindow *, double x, double y);
   static void scroll_callback(GLFWwindow*, double, double y_offset);
   static void mouse_button_callback(GLFWwindow* window, int button, int action, int);

   static constexpr int num_keys = GLFW_KEY_LAST + 1;
   bool keys_current[num_keys]{};

   static constexpr int num_buttons = GLFW_MOUSE_BUTTON_LAST + 1;
   bool buttons_current[num_buttons]{};

   bool first_cursor_event = true;
};
}  // namespace caldera
