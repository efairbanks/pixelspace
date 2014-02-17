#include "engine.h"

#define ERROR(ERRORSTRING) printf(ERRORSTRING"\n"); exit(1);

PixelSpace* PixelSpace::_engine = NULL;
bool PixelSpace::running = false;

PixelSpace* PixelSpace::Engine() {
  return _engine;
}

PixelSpace* PixelSpace::Engine(unsigned int screenWidth,
			       unsigned int screenHeight,
			       unsigned int frameRate) {
  if(!_engine) {
    _engine = new PixelSpace;
    _engine->_screenWidth = screenWidth;
    _engine->_screenHeight = screenHeight;
    _engine->_frameRate = frameRate;
    printf("VARS\n");
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)) {
      ERROR("SDL_Init failure. ");}
    printf("INIT\n");
    atexit(SDL_Quit);
    printf("SDLQUIT\n");
    _engine->_screen = SDL_SetVideoMode(_engine->_screenWidth,
					_engine->_screenHeight,
					32,
					SDL_SWSURFACE);
    printf("SCREEN\n");
    if(_engine->_screen==NULL) {
      ERROR("_screen init failure.");}
    SDL_AddTimer(1000/_engine->_frameRate,
		 PixelSpace::_FrameCallback,
		 NULL);
    running = true;
  }
    return _engine;
}

bool PixelSpace::ShutDown() {
  return false;
}

void PixelSpace::FillScreen(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  for(int ix = 0; ix < _screenWidth; ix++)
    for(int iy = 0; iy < _screenHeight; iy++) {
      unsigned int offset = ix;
      offset += ((_screenHeight-1)-iy)*(_screen->pitch/4);
      ((unsigned int*)_screen->pixels)[offset] = 
	SDL_MapRGBA(_screen->format,r,g,b,a);
    }
}

void PixelSpace::DrawPixel(double x, double y,
			   Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  unsigned int ix =(unsigned int)((x*(_screenWidth/2))+(_screenWidth/2));
  unsigned int iy =(unsigned int)((y*(_screenHeight/2))+(_screenHeight/2));
  while(ix >= _screenWidth) ix -= _screenWidth;
  while(iy >= _screenHeight) iy -= _screenHeight;
  unsigned int offset = ix;
  offset += ((_screenHeight-1)-iy)*(_screen->pitch/4);
  ((unsigned int*)_screen->pixels)[offset] = SDL_MapRGBA(_screen->format,
							 r,g,b,a);
}

Uint32 PixelSpace::_FrameCallback(Uint32 interval,
				  void* param) {
  if (SDL_MUSTLOCK(_engine->_screen)) 
    if (SDL_LockSurface(_engine->_screen) < 0) 
      return 0;
  int tick = SDL_GetTicks();

  // --- //
  
  _engine->FillScreen(0,0,0,0);
  _engine->DrawPixel(0+(tick/(1000.0*60.0)),0,0xff,0xff,0xff,0xff);

  // --- //

  if (SDL_MUSTLOCK(_engine->_screen)) 
    SDL_UnlockSurface(_engine->_screen);
  SDL_UpdateRect(_engine->_screen,
		 0,
		 0,
		 _engine->_screenWidth,
		 _engine->_screenHeight);
  
  // --- //

  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (event.key.keysym.sym == SDLK_ESCAPE)
	running = false;
      break;
      //case SDL_QUIT:
    }
  }
  
  // --- //
  
  SDL_AddTimer(1000/_engine->_frameRate,
	       PixelSpace::_FrameCallback,
	       NULL);

  return 1000/_engine->_frameRate;
}
