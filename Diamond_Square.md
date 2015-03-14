# Introduction #
Diamond-Square is an improvement of Midpoint\_Displacement that attempts to combine the speed of Midpoint\_Displacement with the quality of Value\_Noise. It ends up being very slightly slower than Midpoint\_Displacement while getting close to the quality of Value\_Noise.

# Contents #



# Algorithm #
Diamond-Square is done by implementing Midpoint\_Displacement with an improvement to eliminate the square-shaped artifacts.

Midpoint\_Displacement is relatively simple. Start with the four corners of a square. Create the midpoint of the square by averaging the four corners around it and adding a random value. Then get the 4 side midpoints by averaging the 2 closest corners. This creates 4 sub-squares. For each of these squares, repeat the 2 steps. Do this iteratively or recursively until every pixel is filled.

Diamond-Square differs in the side-midpoint step. Instead of just using the two closest corners to find the midpoint, Diamond-Square uses the four closest corners of the surrounding diamond. A random value is also added to this average.

Here is plain Midpoint\_Displacement. The side-midpoint (circled) is computed by averaging the two closest corners (in the dark-red rectangles):

![http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/dss2.png](http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/dss2.png)

Here is Diamond-Square. The side-midpoint (circled) is computed by averaging the four corners of the surrounding diamond (in the dark-red rectangles), then adding a small random value:

![http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/dss1.png](http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/dss1.png)

This eliminates the square-shaped artifacts of Midpoint\_Displacement. However, there is an issue: what about on the edges? A point at the very edge of the map won't have 4 diamond corners, it will only have three. There are a few different approaches to fixing this:

  * Just average the three values, ignoring the fourth.
  * Use a random value that is pretty close to the other three.
  * Use a default value, such as what you initialized the original four corners to.
  * Wrap around; grab the value from the other side of the map.

Each has pros and cons. The first three options tend to leave 'creases' on the edges, but are very easy to implement. The fouth option, wrapping around, can be difficult to visualize and implement, but gives interesting results.

Here is an example of wrapping. The circled point is on the edge, and so only has 3 corners for the diamond (in dark-red rectangles). It pretends it is on the other side (pointed to by an arrow) and borrows the value from that point's corresponding corner.

http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/figure5.PNG

This eliminates the creasing, but has a possibly undesirable side-effect. The image itself now wraps, meaning if you put two of the same map side by side, it would be seamless:

http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/figure10.PNG

It looks repetitive, but can be necessary in some situations. For the old-school side-scrolling air-plane games, for instance, one could generate a map on the fly, then let it continue looping by. The player probably won't notice, but will get a new experience on each level with no effort from any artists. On the other hand, most people won't notice that it can wrap if it isn't repeated.

# Comparison #

## Pros ##
  * Blazingly fast. Less than 1 second for 1025 x 1025 images (although it is fractionally slower than Midpoint\_Displacement).
  * None of the square artifacts from vanilla Midpoint\_Displacement
  * Great quality, on par with Value\_Noise in most situations.
  * Can produce repeatable (wrapping) images if desired.

## Cons ##
  * The map **must** be stored in memory, because pixels reference other pixels.
  * Because it must be stored in memory, memory becomes a constraint. Most OS's won't allow an image beyond 1025 x 1025 without some fancy footwork (Windows won't allow one beyond 513 x 513).
  * The map must have a width and height of 2<sup>x</sup> + 1 pixels.
  * Has 'creasing' artifacts if wrapping isn't used.
    * Wrapping isn't always wanted.

# Ideas #
I would like to find a way to eliminate the creasing artifacts without wrapping. Wrapping can be useful, but it becomes a constraint if it's required. I would also like to create a 'zooming' program for this algorithm as well, although it will be much more complicated.

# Conclusions #
This algorithm is nearly as fast as Midpoint\_Displacement (still less than 1 second for a 1025x1025 image), yet produces much better quality. Yet it still has the same constraints as Midpoint\_Displacement: memory and size (2<sup>x</sup> + 1 pixels). However, maps can be created on the fly with Midpoint\_Displacement, unlike Value\_Noise that takes 28 seconds for a 512x512 image. Both have their advantages and disadvantages, and both have their place. If time isn't a constraint or memory is, I prefer Value\_Noise because of the lack of any visual artifacts. If time is a constraint and memory isn't, Diamond\_Square is lightning-fast while still providing good quality.

Click here to see it in action: http://gimli.morningside.edu/~tra001/Algorithms/DiamondSquare/DiamondSquare.cgi