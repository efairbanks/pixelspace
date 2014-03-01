#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "engine.hpp"

/* PIXELSPACE: High Concept
 * 
 * A pixel-based space shooter where everything is destructable. Every visible game object
 * inherits from a base game object that is represented by one or more pixels.
 * 
 * 
 */

int main(int argc, char** argv) {
  PixelSpace::Engine(640,480,40);
  printf("INIT!\n");
  while(PixelSpace::running) SDL_Delay(1000); // There should really be some kind of mutex
  PixelSpace::Engine()->ShutDown();           // for checking PixelSpace::running
  exit(0);
}
