#include <iostream>

#include "app.h"

int main(int argc, char* argv[]) {
   std::cout << "hello world" << "\n";

   caldera::app app;
   app.start();

   return 0;
}
