#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aColor;

out vec3 vColor;

const float kScale = 0.2;
const float kNear = -50.0;
const float kFar = 50.0;

void main() {
   vColor = aColor;

   float ndcZ = clamp((aPos.z - kNear) / (kFar - kNear) * 2.0 - 1.0, -1.0, 1.0);

   gl_Position = vec4(aPos.x * kScale, aPos.y * kScale, ndcZ, 1.0);
}
