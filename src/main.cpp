#include "app.h"

int main(int argc, char *argv[]) {
   caldera::App *app = new caldera::App(argc, argv);
   app->start();

   return 0;
}
