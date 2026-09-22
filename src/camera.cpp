#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace caldera {

glm::vec3 project_to_sphere(float x, float y, int width, int height) {
   // normalize screen coordinates
   float nx = (2.0f * x - (float)width) / (float)width;
   float ny = ((float)height - 2.0f * y) / (float)height;  // screen y goes down
   float len2 = nx * nx + ny * ny;                         // compute 2d length

   // compute z depending on distance from center
   // this is the sphere part
   float nz = len2 <= 0.5f ? std::sqrt(1.0f - len2) : 0.5f / std::sqrt(len2);
   return glm::normalize(glm::vec3(nx, ny, nz));
}

Camera::Camera(glm::vec3 center) {
   this->center = center;
   update();
}

void Camera::update() {
   position = center + orientation * glm::vec3(0.0f, 0.0f, distance);
}

glm::mat4 Camera::get_view_matrix() {
   glm::vec3 up = orientation * world_up;
   return glm::lookAt(position, center, up);
}
glm::mat4 Camera::get_proj_matrix(int width, int height) {
   return glm::perspective(glm::radians(fov), (float)width / (float)height, near_plane, far_plane);
}

void Camera::frame(glm::vec3 center, float radius) {
   radius = std::max(radius, 0.01f);
   this->center = center;
   distance = radius / tan(glm::radians(fov * 0.5f)) * 1.2f;
   far_plane = std::max(far_plane, radius * 4.0f);
   update();
}

void Camera::orbit(glm::vec2 prev, glm::vec2 curr, int width, int height) {
   glm::vec3 p1 = project_to_sphere(prev.x, prev.y, width, height);
   glm::vec3 p2 = project_to_sphere(curr.x, curr.y, width, height);

   // find the axis of rotation
   glm::vec3 axis = glm::cross(p1, p2);  // gives a perpendicular vector
   float axis_len = glm::length(axis);   // how much to rotate
   if (axis_len < 1e-6f)
      return;  // prevent zero-length

   // since p1 and p2 are unit vectors, their dot product is the cosine of the angle between them
   // we can then scale this by sensitivity
   float angle = std::acos(glm::clamp(glm::dot(p1, p2), -1.0f, 1.0f)) * sensitivity;

   // transform the axis from view space to world space
   glm::vec3 world_axis = orientation * (axis / axis_len);
   orientation = glm::normalize(glm::angleAxis(-angle, world_axis) * orientation);  // apply
   update();
}

void Camera::pan(float dx, float dy) {
   glm::vec3 right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
   glm::vec3 up = orientation * world_up;
   center += (-right * dx + up * dy) * pan_speed * distance;
   update();
}

void Camera::zoom(float delta) {
   distance = std::max(distance - delta * zoom_speed * distance, 0.01f);
   update();
}

}  // namespace caldera
