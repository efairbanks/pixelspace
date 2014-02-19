#ifndef PIXELSPACE_ENGINE_H
#define PIXELSPACE_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <list>
#include "SDL/SDL.h"

class SpaceObject {
 public:
  double x;
  double y;
  double xAccel;
  double yAccel;
  Uint32 ticks;

  SpaceObject(double x, double y, double xAccel, double yAccel);
  Uint32 Tick();
  void Render();
};

class PixelSpace {
 public:
  static bool running;
  static Uint32 ticks;
  std::list<SpaceObject*> spaceObjects;
  // --- //
  static PixelSpace* Engine();
  static PixelSpace* Engine(unsigned int screenWidth,
			    unsigned int screenHeight,
			    unsigned int frameRate);
  void DrawPixel(double x, double y,
		 Uint8 r, Uint8 g, Uint8 b, Uint8 a);
  void FillScreen(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
  Uint32 Tick();
  bool ShutDown();
 private:
  // --- //
  static PixelSpace* _engine;
  // --- //
  SDL_Surface* _screen;
  unsigned int _screenWidth;
  unsigned int _screenHeight;
  unsigned int _frameRate;
  // --- //
  static Uint32 _FrameCallback(Uint32 interval,
			       void* param);
  // --- //
  PixelSpace(){};
  PixelSpace(PixelSpace const&){};
  PixelSpace& operator=(PixelSpace const&){};
};

#endif
