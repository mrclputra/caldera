#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aColor;

out vec3 vColor;

void main() {
   vColor = aColor;
   gl_Position = vec4(aPos.x * 0.2, aPos.y * 0.2, 0.0, 1.0);
}
