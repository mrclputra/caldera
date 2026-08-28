#pragma once

namespace caldera {
   // im not sure if this is the best approach for storing point clouds
   // here I am just storing the opengl handles for uploaded stuff, though it may be useful to store the actual vertex data in-memory
   //    i.e. if I want to do some kind of data manipulation down the line
   //
   // but then again this depends entirely on what I want to do with this project

   struct PointCloud {
      unsigned int vao, vbo;
      unsigned int vertex_count;
   };
}
