# Example: Bresenham Line Renderer (C)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

I want to demonstrate the line renderer presented by Bresenham in [[1]](#1), therefore I've implemented the algorithm in C as a software renderer. The window management and drawing of individual pixels is handled by SDL2 in this demonstration.

# Building & Running

## Dependencies

- SDL2

To install the dependencies on Arch Linux, run:
```
sudo pacman -S sdl2
```

## Building

To build the example, run:

```
make
```

To rebuild, run:

```
make rebuild
```

To remove the object files and the executable, run:

```
make clean
```

## Running

Run the example with:

```
./bin/bresenham
```

<p align="center">
  <img width="800" height="600" src="https://github.com/Diordany/c-example-bresenham/assets/54911023/05dafb69-7ebb-444a-ae40-c2f266c18abb">
</p>

The start and end of the line are marked with red and green respectively.

# Framework

Before we delve into the implementation of the Bresenham renderer, I want to give a brief overview of the framework for this example.

The window management and drawing is handled by SDL2. First, an SDL window is created, which is accessed with a pointer of type `SDL_Window *`. With the window, an SDL renderer is created that can be accessed with a pointer of type `SDL_Renderer *`. The SDL renderer can actually render lines already with `SDL_RenderDrawLine()`, but for this example I'm going to use my own software implementation. The SDL renderer is only used to draw individual pixels and to render markers (to point out both ends of the line). SDL2 is also used to control the main loop of the example.

At the start of the run, two random points are generated to indicate the start and end of the line. This is done with the standard C library.

# The Line Renderer

The renderer must first figure out in which octant with respect to $(x_1, y_1)$ it has to operate. To do so, the slope of the line must be taken into account [[1]](#1). When the octant is known, the renderer sets up a frame of reference to draw the line.

## Calculating the Slope
Let's consider the mathematical representation of a linear function:

$$y = \frac{\Delta y}{\Delta x}(x - x_0) + y_0 = \frac{y_2 - y_1}{x_2 - x_1}(x - x_0) + y_0$$

In this case, we can see that the slope is $\frac{\Delta y}{\Delta x}$, so we can start off by calculating the displacement $(\Delta x, \Delta y)$ from the starting point $(x_1, y_1)$ to the end of the line $(x_2, y_2)$:

$$\Delta x = x_2 - x_1$$

$$\Delta y = y_2 - y_1$$

```c
int xDiff = p_x2 - p_x1;
int yDiff = p_y2 - p_y1;
```

The absolute values $|\Delta x|$ and $|\Delta y|$ are then calculated, so we can determine the steepness of the slope [[1]](#1):

```c
int xDiffAbs = (xDiff < 0) ? -xDiff : xDiff;
int yDiffAbs = (yDiff < 0) ? -yDiff : yDiff;
```

## Determining the Octant

Now that we know the relevant details of the slope, we can figure out in which octant we should operate. We'll introduce the boolean variables $X$, $Y$ and $Z$ [[1]](#1).

We use $X$ to test whether the $x$ displacement is positive or not.

$$X = (\Delta x \ge 0)$$

```c
bool posXDiff = xDiff >= 0;
```

We use $Y$ to test whether the $y$ displacement is positive or not.

$$Y = (\Delta y \ge 0)$$

```c
bool posYDiff = yDiff >= 0;
```

We use $Z$ to test whether the slope is shallow or steep with respect to the x-axis (true if shallow and false if steep).

$$Z = (|\Delta x| - |\Delta y| \ge 0)$$

```c
bool shallow = xDiffAbs - yDiffAbs >= 0;
```

We can map $X$, $Y$ and $Z$ to the octant with the following table [[1]](#1):

| X | Y | Z | Octant |
|:-:|:-:|:-:|:------:|
| 0 | 0 | 0 | 6      |
| 0 | 0 | 1 | 5      |
| 0 | 1 | 0 | 3      |
| 0 | 1 | 1 | 4      |
| 1 | 0 | 0 | 7      |
| 1 | 0 | 1 | 8      |
| 1 | 1 | 0 | 2      |
| 1 | 1 | 1 | 1      |

## Setting the Frame of Reference

Now that we are able to determine the octant, we can set the frame of reference of the renderer accordingly. The frame of reference has an a- and b-axis, how this relates to the x- and y-axis depends on the octant.

First, let's declare the relevant variables:

```c
int a, b;
int *x, *y;
int aDiff, bDiff;
int aInc, bInc;
int aTerm;
```

The first pair (`a` and `b`) represents a point $(a_i, b_i)$ in the frame of reference. The initial point $(a_1, b_1)$ is determined by:

$$a_1 = \begin{cases}
  x_1 & \quad\text{if}\quad |\Delta x| - |\Delta y| \ge 0\\
  y_1 & \quad\text{if}\quad |\Delta x| - |\Delta y| \lt 0\\
\end{cases}$$

$$b_1 =
\begin{cases}
  y_1 & \quad\text{if}\quad |\Delta x| - |\Delta y| \ge 0\\
  x_1 & \quad\text{if}\quad |\Delta x| - |\Delta y| \lt 0\\
\end{cases}$$

The important detail to note here is that the a- and b-axis fall along the x- and y-axis respectively if the slope is shallow. The axes are inverted if the slope is steep. All other variables are determined in a similar fashion.

The second pair variables (`*x` and `*y`) are pointers that are used to translate the point $(a_i, b_i)$ in the frame of reference to a point $(x_i, y_i)$ in the screenspace of SDL.

The third pair (`aDiff` and `bDiff`) represents the displacement $(\Delta a, \Delta b)$ in the frame of reference. In theory, the renderer only moves in the positive a and b directions, therefore we have to make sure that we use the absolute values of $\Delta x$ and $\Delta y$ for the assignment [[1]](#1). The values are determined with the following expressions:

$$\Delta a =
\begin{cases}
  |\Delta x| & \quad\text{if}\quad |\Delta x| - |\Delta y| \ge 0\\
  |\Delta y| & \quad\text{if}\quad |\Delta x| - |\Delta y| \lt 0\\
\end{cases}$$

$$\Delta b =
\begin{cases}
  |\Delta y| & \quad\text{if}\quad |\Delta x| - |\Delta y| \ge 0\\
  |\Delta x| & \quad\text{if}\quad |\Delta x| - |\Delta y| \lt 0\\
\end{cases}$$

The fourth pair (`aInc` and `bInc`) is used to make sure that the a- and b-axis point in the appropriate direction in the SDL screen space. Their values are determined by:

$$a_{+} =
\begin{cases}
  1 & \quad\text{if}\quad (X \cap Z) \cup (Y \cap \overline{Z}) \\
  -1 & \quad\text{otherwise}
\end{cases}$$

$$b_{+} =
\begin{cases}
  1 & \quad\text{if}\quad (Y \cap Z) \cup (X \cap \overline{Z}) \\
  -1 & \quad\text{otherwise}
\end{cases}$$

Earlier I mentioned that the renderer only moves in the positive a and b directions in theory. You can see here that it's not necessarily true in practice, as the coordinates are prematurely converted to SDL screen space for convenience. The renderer still works according to the theory in [[1]](#1) however. In the end, the increments are still moving the points towards what are considered the positive a and b directions of the frame of reference.

The expression can be further simplified if the slope is tested beforehand, which is the case in my implementation. For shallow slopes, the expressions are:

$$a_+ =
\begin{cases}
  1 & \quad\text{if}\quad \Delta x \ge 0\\
  -1 & \quad\text{if}\quad \Delta x \lt 0
\end{cases}$$

$$b_+ =
\begin{cases}
  1 & \quad\text{if}\quad \Delta y \ge 0\\
  -1 & \quad\text{if}\quad \Delta y \lt 0
\end{cases}$$

For steep slopes, the expressions are:

$$a_+ =
\begin{cases}
  1 & \quad\text{if}\quad \Delta y \ge 0\\
  -1 & \quad\text{if}\quad \Delta y \lt 0
\end{cases}$$

$$b_+ =
\begin{cases}
  1 & \quad\text{if}\quad \Delta x \ge 0\\
  -1 & \quad\text{if}\quad \Delta x \lt 0
\end{cases}$$

Finally the last variable `aTerm` is used to terminate the drawing procedure. It sits just outside the bounds of the line and is defined by:

$$a_{n + 1} =
\begin{cases}
  x_2 + a_+ & \quad\text{if}\quad |\Delta x| - |\Delta y| \ge 0\\
  y_2 + a_+ & \quad\text{if}\quad |\Delta x| - |\Delta y| \lt 0
\end{cases}$$

The final value of `a` is given by:

$$a_n =
\begin{cases}
  x_2 & \quad\text{if}\quad |\Delta x| - |\Delta y| \ge 0\\
  y_2 & \quad\text{if}\quad |\Delta x| - |\Delta y| \lt 0
\end{cases}$$

To put all of this verbose nonsense into practice, I've written the following:

```c
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
```

## The Rendering Procedure

Now that the frame of reference is set, we can move on to the rendering procedure. Let's start with the $a_i$ variable, which really simple actually, because the procedure just iterates over the range of $[a_1, a_n]$.

The $b_i$ value requires more explanation. Bresenham [[1]](#1) presented a clever method to figure out the relation between $b_i$ and $a_i$ without floating point arithmetic and without multiplication or division. For every value of $a_i$, the algorithm should decide whether or not to increment the value of $b_i$ [[1]](#1). The decision is based on which point is closer to the ideal line. The distance between the ideal line and the points $(a_i, b_{i - 1})$ and $(a_i, b_{i - 1} + b_+)$ are given by $r$ and $q$ respectively. So the new value of $b_i$ is determined with:

$$b_i =
\begin{cases}
  b_{i - 1} + b_+ & \quad\text{if}\quad r - q \ge 0\\
  b_{i - 1} & \quad\text{if}\quad r - q \lt 0\\
\end{cases}$$

However, we're not going to figure out the values of $r$ and $q$, because Bresenham derived a more convenient way to test whether $r - q$ is positive or not [[1]](#1). Deriving the equation is beyond the scope of this example, but the paper does an excellent job at describing how to derive it. Bresenham expressed the sign of $r - q$ in terms of $\Delta a$ and $\Delta b$, which is convenient, because we already figured those out. The following recursive relation was presented in [[1]](#1):

$$\delta_{i + 1} =
\begin{cases}
  \delta_i + 2\Delta b - 2\Delta a & \quad\text{if}\quad \delta_i \ge 0\\
  \delta_i + 2\Delta b & \quad\text{if}\quad \delta_i \lt 0\\
\end{cases}$$

For which the initial value is set with [[1]](#1):

$$\delta_1 = 2\Delta b - \Delta a$$

Utilizing this expression, the value of $b_i$ can now be determined with:

$$b_i =
\begin{cases}
  b_{i - 1} + b_+ & \quad\text{if}\quad \delta_i \ge 0\\
  b_{i - 1} & \quad\text{if}\quad \delta_i \lt 0\\
\end{cases}$$

After closer examination, we can see that that the equations for $\delta_1$ and $\delta_{i + 1}$ are very similar to the edge function from [[2]](#2). Let's consider the mathematical representation of a linear function again, but this time for the frame of reference:

$$b = \frac{\Delta b}{\Delta a}(a - a_0) + b_0$$

If we rewrite this, we get a special case of the edge function from [[2]](#2), where the function is equal to 0:

$$\Delta b (a - a_0) - \Delta a (b - b_0) = 0$$

The edge function detects how much the point $(a, b)$ deviates from the ideal line [[2]](#2). It therefore makes sense that the function would output 0 for every point in the linear function. So let's now consider a more general form of the edge function:

$\delta(a^\prime,b^\prime) = \Delta b(a^\prime - a_0) - \Delta a(b^\prime - b_0)$

We can now see the relation between $\delta_1$ and $\delta_{i + 1}$ and the edge function from [[2]](#2):

$$\delta_1 = \delta(a_0 + 2, b_0 + 1) = 2\Delta b - \Delta a$$

$$\delta_{i + 1} =
\begin{cases}
  \delta_i + \delta(a_0 + 2, b_0 + 2) = \delta_i + 2\Delta b - 2\Delta a & \quad\text{if}\quad \delta_i \ge 0\\
  \delta_i + \delta(a_0 + 2, b_0) = \delta_i + 2\Delta b & \quad\text{if}\quad \delta_i \lt 0\\
\end{cases}$$

As you can see, expression for $\delta_{i + 1}$ compensates for incrementing the $a_i$ and $b_i$ variables, while $\delta_1$ sets an initial offset.

Okay, enough ramling for now, let's see the actual C implementation:

```c
int error = bDiff + bDiff - aDiff;

while (a != aTerm)
{
  SDL_RenderDrawPoint(p_renderer, *x, *y);

  a += aInc;
  error += bDiff + bDiff;

  if (error >= 0)
  {
    b += bInc;
    error -= aDiff + aDiff;
  }
}
```

# Conclusion

For such a simple concept as drawing a line, the Bresenham algorithm turned out to be more complex than I initially thought, and I learned a lot during the process of implementing it as a software renderer. Also, deriving the equations myself gave me an opportunity to dust off my mathematical skills. I'm fascinated with image and video rendering techniques, and I'd like to learn a lot more about them in the future by working on projects like this.

# References

<a id="1">[1]</a> 
&nbsp; J. E. Bresenham, "Algorithm for computer control of a digital plotter" *IBM Systems Journal*, 1965.

<a id="1">[2]</a>
&nbsp; J. Pineda, "A Parallel Algorithm for Polygon Rasterization" *Computer Graphics*, vol. 22, no. 4, Aug. 1988.
