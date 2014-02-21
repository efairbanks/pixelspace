#include "engine.h"

#define ERROR(ERRORSTRING) printf(ERRORSTRING"\n"); exit(1);

PixelSpace* PixelSpace::_engine = NULL;
bool PixelSpace::running = false;
Uint32 PixelSpace::ticks = 0;

Player::Player(SDLKey rotateLeft,
	       SDLKey rotateRight,
	       SDLKey boost,
	       SDLKey fire) {
  this->rotateLeft = rotateLeft;
  this->rotateRight = rotateRight;
  this->boost = boost;
  this->fire = fire;
  this->angle = 0;
  this->magnitude = 0;
  this->ship = new SpaceObject(0,0,0,0);
  PixelSpace::_engine->spaceObjects.push_front(this->ship);
}

Uint32 Player::Tick() {
  ship->xAccel += cos(angle*2*M_PI)*magnitude;
  ship->yAccel += sin(angle*2*M_PI)*magnitude;
}

void Player::SetInput(SDLKey key, bool keyDown) {
  if(key==rotateLeft && keyDown)  angle -= M_PI*2.0/8;
  if(key==rotateRight && keyDown) angle += M_PI*2.0/8;
  if(key==boost && keyDown) magnitude = 0.0001;
  if(key==boost && !keyDown) magnitude = 0;
}

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
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)) {
      ERROR("SDL_Init failure. ");}
    atexit(SDL_Quit);
    _engine->_screen = SDL_SetVideoMode(_engine->_screenWidth,
					_engine->_screenHeight,
					32,
					SDL_SWSURFACE);
    if(_engine->_screen==NULL) {
      ERROR("_screen init failure.");}
    SDL_AddTimer(1000/_engine->_frameRate,
		 PixelSpace::_FrameCallback,
		 NULL);
    // --- TEST --- //
    _engine->players.push_front(new Player(SDLK_LEFT,
					   SDLK_RIGHT,
					   SDLK_UP,
					   SDLK_DOWN));
    /*
    for(int i = 0; i < 1000; i++) {
      _engine->spaceObjects.push_front(new SpaceObject(0,
						       0,
						       ((rand()%100)-50)*0.001,
						       ((rand()%100)-50)*0.001));
    }
    */
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
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      for(std::list<Player*>::const_iterator i = _engine->players.begin();
	  i != _engine->players.end();
	  ++i) (*i)->SetInput(event.key.keysym.sym, true);
      break;
    case SDL_KEYUP:
      for(std::list<Player*>::const_iterator i = _engine->players.begin();
	  i != _engine->players.end();
	  ++i) (*i)->SetInput(event.key.keysym.sym, false);
      if (event.key.keysym.sym == SDLK_ESCAPE) {
	running = false;
	return 0;
      }
      break;
      //case SDL_QUIT:
    }
  }
  for(std::list<Player*>::const_iterator i = _engine->players.begin();
      i != _engine->players.end();
      ++i) (*i)->Tick();
  for(std::list<SpaceObject*>::const_iterator i = _engine->spaceObjects.begin();
      i != _engine->spaceObjects.end();
      ++i) (*i)->Tick();
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
