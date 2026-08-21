#pragma once

#include <glm/glm.hpp>

namespace caldera {
class Camera {
   glm::vec3 front{}, up{}, right{};
   float yaw = -90.0f;
   float pitch = 0.0f;
   float sens = 0.07f;
   float speed = 3.0f;  // movement
   float fov = 50.0f;
   float near_plane = 0.1f;
   float far_plant = 10000.0f;

   Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

   void update(float delta);

   glm::mat4 getViewMatrix();
   glm::mat3 getProjectionMatrix(int width, int height);
};
}  // namespace caldera
