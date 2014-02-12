#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"

SDL_Surface *screen;
unsigned int W_WIDTH;
unsigned int W_HEIGHT;
unsigned int* W_BUFFER;

#define PIXELCOLOR(RED,GREEN,BLUE) (((RED<<16)&0xFF0000)|((GREEN<<8)&0x00FF00)|((BLUE)&0x0000FF))
#define XYTOBUFFEROFFSET(X,Y) X+((W_HEIGHT-Y)*W_HEIGHT)
#define XYTOSCREENOFFSET(X,Y,PITCH) X+((W_HEIGHT-Y)*(PITCH/4))

unsigned int charX;
unsigned int charY;

void render()
{   
  // Lock surface if needed
  if (SDL_MUSTLOCK(screen)) 
    if (SDL_LockSurface(screen) < 0) 
      return;

  // Ask SDL for the time in milliseconds
  int tick = SDL_GetTicks();

  // Declare a couple of variables
  int i, j, yofs, ofs;

  memset(W_BUFFER,0,sizeof(unsigned int)*W_WIDTH*W_HEIGHT);
  
  W_BUFFER[XYTOBUFFEROFFSET(charX,charY)] = PIXELCOLOR(0x40,0xFF,0xA0);

  for(int x = 0; x < W_WIDTH; x++)
    for(int y = 0; y < W_HEIGHT; y++)
      ((unsigned int*)screen->pixels)[XYTOSCREENOFFSET(x,y,screen->pitch)] = W_BUFFER[XYTOBUFFEROFFSET(x,y)];

  // Unlock if needed
  if (SDL_MUSTLOCK(screen)) 
    SDL_UnlockSurface(screen);

  // Tell SDL to update the whole screen
  SDL_UpdateRect(screen, 0, 0, W_WIDTH, W_HEIGHT);    
}

void shutDown() {
  free(W_BUFFER);
}

// Entry point
int main(int argc, char *argv[])
{
  if(argc<3) {
    printf("Example usage: ./pixelspace SCREEN_WIDTH SCREEN_HEIGHT\n");
    exit(1);
  }

  W_WIDTH = atoi(argv[1]);
  W_HEIGHT = atoi(argv[2]);

  charX = W_WIDTH/2;
  charY = W_HEIGHT/2;

  W_BUFFER = (unsigned int*)malloc(sizeof(unsigned int)*W_WIDTH*W_HEIGHT);
  memset(W_BUFFER,0,sizeof(unsigned int)*W_WIDTH*W_HEIGHT);

  // Initialize SDL's subsystems - in this case, only video.
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
    {
      fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
      exit(1);
    }

  // Register SDL_Quit to be called at exit; makes sure things are
  // cleaned up when we quit.
  atexit(SDL_Quit);
    
  // Attempt to create a W_WIDTHxW_HEIGHT window with 32bit pixels.
  screen = SDL_SetVideoMode(W_WIDTH, W_HEIGHT, 32, SDL_SWSURFACE);
  
  // If we fail, return error.
  if ( screen == NULL ) 
    {
      fprintf(stderr, "Unable to set W_WIDTHxW_HEIGHT video: %s\n", SDL_GetError());
      exit(1);
    }

  // Main loop: loop forever.
  while (1)
    {
      // Render stuff
      render();

      // Poll for events, and handle the ones we care about.
      SDL_Event event;
      while (SDL_PollEvent(&event)) 
	{
	  switch (event.type) 
	    {
	    case SDL_KEYDOWN:
	      if (event.key.keysym.sym == SDLK_DOWN) charY -= 5;
	      break;
	    case SDL_KEYUP:
	      // If escape is pressed, return (and thus, quit)
	      if (event.key.keysym.sym == SDLK_ESCAPE)
		return 0;
	      break;
	    case SDL_QUIT:
	      return(0);
	    }
	}
    }
  return 0;
}
