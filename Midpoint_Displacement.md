# Introduction #
Midpoint displacement is the second algorithm I have implemented for this project. It combines blazing speed with ease of implementation. However, in native form visual artifacts are abundant, although with a few tweaks these can be mostly eliminated.

# Contents #



# Algorithm #
The algorithm for midpoint displacement is actually very simple, especially when compared to its improvement the Diamond\_Square algorithm. First, create an array that represents a blank map. You begin by giving the four corners a random value. This is your starting square.

![http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/firststeps.png](http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/firststeps.png)

In this square, create the middle point by averaging the four corners and adding a small 'error', or random value. Then create the midpoints of the 4 sides by averaging the two corners each is between. After these steps, you are left with 4 squares. Repeat the steps:

1. Create the middle point by averaging the four corners and adding a small 'error'.

2. Create the midpoint of each side by averaging the two corners each point is between.

![http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/othersteps.png](http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/othersteps.png)

Each iteration, make the range of the RNG smaller. That way the original few points can have pretty large variation, but the later points only get tiny adjustments. This ensures the right amount of detail in an image. In the code I call this 'range' and adjust it by 'rangeModifier' each iteration, but I could have just as easily called them 'amplitude' and 'persistence'. In fact, much like Value\_Noise the rangeModifier is 0.65. This value seems to strike the right balance between large and small details.

In all, this algorithm is quite easy to implement. The 'working' chunk of the code is pretty light, then I have my other modules for printing out the image from the array.

However, this vanilla form creates some striking visual artifacts:

![http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/vanilla.png](http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/vanilla.png)

This is at 400% zoom for clarity, but many artifacts are visible from 100% zoom. This is caused by the square shaped paths the algorithm takes. One point where this occurs the most is at the side-midpoints. Originally I didn't add any error to the side-midpoints and just averaged the two corners. By adding an error to these as well, the severity of the artifacts is reduced:

![http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/modified.png](http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/modified.png)

While artifacts can still be found, from 100% they are generally too small to notice.

One last point concerning the algorithm must be made. You may have noticed that the example above is 9 x 9 pixels. This is necessary. In fact, because of the dividing nature of the algorithm, the map must be 2<sup>x</sup> + 1 pixels wide and tall. It is possible to make rectangular maps, but they require extra setup. So maps must be 65x65, 129x129, 257x257, 513x513, 1025x1025, 2049x2049, etc.

# Comparison #

## Pros ##
  * Blazingly fast. Less than 1 second for 1025 x 1025 images.
  * Easy to implement.
  * Decent quality by itself.
  * Good quality with the adjustments.

## Cons ##
  * The map **must** be stored in memory, because pixels reference other pixels.
  * Because it must be stored in memory, memory becomes a constraint. Most OS's won't allow an image beyond 1025 x 1025 without some fancy footwork (Windows won't allow one beyond 513 x 513).
  * The vanilla form has horrible visual artifacts, but this can be avoided.
  * The map must have a width and height of 2<sup>x</sup> + 1 pixels.

# Ideas #
Because this code is so fast, it could be used in real-time to create maps for games on-the-fly. At some point I would like to implement a 4-dimensional version of this or Diamond\_Square (with the fourth dimension being time). This would create an evolving landscape, or changing clouds. However Diamond\_Square may be difficult to extend into another dimension. Regular midpoint displacement may be simple enough to implement in 4D.

# Conclusions #
This algorithm is lightning-fast and can produce acceptable quality. It may also scale well into 4D. However, it is a memory hog, which limits what it can do. It is possible to step around these constraints, but at the cost of speed. Diamond\_Square is an improvement upon this algorithm that barely loses any speed, so in most cases I would have to recommend Diamond\_Square. However this algorithm is easy to understand and implement. In all a good starting point, but it can largely be replaced by Diamond\_Square.

Click here to see it in action: http://gimli.morningside.edu/~tra001/Algorithms/MidpointDisplacement/midpointdisp.cgi