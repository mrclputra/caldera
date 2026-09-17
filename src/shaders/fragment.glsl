#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
   // calculate distance from point center
   vec2 circCoord = gl_PointCoord - vec2(0.5);
   if (dot(circCoord, circCoord) > 0.25){
      discard;
   }

   FragColor = vec4(vColor, 1.0);
   // FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f); // debug
}
