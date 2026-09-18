#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace caldera {
// this is an orbit camera, that rotates around a center at a distance
class Camera {
 public:
   Camera(glm::vec3 center = glm::vec3(0.0f));

   glm::mat4 get_view_matrix();
   glm::mat4 get_proj_matrix(int width, int height);

   void frame(glm::vec3 center, float radius); // fit object in view

   void orbit(glm::vec2 prev, glm::vec2 curr, int width, int height);
   void pan(float dx, float dy);
   void zoom(float delta);

   glm::vec3 position{};
   glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
   glm::vec3 center{0.0f};
   float distance = 10.0f;

   float sensitivity = 1.4f;
   float pan_speed = 0.0003f;
   float zoom_speed = 0.05f;
   float fov = 20.0f;
   float near_plane = 0.1f;
   float far_plane = 10000.0f;

 private:
   glm::vec3 world_up{0.0f, 1.0f, 0.0f};
   void update(); // update position and stuff
};
}  // namespace caldera
