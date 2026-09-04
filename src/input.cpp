#include "input.h"

namespace caldera {

Input::Input(GLFWwindow *window) {
   glfwSetWindowUserPointer(window, this);
   glfwSetKeyCallback(window, key_callback);
   glfwSetCursorPosCallback(window, cursor_pos_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetMouseButtonCallback(window, mouse_button_callback);

   glfwGetCursorPos(window, &cursor_x, &cursor_y);
}

void Input::update() {
   cursor_dx = 0.0f;
   cursor_dy = 0.0f;
   scroll_delta = 0.0f;
}

bool Input::is_mouse_down(int key) const {
   return buttons_current[key];
}
bool Input::is_key_down(int key) const {
   return keys_current[key];
}

void Input::mouse_button_callback(GLFWwindow *window, int button, int action, int) {
   if (button < 0 || button >= num_buttons)
      return;

   Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
   if (action == GLFW_PRESS)
      input->buttons_current[button] = true;
   else if (action == GLFW_RELEASE)
      input->buttons_current[button] = false;
}

void Input::key_callback(GLFWwindow *window, int key, int, int action, int) {
   if (key < 0 || key >= num_keys)
      return;  // GLFW_KEY_UNKNOWN is -1

   Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
   if (action == GLFW_PRESS)
      input->keys_current[key] = true;
   else if (action == GLFW_RELEASE)
      input->keys_current[key] = false;
}

void Input::cursor_pos_callback(GLFWwindow *window, double x, double y) {
   Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));

   if (input->first_cursor_event) {
      input->cursor_x = x;
      input->cursor_y = y;
      input->first_cursor_event = false;
   }

   input->cursor_dx += x - input->cursor_x;
   input->cursor_dy += y - input->cursor_y;
   input->cursor_x = x;
   input->cursor_y = y;
}

void Input::scroll_callback(GLFWwindow *window, double, double y_offset) {
   Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
   input->scroll_delta += y_offset;
}

}  // namespace caldera
