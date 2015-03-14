# Introduction #
Value noise is the first algorithm I attempt for this project. It promises great quality with high speed. As it turns out only the former is true, but the algorithm stacks up well against my personal favorite, the Diamond-Square algorithm.



# Contents #



# Algorithm #
Value noise is created by summing together interpolated values of noise at various amplitudes and frequencies. That may sound complicated, but it's really quite simple. If you take a few points of noise (random values) and draw smooth lines between you can get some random looking hills. No details, but big picture. Suppose you did this a few times with more points each time and smaller values. You'd get tamer and tamer lines with more detail each time. Add all these lines together (the hills and the rocks) and you get the big picture plus the little details plus everything in-between.

To do this, I run this code for each pixel:

```
//for each pixel, get the value
total = 0.0f;
frequency = 1.0f/(float)hgrid;
amplitude = persistence;
		
for (i = 0; i < octaves; ++i)
{
	total += smoothedNoise((float)x * frequency, (float)y * frequency) * amplitude;		
	frequency *= 2;
	amplitude *= persistence;
}
			
//now that we have the value, put it in
map[x][y]=total;
```

There are four terms here: octaves, frequency, amplitude, and persistence.

**Octaves** are how many layers you are putting together. If you start with big features, the number of octaves determines how detailed the map will look.

The **frequency** of a layer is how many points fit into the space you've created. So for the mountain scale, you only need a few points, but at the rock scale you may need hundreds of points. In the code above, I start with a small frequency (which equates to large features) and move to higher frequencies which produce smaller details.

The **amplitude** is how tall the features should be. Frequency determines the width of features, amplitude determines the height. Each octave the amplitude shrinks, meaning small features are also short. This doesn't have to be the case, but for this case it makes pleasing maps.

**Persistence** is what makes the amplitude shrink (or not shrink). I use a persistence of 0.65. If it is higher then the amplitude will barely shrink, and maps get crazy. Too low and the details become miniscule, and the map looks washed out.

The algorithm itself has two parts that aren't very intuitive: the smoothing (or interpolation) function and the random number generator (RNG).

The interpolation function is especially important at lower frequencies (big features) because we only get a few points to work with. The interpolation function draws the line between the points. To do this you can use any interpolation function (there are many), however I included three in the code: linear, cosine, and cubic. Linear just draws straight lines between each point, which looks quite jagged. Good for mountains, not very good for hills or ocean floors. Cosine gives smoother results, and is only a hair slower than linear. Cubic gives perfectly smooth lines, but is very very slow. If the program has time, I prefer to use cubic; if it doesn't then I prefer to use cosine.

The random number generator is the tricky part. In the above code each pixel is generated independently, no other pixels are accessed. This can be very helpful, but can also be a head-ache. The RNG will be called many times for each point in a layer, but always has to come up with the same answer. Yet each map must be unique, so how can we accomplish this? I used the following code:

```
float random(int x, int y){
	int r;
    float s;

	srand(y*hgrid+x+seed);

	r=rand();

    s = (float)(r & 0x7fff)/(float)0x7fff;

    return (s);
}
```

Essentially, I seed the RNG built into C++ each time the function is called. At the beginning of the program I create a global seed from a time function, so each map will be unique. Yet I also seed it with the x and y values (I use y times the width so 2,3 is not the same as 3,2). rand() then creates a unique value. After that I just shrink it to size. It is important to note that some compilers have their own RNG, so this may not work for everyone. Visual C++ 2008 was especially troublesome, however GCC on Linux works perfectly.

# Comparison #

## Pros ##
  * Very good detail, and the amount of detail can be controlled.
  * No visible artifacts.
  * Nearly nothing _has_ to be stored in memory, although it can be.
  * If each pixel is rendered individually, the program is not limited by the amount of RAM it is allocated (4000 x 4000 floats won't work...)

## Cons ##
  * Much, much slower than Midpoint\_Displacement.
  * More complex to implement than Midpoint\_Displacement.
  * You can't 'paint' values in like Midpoint\_Displacement (I want a mountain right... here.)
  * Without having the entire map in memory before printing, it's impossible to determine the absolute maximum and minimum values. That means there is a chance no white mountains will appear, and even a small chance there will be absolutely no oceans. If stored in memory first, the only advantage it has over Midpoint\_Displacement is the lack of visual artifacts.

# Ideas #
Because each pixel can be rendered to theoretically infinite detail (just keep increasing the octaves), and any point can be calculated independently of other points, one could make an interactive program that lets you 'zoom and enhance' into finer and finer detail. One could also make several passes at an image, rendering a few octaves at a time, creating a more and more detailed image while improving speed if done in real-time.

# Conclusions #
While not as complex as some other algorithms, this algorithm has decent speed with great quality. The quality is highly controllable, so it can be tailored to any need. It has distinct advantages over my favorite, Diamond-Square, yet falls short in terms of speed. In all, a very good and interesting program I will be sure to keep in my tool-belt.