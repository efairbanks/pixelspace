#ifndef PIXELSPACE_ENGINE_H
#define PIXELSPACE_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL/SDL.h"

class PixelSpace {
 public:
  static bool running;
  static PixelSpace* Engine();
  static PixelSpace* Engine(unsigned int screenWidth,
			    unsigned int screenHeight,
			    unsigned int frameRate);
  bool ShutDown();
 private:
  void DrawPixel(double x, double y,
		 Uint8 r, Uint8 g, Uint8 b, Uint8 a);
  void FillScreen(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
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
