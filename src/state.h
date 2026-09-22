#pragma once
#include <string>

// this is a pull-based architecture
namespace caldera {

struct State {
   int init_points = 4500; // this is the initial point count; needs to be a fraction of the loaded vertex count
   float growth_rate = 1.1f;
   unsigned int vertex_count = 0;

   int view_mode = 0;
   unsigned int point_size = 3;

   std::string loaded_filename;
   // bool request_load = false;
};
extern State state;

}  // namespace caldera
