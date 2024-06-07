#include "draw.h"



bool init();
void close();

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gTexture = NULL;
SDL_Surface *loadedSurface = NULL;

bool init()
{
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    gWindow = SDL_CreateWindow("AOKG Lab 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      } else {
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      }
    }
  }
  return success;
}

void close()
{
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;
  SDL_Quit();
}

int main(int argc, char *argv[])
{
  if (!init()) {
    printf("Failed to initialize!\n");
  } else {
    loadedSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
      0x00FF0000,// R
      0x0000FF00,// G
      0x000000FF,// B
      0x00000000);// alpha

    gTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    SDL_FillRect(loadedSurface, NULL, RGB32(0, 0, 0));
    if (NULL == gTexture) {
      printf("Failed to load media!\n");
    } else {
      bool quit = false;
      SDL_Event e;
      MODE mode = Inside;
      Circle circle;
      circle.set_points();
      Rectangle window;
      window.set_points();

      while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
          if (SDL_QUIT == e.type) {
            quit = true;
          }
          if (SDL_KEYDOWN == e.type) {
            switch (e.key.keysym.scancode) {
            case SDL_SCANCODE_RIGHT:
              printf("SDL_SCANCODE_RIGHT have been presssed\n");
              circle.get_affine_koef(1, -10);
              break;
            case SDL_SCANCODE_LEFT:
              printf("SDL_SCANCODE_LEFT have been presssed\n");
              circle.get_affine_koef(1, 10);
              break;
            case SDL_SCANCODE_UP:
              printf("SDL_SCANCODE_UP have been presssed\n");
              circle.get_affine_koef(2, -10);
              break;
            case SDL_SCANCODE_DOWN:
              printf("SDL_SCANCODE_DOWN have been presssed\n");
              circle.get_affine_koef(2, 10);
              break;
            case SDL_SCANCODE_Q:
              printf("SDL_SCANCODE_KP_PLUS have been presssed\n");
              circle.get_affine_koef(3, 5);
              break;
            case SDL_SCANCODE_E:
              printf("SDL_SCANCODE_KP_MINUS have been presssed\n");
              circle.get_affine_koef(3, -5);
              break;
            case SDL_SCANCODE_KP_PLUS:
              printf("SDL_SCANCODE_KP_PLUS have been presssed\n");
              circle.get_affine_koef(4, 0.01);
              break;
            case SDL_SCANCODE_KP_MINUS:
              printf("SDL_SCANCODE_KP_MINUS have been presssed\n");
              circle.get_affine_koef(4, -0.01);
              break;
            case SDL_SCANCODE_1:
              printf("SDL_SCANCODE_1 have been presssed\n");
              mode = Inside;
              break;
            case SDL_SCANCODE_2:
              printf("SDL_SCANCODE_2 have been presssed\n");
              mode = Outside;
              break;
            case SDL_SCANCODE_3:
              printf("SDL_SCANCODE_3 have been presssed\n");
              mode = Visible;
              break;
            case SDL_SCANCODE_EQUALS:
              printf("SDL_SCANCODE_+ have been presssed\n");
              window.get_affine_koef(4, 0.01);
              break;
            case SDL_SCANCODE_MINUS:
              printf("SDL_SCANCODE_- have been pressed\n");
              window.get_affine_koef(4, -0.01);
              break;
            case SDL_SCANCODE_KP_8:
              printf("SDL_SCANCODE_KP_8 have been presssed\n");
              window.get_affine_koef(2, -10);
              break;
            case SDL_SCANCODE_KP_2:
              printf("SDL_SCANCODE_KP_2 have been presssed\n");
              window.get_affine_koef(2, 10);
              break;
            case SDL_SCANCODE_ESCAPE:
              quit = true;
              break;
            default:
              break;
            }
          }
        }
        SDL_RenderClear(gRenderer);

        SDL_FillRect(loadedSurface, NULL, RGB32(0, 0, 0));

        draw(loadedSurface, circle, window, mode);

        SDL_UpdateTexture(gTexture, NULL, loadedSurface->pixels, loadedSurface->pitch);
        SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
        SDL_RenderPresent(gRenderer);
      }
    }
  }
  close();
  return 0;
}


