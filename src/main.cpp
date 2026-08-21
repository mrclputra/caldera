#include <iostream>

#define GLFW_INCLUDE_NONE
#include "app.h"

int main(int argc, char *argv[]) {
   std::cout << "Woohoo!! I am compiled with LLVM!!" << "\n";
   std::cout << "This is version " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << "\n";

   auto app = caldera::App();
   app.run();

   return 0;
}
