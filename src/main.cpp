#include "app.h"

int main(int argc, char *argv[]) {
   // ideally we would want to pipe the arguments to the application class, incase we want to load from the cli

   caldera::App *app = new caldera::App(argc, argv);
   app->start();

   return 0;
}
