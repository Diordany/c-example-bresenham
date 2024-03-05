/*
MIT License

Copyright (c) 2024 Diordany van Hemert

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// C headers:
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// SDL headers:
#include <SDL2/SDL.h>

// Example headers:
#include <example/line-renderer.h>

#define DISP_WIDTH 800
#define DISP_HEIGHT 600

#define PADDING_H 20
#define PADDING_V 20

int main(void)
{
  // Initialize SDL.
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO))
  {
    fputs("Couldn't initialize SDL.", stderr);
    return EXIT_FAILURE;
  }

  // Create an SDL window.
  SDL_Window *window = SDL_CreateWindow("Bresenham Line Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        DISP_WIDTH, DISP_HEIGHT, 0);

  if (window == NULL)
  {
    fputs("Couldn't create an SDL window.", stderr);
    return EXIT_FAILURE;
  }

  // Create an SDL renderer.
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  if (renderer == NULL)
  {
    fputs("Couldn't create an SDL renderer.", stderr);
    return EXIT_FAILURE;
  }

  // Generate a random line.
  srand(time(NULL));

  int x1 = PADDING_H + rand() % (DISP_WIDTH - 2 * PADDING_H);
  int y1 = PADDING_V + rand() % (DISP_HEIGHT - 2 * PADDING_V);

  SDL_Rect startMarker = { .x = x1 - 1, .y = y1 - 1, .w = 3, .h = 3 };

  int x2 = PADDING_H + rand() % (DISP_WIDTH - 2 * PADDING_H);
  int y2 = PADDING_V + rand() % (DISP_HEIGHT - 2 * PADDING_V);

  SDL_Rect endMarker = { .x = x2 - 1, .y = y2 - 1, .w = 3, .h = 3 };

  printf("Line: (%d, %d) -- (%d, %d)\n", x1, y1, x2, y2);

  SDL_Event event;
  bool running = true;

  // Main loop.
  while (running)
  {
    // Wait for the quit signal.
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        running = false;
      }
    }

    // Fill the screen with black.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Mark the start with red.
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &startMarker);

    // Mark the end with green.
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &endMarker);

    // Set the line in white.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    render_bresenham_line(renderer, x1, y1, x2, y2);

    // Refresh the surface.
    SDL_RenderPresent(renderer);

    // Sleep a little to prevent the program from eating up CPU resources.
    SDL_Delay(10);
  }

  return EXIT_SUCCESS;
}