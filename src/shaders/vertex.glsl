#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aColor;

out vec3 vColor;

uniform mat4 view;
uniform mat4 proj;

void main() {
   vColor = aColor;

   vec4 viewPos = view * vec4(aPos, 1.0f);
   float distance = -viewPos.z; // you can extract specific attributes of vec4 like this
   gl_PointSize = 200.0 / max(distance, 0.001);
   // gl_PointSize = 10.0;
   gl_Position = proj * viewPos;
}
