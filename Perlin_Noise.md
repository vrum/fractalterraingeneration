# Introduction #
Perlin Noise was created by Ken Perlin while working on the Disney movie _Tron_. Since then it has become the industry standard for noise generation, even getting hardware support from graphics-card companies. It is similar to Value\_Noise in many ways, yet differs enough to provide the same level of quality at much higher speeds.

# Contents #



# Algorithm #
I would highly recommend reading the wiki entry (or a better guide) on Value\_Noise before continuing. The two are more similar than different.

Perlin Noise begins by creating a grid of vectors or gradients. Each grid point has a gradient pointing away from it in a random direction. This is easier to visualize with arrows:

![http://gimli.morningside.edu/~tra001/download/perlin.arrows.png](http://gimli.morningside.edu/~tra001/download/perlin.arrows.png)

Now for any given point (or pixel) interpolate the value from the surrounding four gradients. This is similar in structure to Value\_Noise, but differs quite heavily. First, each grid-point has a value of 0, and every other point is found by interpolating between gradients instead of value-points. There are a few steps to this:

  1. Get the four closest gradient values.
  1. For each of the above, get the dot-product using the relative distance between it and the pixel.
  1. Use a fade function to skew the interpolation value.
  1. Linearly interpolate to find the value.

Step 1 is easy: if you know the width of the squares in the grid, the position for the pixel will be the index in the image (ie: pixel 23,68) divided by the width of the squares. So if the squares are 64 pixels wide, pixel (23,68) will be at position (0.359, 1.0625). From there, round these values up and down to find the surrounding four grid-points. In this case they would be: (0,1),(1,1),(0,2),(1,2).

Step 2 is easy to implement, but not easy to understand. The dot-product is a linear-algebra function that takes 2 vectors (a vector is a 1-dimensional chain of numbers, ie: (1,5,2,0.223)) and outputs a single number. In this case, the first vector is one of the four gradients from the above step. The second vector is made up of the relative distance from the point to that grid-point. This can also be easily found. First get just the fractional part of the position from step one: in our example (0.359, 1.0625) would create (0.369, 0.0625). Next adjust for the particular grid-point as such:

  * top-left: just x, y
  * top-right: x-1, y
  * bottom-left: x, y-1
  * bottom-right: x-1, y-1

This may seem counter-intuitive, but it is to adjust for the possibly negative values in the gradients. Gradients pointing left or down will have negative values, so subtracting 1 from the values compensates for this. I admit I'm still not 100% clear on this, but it is a necessary step.

Once you have your second vector, the dot product is easy to compute. Each vector should be the same length, so for each pair of values (such as index 1 in both, or index 2 in both) multiply the two together, then add all the products together. This may be easier to visualize in pseudo-code:

```
float dotProduct(vector a, vector b)
{
    float total = 0.0;

    for (i=0; i<a.length; i++)
    {
        total += a[i] * b[i];
    }

    return (total);
}

```

Step 3 is again simple to implement but difficult to visualize. Here we compute a fade value (in fact one for each dimension of the image). When doing linear interpolation, one generally uses the distance between the two options as the value. In this case, we use a fade function to skew that value, which gives smoother results. Ken Perlin originally used the Hermite function, but for math reasons beyond me (something about derivatives) finally landed on the following function: 6x<sup>5</sup> - 15x<sup>4</sup> + 10x<sup>3</sup> , where x is the fractional value from step 2. This must be done for each dimension, so if the image is 2-dimensional you must fade the x-fraction and the y-fraction.

Finally in step four we do simple linear interpolation (also called LERP). Linear interpolation takes three arguments: value A, value B, and amount T. If you put a sliding scale between A and B, with 0 being at A and 1 being at B, the function would return a value at T between A and B. In this case, A and B are dot products from step 2 and T is the fade value from Step 3.
It is important to note that linear interpolation only works in 1 dimension. To use it in higher dimension gets a bit trickier:

```
   interpolatedx1 = LERP(dot00, dot10, fadedX);
   interpolatedx2 = LERP(dot01, dot11, fadedX);

   finalValue = LERP(interpolatedx1, interpolatedx2, fadedY);
```

Essentially we LERP twice on the x-axis to get two values for LERPing on the y-axis:

In 3 dimensions the function must be called 7 times, in 4 dimensions it must be called 15 times. This is all for 1 pixel. In a 2-dimensional 500x500 image, it must be called 750,000 times. The function is not very expensive itself, but in higher dimensions Perlin Noise becomes very expensive.

Ken Perlin faced two challenges when creating this algorithm: how to create the random numbers, and how to create the random gradients. It is very important that every pixel can gets the same values for each grid-point, yet space was very limited at the time. He originally settled on a permutation table and a gradient table. He later used some bit-fiddling to eliminate the gradient table, but today many still use gradient tables as they are pretty small and easier to visualize.

The gradient table holds gradient vectors that are equally distributed in every direction. In 2D, this is gradients equally distributed around the unit-circle. In 3D it is gradients equally distributed around a sphere.

The permutation table is his answer to the issue of random numbers. First take an array of decent length, usually 256 values. Fill it sequentially with each number in that range: so index 1 gets 1, index 8 gets 8, index 251 gets 251, etc. Then randomly shuffle the values so you have a table of 256 random values, but only contains the values between 0 and 255.

To get a gradient for a given grid-point (in 2D), do the following: `grad = gradient[permutation[X + permutation[Y]]] ` If you later pass in the same values, you will get the same gradient. Some wrapping is necessary, as the gradient table is generally very small (8-16 values) and the permutation table is generally much larger.

That is all that is required to make a Perlin Noise function. However, that won't generally give you the image you want. Here is what it looks like in the above form:

![http://gimli.morningside.edu/~tra001/download/perlin.single.png](http://gimli.morningside.edu/~tra001/download/perlin.single.png)

That is noise, but there is no detail. To get the image we want, we must follow the same approach as Value\_Noise: use Fractional\_Brownian\_Motion. By combining the two, we get an image like this:

![http://gimli.morningside.edu/~tra001/download/perlin.multi.png](http://gimli.morningside.edu/~tra001/download/perlin.multi.png)

# Implementation Pitfalls #
This algorithm seemed to fight me more than others when implementing it. It takes only very small mistakes to create bizarre images. I hope that by showing my mistakes here you will be able to fix your program if you get similar images. Conversely, if you want images like this, you'll know how to do it.

At first I created a working algorithm that looked good at first glance, but upon closer inspection was very dull in the upper-left corner. In fact, it was very 'excited' in the bottom-right corner as well:

![http://gimli.morningside.edu/~tra001/download/perlin.mistake1.png](http://gimli.morningside.edu/~tra001/download/perlin.mistake1.png)

As it turns out I was passing in the integer portion of the position instead of the fractional part. That meant that in the upper-left corner (0,0) the dot product was creating very small values. At the opposite corner the grid-points are larger (5,5) and so the dot-product is much bigger. I fixed this by passing in the fractional part instead, which should have fixed the problem. As it turns out, I had another error which was giving me images like this:

![http://gimli.morningside.edu/~tra001/download/perlin.mistake2.png](http://gimli.morningside.edu/~tra001/download/perlin.mistake2.png)

It looks like a puzzle, and it was baffling. As it turns out in step 2 when you should subtract 1 I was adding 1. It creates an interesting effect, especially in higher octaves.

# Comparison #

## Pros ##
  * All the advantages of Value Noise
    * Very good detail, and the amount of detail can be controlled.
    * No visible artifacts.
    * Nearly nothing _has_ to be stored in memory, although it can be.
  * Very fast: 500x500 image at 16 octaves in less than a second.
  * Pretty easy to implement if you understand it.

## Cons ##
  * Every grid-point is at 0. This is barely noticeable and can be fixed if using multiple octaves.
  * Difficult to understand.

# Ideas #
The core of Perlin Noise seems more complex than Value\_Noise. The only difference in the framework is that Perlin Noise uses a permutation table to get random numbers, while my implementation of Value\_Noise seeds the RNG with the X and Y values. I would like to use a permutation table with Value\_Noise to see if that speeds it up. Excluding octaves, Value\_Noise is very simple, while Perlin\_Noise seems to do many more multiplications. I would like to test which method is faster, and attempt to integrate whichever is faster into the opposing algorithm.

# Conclusions #
This algorithm combines the speed of Diamond\_Square with the quality of Value\_Noise. It is a very powerful algorithm with good speed. However, Perlin himself wasn't satisfied with it and created Simplex\_Noise as a replacement. However, this algorithm is much easier to understand than Simplex\_Noise and is good enough in most cases. In all, a very powerful function that succeeds in combining the speed of Midpoint\_Displacement with the quality of Value\_Noise.

Click here to see it in action:
http://gimli.morningside.edu/~tra001/Algorithms/PerlinNoise/perlinNoise.cgi