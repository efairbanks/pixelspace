#include "engine.hpp"

#define ERROR(ERRORSTRING) printf(ERRORSTRING"\n"); exit(1);

PixelSpace* PixelSpace::_engine = NULL;
bool PixelSpace::running = false;
Uint32 PixelSpace::ticks = 0;

// --- test --- //
SDL_Surface* SHIP_SURFACE;
SDL_Surface* LOGO_SURFACE;
// ------------ //

Player::Player(SDLKey moveLeft,
	       SDLKey moveRight,
	       SDLKey fire) {
  this->moveLeft = moveLeft;
  this->moveRight = moveRight;
  this->fire = fire;
  this->magnitude = 0;
  this->ship = new SpaceObject(0,0.0-(PixelSpace::Engine()->_screenHeight/3.0),0,0);
}

Uint32 Player::Tick() {
  ship->x += magnitude;
}

void Player::SetInput(SDLKey key, bool keyDown) {
  if(key==moveLeft && keyDown) magnitude = -0.1;
  if(key==moveRight && keyDown) magnitude = 0.1;
  if((key==moveRight || key==moveLeft) && !keyDown)
    magnitude = 0;
  if(key==fire && keyDown)
    new VolatilePixel(ship->x,ship->y+1,
		      magnitude,0.5,
		      0xff,0xff,0xff);
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
    SHIP_SURFACE = SDL_LoadBMP("./ship.bmp");
    LOGO_SURFACE = SDL_LoadBMP("./cacheblasters.bmp");
    LOGO_SURFACE = SHIP_SURFACE;
    SDL_ConvertSurface(SHIP_SURFACE,
		       _engine->_screen->format,
		       0);
    SDL_ConvertSurface(LOGO_SURFACE,
		       _engine->_screen->format,
		       0);
    _engine->players.push_front(new Player(SDLK_LEFT,
					   SDLK_RIGHT,
					   SDLK_UP));
    for(int i = 0; i < LOGO_SURFACE->w; i++){
      for(int j = 0; j < LOGO_SURFACE->h; j++) {
	int invj = (LOGO_SURFACE->h-1)-j;
	int offset = i + (invj*LOGO_SURFACE->pitch/4);
	SDL_Color pixelColor = ((SDL_Color*)LOGO_SURFACE->pixels)[offset];
	if(pixelColor.r+pixelColor.g+pixelColor.b)
	  new VolatilePixel(i-(LOGO_SURFACE->w/2),
			    j-(LOGO_SURFACE->h/2),
			    0,0,
			    pixelColor.r,
			    pixelColor.g,
			    pixelColor.b);
      }
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
  //SDL_FillRect(_screen, NULL, SDL_MapRGBA(_screen->format,r,g,b,a));
  for(int i = 0; i < _screen->w; i++)
    for(int j = 0; j < _screen->h; j++) {
      int offset = i + (j*_screen->pitch/4);
      SDL_Color color = ((SDL_Color*)_screen->pixels)[offset];
      color.r = (Uint8)(color.r*0.85);
      color.g = (Uint8)(color.g*0.85);
      color.b = (Uint8)(color.b*0.85);
      ((SDL_Color*)_screen->pixels)[offset] = color;
    }
}

Uint32 PixelSpace::Tick() {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      for(std::list<Player*>::const_iterator i = _engine->players.begin();
	  i != _engine->players.end();
	  i++) (*i)->SetInput(event.key.keysym.sym, true);
      break;
    case SDL_KEYUP:
      for(std::list<Player*>::const_iterator i = _engine->players.begin();
	  i != _engine->players.end();
	  i++) (*i)->SetInput(event.key.keysym.sym, false);
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
      ++i) {
    std::list<SpaceObject*>::const_iterator nexti = i;
    nexti++;
    if(abs((*i)->x) >= _engine->_screenWidth ||
       abs((*i)->y) >= _engine->_screenHeight) {
      delete (*i);
      _engine->spaceObjects.remove((*i));
      i = nexti;
    }

    (*i)->Tick();
  }
  // --- //
  this->ticks++;
  return this->ticks;
}

void PixelSpace::DrawPixel(double x, double y,
			   Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    unsigned int ix =(int)x + (_screenWidth/2);
    unsigned int iy =(int)y + (_screenHeight/2);
    if(ix>=0 && iy>=0 && ix<_screenWidth && iy<_screenHeight){
      unsigned int offset = ix;
      offset += ((_screenHeight-1)-iy)*(_screen->pitch/4);
      ((unsigned int*)_screen->pixels)[offset] = SDL_MapRGBA(_screen->format,
							     r,g,b,a);
    }
}

void PixelSpace::DrawSurface(double x, double y, SDL_Surface* surface) {
  for(int i = 0; i < surface->w; i++) {
    for(int j = 0; j < surface->h; j++) {
      int invj = (surface->h-1)-j;
      int surfaceOffset=i+(invj*surface->pitch/4);
      SDL_Color color = ((SDL_Color*)surface->pixels)[surfaceOffset];
      if(color.r+color.b+color.g)
	_engine->DrawPixel(x+i-(surface->w/2),
			   y+j-(surface->h/2),
			   color.r,
			   color.g,
			   color.b,
			   0xff);
    }
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
  PixelSpace::_engine->spaceObjects.push_front(this);
}

Uint32 SpaceObject::Tick() {
  x += xAccel;
  y += yAccel;

  ticks++;
  return ticks;
}

void SpaceObject::Render() {
  PixelSpace::Engine()->DrawSurface(x,y,SHIP_SURFACE);
}

VolatilePixel::VolatilePixel(double x, double y,
		double xAccel, double yAccel,
		Uint8 r, Uint8 g, Uint8 b) : SpaceObject(x,
							 y,
							 xAccel,
							 yAccel) {
  this->r = r;
  this->g = g;
  this->b = b;
}

void VolatilePixel::Render() {
  PixelSpace::Engine()->DrawPixel(x,y,r,g,b,0xFF);
}

Uint32 VolatilePixel::Tick() {
  Uint32 tick = SpaceObject::Tick();
  if(xAccel+yAccel!=0) {
    for(std::list<SpaceObject*>::const_iterator i = PixelSpace::Engine()->spaceObjects.begin();
	i != PixelSpace::Engine()->spaceObjects.end();
	i++) {
      std::list<SpaceObject*>::const_iterator nexti = i;
      nexti++;
      if((((int)x)==((int)(*i)->x))&&(((int)y)==((int)(*i)->y))) {
	if((*i)!=this) {
	  delete (*i);
	  PixelSpace::Engine()->spaceObjects.remove((*i));
	  i = nexti;
	}
      }
    }
  }
  return tick;
}
