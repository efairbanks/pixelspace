#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "engine.h"

int main(int argc, char** argv) {
  PixelSpace::Engine(640,480,30);
  printf("INIT!\n");
  while(PixelSpace::running) SDL_Delay(1000);
  PixelSpace::Engine()->ShutDown();
  exit(0);
}
