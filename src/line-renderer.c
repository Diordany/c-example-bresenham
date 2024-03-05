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

// SDL headers:
#include <SDL2/SDL.h>

void render_bresenham_line(SDL_Renderer *p_renderer, int p_x1, int p_y1, int p_x2, int p_y2)
{
  // Cancel if no renderer was given.
  if (p_renderer == NULL)
  {
    return;
  }

  // Calculate the displacement.
  int xDiff = p_x2 - p_x1;
  int yDiff = p_y2 - p_y1;

  // Get the absolute values of the differences.
  int xDiffAbs = (xDiff < 0) ? -xDiff : xDiff;
  int yDiffAbs = (yDiff < 0) ? -yDiff : yDiff;

  // Decide in which octant the line should be rendered.
  //
  // posXDiff: the x-displacement is positive.
  // posYDiff: the y-displacement is positive.
  // shallow: the line is shallow with respect to the x-axis.
  //
  // Octant 1: posXDiff && posYDiff && shallow    [1 1 1]
  // Octant 2: posXDiff && posYDiff && !shallow   [1 1 0]
  // Octant 3: !posXDiff && posYDiff && !shallow  [0 1 0]
  // Octant 4: !posXDiff && posYDiff && shallow   [0 1 1]
  // Octant 5: !posXDiff && !posYDiff && shallow  [0 0 1]
  // Octant 6: !posXDiff && !posYDiff && !shallow [0 0 0]
  // Octant 7: posXDiff && !posYDiff && !shallow  [1 0 0]
  // Octant 8: posXDiff && !posYDiff && shallow   [1 0 1]
  bool posXDiff = xDiff >= 0;
  bool posYDiff = yDiff >= 0;
  bool shallow = xDiffAbs - yDiffAbs >= 0;

  // Frame of reference.

  int a, b;
  int *x, *y;
  int aDiff, bDiff;
  int aInc, bInc;
  int aTerm;

  if (shallow)
  {
    a = p_x1;
    b = p_y1;

    x = &a;
    y = &b;

    aDiff = xDiffAbs;
    bDiff = yDiffAbs;

    aInc = (posXDiff) ? 1 : -1;
    bInc = (posYDiff) ? 1 : -1;

    aTerm = p_x2 + aInc;
  }
  else
  {
    a = p_y1;
    b = p_x1;

    x = &b;
    y = &a;

    aDiff = yDiffAbs;
    bDiff = xDiffAbs;

    aInc = (posYDiff) ? 1 : -1;
    bInc = (posXDiff) ? 1 : -1;

    aTerm = p_y2 + aInc;
  }

  // The sign of this value decides whether or not b should be incremented.
  int sign = bDiff + bDiff - aDiff;

  // Draw until the end of the line is reached.
  while (a != aTerm)
  {
    SDL_RenderDrawPoint(p_renderer, *x, *y);

    a += aInc;

    if (sign >= 0)
    {
      b += bInc;
      sign += bDiff + bDiff - aDiff - aDiff;
    }
    else
    {
      sign += bDiff + bDiff;
    }
  }
}