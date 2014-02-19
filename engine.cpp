#include "engine.h"

#define ERROR(ERRORSTRING) printf(ERRORSTRING"\n"); exit(1);

PixelSpace* PixelSpace::_engine = NULL;
bool PixelSpace::running = false;
Uint32 PixelSpace::ticks = 0;

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
    // --- TEST --- //
    for(int i = 0; i < 100; i++) {
      _engine->spaceObjects.push_front(new SpaceObject(0,
						       0,
						       ((rand()%100)-50)*0.001,
						       ((rand()%100)-50)*0.001));
    }
    // --- TEST --- //
    running = true;
  }
    return _engine;
}

bool PixelSpace::ShutDown() {
  return false;
}

void PixelSpace::FillScreen(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  SDL_FillRect(_screen, NULL, SDL_MapRGBA(_screen->format,r,g,b,a));
}

Uint32 PixelSpace::Tick() {
  // --- TEST --- //
  for(std::list<SpaceObject*>::const_iterator i = _engine->spaceObjects.begin();
      i != _engine->spaceObjects.end();
      ++i) (*i)->Tick();

  // --- TEST --- //
  // --- //
  this->ticks++;
  return this->ticks;
}

void PixelSpace::DrawPixel(double x, double y,
			   Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    unsigned int ix =(unsigned int)x + (_screenWidth/2);
    unsigned int iy =(unsigned int)y + (_screenHeight/2);
    if(ix>0 && iy>0 && ix<_screenWidth && iy<_screenHeight){
      unsigned int offset = ix;
      offset += ((_screenHeight-1)-iy)*(_screen->pitch/4);
      ((unsigned int*)_screen->pixels)[offset] = SDL_MapRGBA(_screen->format,
							     r,g,b,a);
    }
}

Uint32 PixelSpace::_FrameCallback(Uint32 interval,
				  void* param) {
  if (SDL_MUSTLOCK(_engine->_screen)) 
    if (SDL_LockSurface(_engine->_screen) < 0) 
      return 0;
  Uint32 tick = SDL_GetTicks();

  while(_engine->ticks < tick) _engine->Tick();

  // --- //
  
  _engine->FillScreen(0,0,0,0);
  
  // --- TEST --- //
  //for(int i = 0; i < 100; i++)
  //_engine->spaceObjects[i]->Render();
  for(std::list<SpaceObject*>::const_iterator i = _engine->spaceObjects.begin();
      i != _engine->spaceObjects.end();
      ++i) (*i)->Render();
  // --- TEST --- //

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
      if (event.key.keysym.sym == SDLK_ESCAPE) {
	running = false;
	return 0;
      }
      break;
      //case SDL_QUIT:
    }
  }
  
  // --- //

  return 1000/_engine->_frameRate;
}

SpaceObject::SpaceObject(double x, double y, double xAccel, double yAccel) {
  this->x = x;
  this->y = y;
  this->xAccel = xAccel;
  this->yAccel = yAccel;
  this->ticks = 0;
}

Uint32 SpaceObject::Tick() {
  x += xAccel;
  y += yAccel;

  ticks++;
  return ticks;
}

void SpaceObject::Render() {
  PixelSpace::Engine()->DrawPixel(x,y,0xff,0xff,0xff,0xff);
}
