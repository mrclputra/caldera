#include "app.h"
#include <iostream>

int main(int argc, char *argv[]) {
   if (argc < 2) {
      std::cout << "usage: caldera.exe <path-to-point-cloud.ply>" << '\n';
      return 0;
   }

   caldera::App *app = new caldera::App(argc, argv);
   app->start();

   return 0;
}
