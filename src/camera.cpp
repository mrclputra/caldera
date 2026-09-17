#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace caldera {
Camera::Camera(glm::vec3 position) {
   this->position = position;
   SPDLOG_INFO("camera object created");
   update();
}

void Camera::update() {
   glm::vec3 v;
   v.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
   v.y = sin(glm::radians(pitch));
   v.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

   front = glm::normalize(v);
   right = glm::normalize(glm::cross(front, world_up));
   up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::get_view_matrix() {
   return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::get_proj_matrix(int width, int height) {
   return glm::perspective(
       glm::radians(fov),
       (float)width / (float)height,
       near_plane,
       far_plane);
}

void Camera::frame(glm::vec3 center, float radius) {
   radius = std::max(radius, 0.01f);

   float distance = radius / tan(glm::radians(fov * 0.5f)) * 1.2f;
   position = center - front * distance;

   far_plane = std::max(far_plane, radius * 4.0f);
   speed = std::max(speed, radius * 0.5f);
}

void Camera::rotate(float x_offset, float y_offset) {
   yaw += x_offset * sensitivity;
   pitch -= y_offset * sensitivity;

   pitch = glm::clamp(pitch, -89.0f, 89.0f);
   update();
}
void Camera::move_forward(float delta) {
   position += front * speed * mult * delta;
}
void Camera::move_backward(float delta) {
   position -= front * speed * mult * delta;
}
void Camera::move_right(float delta) {
   position += right * speed * mult * delta;
}
void Camera::move_left(float delta) {
   position -= right * speed * mult * delta;
}
void Camera::move_up(float delta) {
   position += up * speed * mult * delta;
}
void Camera::move_down(float delta) {
   position -= up * speed * mult * delta;
}

}  // namespace caldera
