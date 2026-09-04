#pragma once

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace caldera {
// this is wrapper class for a camera in opengl
// it is important to note that these matrices not updated every frame,
//    but rather only when inputs are called
class Camera {
 public:
   // attributes
   glm::vec3 position;
   glm::vec3 front, up, right;  // needed for view matrix
   float pitch{0.0f};
   float yaw{90.0f};

   float sensitivity = 0.12f;
   float speed = 3.0f;
   float fov = 50.0f;  // deg
   float near_plane = 0.1f;
   float far_plane = 10000.0f;

   Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

   void update();

   glm::mat4 get_view_matrix();
   glm::mat4 get_proj_matrix(int width, int height);

   // movement
   void rotate(float x_offset, float y_offset);
   void move_forward(float delta);
   void move_backward(float delta);
   void move_right(float delta);
   void move_left(float delta);
   void move_up(float delta);
   void move_down(float delta);

 private:
   glm::vec3 world_up{0.0f, 1.0f, 0.0f};
};
}  // namespace caldera
