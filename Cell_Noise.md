# Introduction #
Cell Noise, also called Worley Noise, is a very simple yet fascinating algorithm. It ends up being slower than Perlin\_Noise, but is still relatively fast. The quality is not compatible with our current algorithms, however it creates fascinating patterns that can be used for certain landscapes

# Contents #



# Algorithm #
Cell Noise is a very simple algorithm, but can yield incredible results. The algorithm only has two parts:

  1. Set up an array of points around the image
  1. For any pixel, the value is the distance from the n<sup>th</sup> closest point.

This may sound simple, but there are a couple wrinkles. First, if you are checking which point is closest to a given pixel, it would be very expensive to find the distance to every single point. The easier way to do it is to set up an arbitrary grid and only find the distance to the points within the current square and the neighboring squares. The algorithm generally has 0 to 5 points per square, set up and kept in memory. In my implementation, each square has exactly one point within it. This may reduce the randomness slightly, but allowed for greater speed.

The second wrinkle is how to track the points. An array can be set up to list each point in each square. This gets a little tricky when using Fractional\_Brownian\_Motion, as each octave will have an increasing amount of points and squares. In my implementation, since each square has exactly one point I use a special random number generator to get the point. If asked about a given square, the RNG will always return the same values. This way no points have to be stored in memory.

As I said before, to find the closest point, it is important to not just search the current square but to also search the surrounding squares. So in 2D, it could look something like this:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/cellgrid.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/cellgrid.png)

The white point with the arrow is the current pixel. The green square is the local square, which is most likely to contain the closest point. However, as the diagram shows, it is not uncommon for the closest point to be within a neighboring square (marked in yellow). In 2D, the algorithm has to search a total of 9 squares, comparing the distances to every point within them. In many implementations the distances are kept in a list, that way first, second, and third closest points (and even out further) can be used. I don't do this in my implementation, but it would be an easy addition.

Quite often it will be the point within the local square, so there are shortcuts that can be taken. One shortcut is to see if other squares are even eligible. First, get the distance from the closest point within the local square. For each surrounding square, get the distance to the closest edge or corner. If that distance is greater than the distance to a local point, don't check any of the points within that square. In implementations where a square can have 5 or more points, this can save a lot of time. In my implementation where each square has 1 point, it would only slow performance, so I did not include this shortcut.

In all it is pretty simple, but the simplicity allows for a lot of experimentation. A few things can be mixed up.

  * The distance function
  * Which point to use for the distance function
  * How to combine the above two

First, there are many different distance functions that can be used. First and easiest to grasp is the **Euclidean** function:

```
    x_difference = pixel.x - closest_point.x;
    y_difference = pixel.y - closest_point.y;

    distance = sqrt(x_difference * x_difference + y_difference * y_difference);
```

This is just the Pythagorean Theorem: a<sup>2</sup> + b<sup>2</sup> = c<sup>2</sup>. A variation on this is **Euclidean-Squared**, which is the same but leaves off the square-root function, as this is computationally very slow. This creates steeper slopes, but still gives roughly the same image at a better speed.

Next is the **Manhattan** function, also called the City Block function:

```
    x_difference = pixel.x - closest_point.x;
    y_difference = pixel.y - closest_point.y;

    distance = abs(x_difference) + abs(y_difference);
```

This creates diamond-shaped patterns that don't make very good terrains, but are still interesting. There are many less common functions (a couple of which are in the source code), and anyone can create their own distance function.

Next we can vary which point is chosen. Most often it is the closest, but it can also be the second-closest, or third-closest, or even 15th-closest. This is denoted by an F and then the number. So using the first-closest point is denoted by F1, the second-closest point is denoted by F2, the 15th-closest point is denoted by F15, and so on. Depending on the distance function, the output can change dramatically between choices.

Finally, we can combine these in interesting ways. For example, deciding a point based on Euclidean F2-F1 (the output of the second-closest point minus the output of the closest point distance functions) produces very crisp lines, as can be seen below. There are no bounds on the combinations; it is even possible to use cell noise values as seeds in other algorithms, or in cell noise itself. There are infinite possibilities, but most don't look very good. Here are some of the more common approaches:

# Gallery #
I don't normally do a gallery, but this is a very versatile algorithm.

Euclidean F1:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1.png)

Euclidean F2:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F2.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F2.png)

Euclidean F2 - F1:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F2-F1.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F2-F1.png)

Euclidean F1 + F2:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1+F2.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1+F2.png)

Euclidean F1 - F2:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1-F2.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1-F2.png)

Euclidean F1 `*` F2:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1.F2.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/F1.F2.png)

Euclidean F1 with Fractional\_Brownian\_Motion in black and white:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/blacknwhitefbm.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/blacknwhitefbm.png)

Euclidean F1 with Fractional\_Brownian\_Motion colorized:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/colorfbm.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/colorfbm.png)

Manhattan F1:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/manhattanF1.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/manhattanF1.png)

Manhattan F2:

![http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/manhattanF2.png](http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/manhattanF2.png)


# Comparison #

## Pros ##
  * Pretty Fast
  * Fascinating output
  * Highly customizable
  * Easy to understand and easy to implement
  * Like Value\_Noise and Perlin\_Noise, very little has to be stored in memory.

## Cons ##
  * Slower than Perlin\_Noise and Simplex\_Noise
  * Doesn't look as random as the other algorithms: there are definitive shapes

# Ideas #
Because it is so easy to customize this algorithm and it has reasonably fast output, I'd like to create a genetic algorithm that would learn to create more interesting images. The color gradient, distance function, point-selection, and combinations could all be altered with fascinating results. The fitness function would either be human-based (which would be very slow) or I'll have to come up with something clever to objectively grade how interesting an image is.

One interesting possibility with Cell Noise is creating Voronoi Diagrams, also called Closest Neighbor diagrams. Examples can be seen here: [Voronoi](http://en.wikipedia.org/wiki/Voronoi_diagram). These diagrams can be created with Cell Noise by assigning a color to a pixel based on the ID of the closest point, not the distance to it. This creates chunks of colors. This would require a slight overhaul of my code, as ID's aren't assigned and colors are carefully chosen. I will probably do it at some point though.

# Conclusions #
In the other algorithms, output has been relatively featureless: it always looked like a wavy sheet, which when combined with Fractional\_Brownian\_Motion creates terrain-like fractals. With Cell Noise there are always definitive shapes and lines, which doesn't appear to create very convincing terrain. In the above colored Euclidean F1 with fBm, it looks very rounded and hilly, like a head of cauliflower (although cauliflower is technically a natural fractal). On the other hand, output like Euclidean F2 or Euclidean F2-F1 looks a lot like a system of canyons. While it doesn't fit with my current look, I will definitely be saving it for other types of terrain and for purely artistic images. In all, a simple yet fascinating algorithm.

Click here to see it in action:
http://gimli.morningside.edu/~tra001/Algorithms/CellNoise/CellNoise.cgi