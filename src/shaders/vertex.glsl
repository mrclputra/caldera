#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aColor;

out vec3 vColor;

uniform mat4 view;
uniform mat4 proj;

uniform uint pointSize;

void main() {
   vColor = aColor;

   vec4 viewPos = view * vec4(aPos, 1.0f);
   gl_PointSize = (float(pointSize) * 100.0) / max(-viewPos.z, 0.001);
   gl_Position = proj * viewPos;
}
