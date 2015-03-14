# Introduction #
Simplex Noise was created by Ken Perlin, the same man who created Perlin\_Noise. Perlin\_Noise worked fantastically in 2 and 3 dimensions, but Perlin was interested in 4, 5, and even up to 15 dimensions. Because Perlin\_Noise is based on a grid, each value must be linearly interpolated (LERP'd) 2<sup>N</sup> - 1 times. In 2D this is just 3 LERPs, but in 3D this is 7, in 4D it is 15 times, and in 15D it is 32767 LERPs for each point. If Fractional\_Brownian\_Motion is used, that's 32767 LERPs for each point, for each octave. Perlin\_Noise is simply unusable above a certain point, and Perlin wanted to create a function that would scale nicely into higher dimensions. Simplex Noise was his answer. While Perlin\_Noise is roughly O(N `*` 2<sup>N</sup>), Simplex Noise is roughly O(N<sup>2</sup>). That's polynomial instead of exponential. This makes it only slightly faster in lower dimensions but much faster in higher dimensions.

# Contents #



# Algorithm #
Simplex Noise is structured similarly to Perlin\_Noise. Both use the same permutation table and gradient table. A value for a given point is found by taking the gradients from the surrounding grid-points and computing the contribution from each. Simplex noise differs in the layout of the grid and how you compute the contributions.

Perlin\_Noise has gradients arranged on a square grid. In 3D it is a cube grid, and so on. However, this creates more corners than necessary. In N-dimensions, a shape with the smallest amount of corners possible (N+1) is called a **simplex**. In 2D, equilateral triangles can be tiled to completely cover a surface. By using less corners, you compute fewer contributions and theoretically do fewer interpolations. As it turns out, with simplex shapes you don't have to do any LERPs at all. Instead what is called a "radial attenuation function" is used to determine the contribution from each corner, then all of the contributions are added together, with no linear-interpolation necessary. In higher dimensions this translates to incredible savings.

The steps are (in theory) pretty simple:
  1. find the surrounding corners
  1. get their respective gradients
  1. get the contribution from each corner with a radial attenuation function and dot product
  1. add up the contributions

Steps 2 and 4 are as simple as they sound, but steps 1 and 3 are quite painful.

**Step 1**: In Perlin\_Noise finding the surrounding four corners is easy. Given a point, the four corners are defined by (int)x,(int)y,(int)x+1,(int)y+1. However in 2D simplex noise we move from squares to a grid of equilateral triangles, like this:

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/simplexgrid.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/simplexgrid.png)

The math here is not nearly as simple, and while I understand the math and how to extrapolate it, I don't know if I'll ever fully understand _why_ it works. The idea is to skew the grid so that we have a semblance of a square grid. By stretching along the diagonal we can get a grid that is axis-aligned and looks like this:

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/simplexgridskewed.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/simplexgridskewed.png)

The first corner (lower-left) and third corner (top-right) are quite easy to get from here. From here the point can either be in the upper triangle or lower triangle. As it turns out, if the fractional part of X is greater than the fractional part of Y, the point is in the lower triangle. If fracY is greater than fracX, the point is in the upper triangle.

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/xandy.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/xandy.png)

It is important to note that numbering of the grid-points is arbitrary as long as it is consistent. At first I was hung up on trying to get an integer from the grid-point in simplex-space (referred to as (x,y) space), when getting numbers from skewed-space (referred to as (i,j) space) is just fine. These integers are only used to assign a gradient to that particular grid-point, so as long as each point always returns the same gradient it is fine. However you must be in normal (x,y) space to get the distances from the three corners.

To skew space, there are two values used in the source code: general\_skew, and general\_unskew. These are multiplied times values to skew or unskew them. I don't know how it works, but here are the general formulas:

```
n = dimensions;
general_skew = (sqrt(n+1) -1)/n;
general_unskew = (n+1-sqrt(n+1))/(n*(n+1));

//in two dimensions:
general_skew = (sqrt(3)-1)/2;
general_unskew = (3 - sqrt(3))/6;

//in three dimensions:
general_skew = 1/3;
general_unskew = 1/6;
```

As you can see in 3 dimensions the formulas condense quite nicely, but most dimensions (like 2 and 4) the numbers get a bit hairy.

**Step 2**: At this point we know the coordinates of the surrounding corners, so we just need to assign a gradient to them. We do this using the same permutation-table method from Perlin\_Noise:

```
//the letter 'b' means bottom corner, 'm' means middle, and 't' means top or upper corner
gradb = permutations[(cornerbx + permutations[cornerby & 255]) & 255] & 7;
gradm = permutations[(cornermx + permutations[cornermy & 255]) & 255] & 7;
gradt = permutations[(cornertx + permutations[cornerty & 255]) & 255] & 7;
```

Just use the X and Y values of the corners in skewed space to get random numbers. Later these numbers are used as indices in the gradient-table. It is important to take wrapping into consideration. The gradient table only has 8 values, and the permutations table only has 256. If this isn't considered you will either get "array index out of bounds" errors or the program will return a null value that will lead to many headaches.

**Step 3**: Here we compute the contribution from each corner. This part is quite complex and again I don't fully understand it. For each corner, `radius` = 0.5 - `x_distance`<sup>2</sup> - `y_distance`<sup>2</sup>. For higher dimensions it scales just the same, so `radius` = 0.5 - `x_distance`<sup>2</sup> - `y_distance`<sup>2</sup> - `z_distance`<sup>2</sup> - `w_distance`<sup>2</sup> ... etc. If `radius` is less than 0, set it to 0. This is done because each corner should only affect points within a certain radius; if a point is outside that radius it should not get any contribution from that corner. If it is greater than 0, the noise contribution is found with `noise` = `radius`<sup>4</sup> `* dotproduct(gradient, x_distance, y_distance);` where `gradient` is the gradient chosen in step 2 for this particular corner.

**Step 4**: Finally, to get the final value just add the noise contributions together. In 2D add up the contributions from the three surrounding corners. No interpolation or fade functions necessary, just straight summation. This is where the boost in speed comes from in higher dimensions.

Do that for each pixel and you get this:

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhite.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhite.png)

For comparison, here is Perlin\_Noise:

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhiteperlin.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhiteperlin.png)

The quality of Simplex noise isn't a huge improvement over Perlin\_Noise (I couldn't tell you the difference), but there is a definite improvement in speed. In 2D, Simplex Noise is only ~10% faster than Perlin\_Noise. Both produce 500x500 images with 16 octaves in less than a second, so the difference there isn't noticeable. But in higher dimensions Simplex Noise screams ahead of Perlin\_Noise. So I've heard, anyway; I have yet to try any of these in 3D. I will get there soon enough.

Finally, at 16 octaves of Fractional\_Brownian\_Motion and some proper coloring, here is the finished product:

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/finalproduct.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/finalproduct.png)


# Comparison #

## Pros ##
  * All the advantages of Perlin Noise
    * Very good detail, and the amount of detail can be controlled.
    * No visible artifacts.
    * Nearly nothing _has_ to be stored in memory, although it can be.
  * Very fast: 500x500 image at 16 octaves in less than a second.
  * Faster than Perlin\_Noise, especially in higher dimensions.
  * Some say the quality is better.

## Cons ##
  * Every grid-point is at 0. This is barely noticeable (especially since the grid is made of triangles) and can be fixed if using multiple octaves.
  * Very, very difficult to understand.

# Ideas #
I would like to test both Simplex noise and Perlin\_Noise in 3D (and possibly higher) to really gauge the efficiency difference.

# Conclusions #
This algorithm was a challenge. With every other algorithm I understood the math before I was finished. While I mostly understand how the math in this algorithm works, I still don't know much about the methods used. The problem with not understanding the math is that it takes a long time to find the bugs. Regardless, this is a very fast, great quality algorithm. I will be keeping this one handy.

However, it is very important to note that my sample code is not optimized, and actually runs slower than Perlin\_Noise. The main culprit is the pow() function, which can be replaced with multiplications to go much faster. The code is not written for speed, it is written for readability. If you are into optimization, I suggest http://www.csee.umbc.edu/~olano/s2002c36/ch02.pdf (Appendix B), where Ken Perlin wrote the algorithm in Java to be put directly on video cards. It is nearly impenetrable. I did optimize my source code slightly for the page below.

Click here to see it in action (with pow()s replaced):
http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/simplexnoiseop.cgi