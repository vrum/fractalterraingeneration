# What is Fractional Brownian Motion? #

Fractional Brownian Motion is the summation of successive octaves of noise, each with higher frequency and lower amplitude. It doesn't especially matter what kind of noise, most will do. Fractional Brownian Motion (often abbreviated as fBm) is often confused with noise algorithms like Value\_Noise and Perlin\_Noise, when in fact it just takes a type of noise and makes a more interesting picture.

# How to make Fractional Brownian Motion #

To use fBm, the following is typical:

```
//for each pixel, get the value
total = 0.0f;
frequency = 1.0f/(float)hgrid;
amplitude = gain;
		
for (i = 0; i < octaves; ++i)
{
	total += noise((float)x * frequency, (float)y * frequency) * amplitude;		
	frequency *= lacunarity;
	amplitude *= gain;
}
			
//now that we have the value, put it in
map[x][y]=total;
```

There are five terms here: octaves, frequency, amplitude, lacunarity, and gain.

**Octaves** are how many layers you are putting together. If you start with big features, the number of octaves determines how detailed the map will look.

The **frequency** of a layer is how many points fit into the space you've created. So for the mountain scale, you only need a few points, but at the rock scale you may need hundreds of points. In the code above, I start with a small frequency (which equates to large features) and move to higher frequencies which produce smaller details.

The **amplitude** is how tall the features should be. Frequency determines the width of features, amplitude determines the height. Each octave the amplitude shrinks, meaning small features are also short. This doesn't have to be the case, but for this case it makes pleasing maps.

**Lacunarity** is what makes the frequency grow. Each octave the frequency is multiplied by the lacunarity. I use a lacunarity of 2.0, however values of 1.8715 or 2.1042 can help to reduce artifacts in some algorithms. A lacunarity of 2.0 means that the frequency doubles each octave, so if the first octave had 3 points the second would have 6, then 12, then 24, etc. This is used almost exclusively, partly because octaves in music double in frequency. Other values are perfectly acceptable, but the results will vary.

**Gain**, also called **persistence**, is what makes the amplitude shrink (or not shrink). Each octave the amplitude is multiplied by the gain. I use a gain of 0.65. If it is higher then the amplitude will barely shrink, and maps get crazy. Too low and the details become miniscule, and the map looks washed out. However, most use 1/lacunarity. Since the standard for lacunarity is 2.0, the standard for the gain is 0.5. Noise that has a gain of 0.5 and a lacunarity of 2.0 is referred to as 1/f noise, and is the industry standard.

# Why use Fractional Brownian Motion? #
Noise alone is a little disappointing. Below are images of noise. Note the lack of details. Value\_Noise, Perlin\_Noise, and Simplex\_Noise, respectively.

![http://gimli.morningside.edu/~tra001/Algorithms/ValueNoise/blacknwhitevalue.png](http://gimli.morningside.edu/~tra001/Algorithms/ValueNoise/blacknwhitevalue.png)

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhiteperlin.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhiteperlin.png)

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhite.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhite.png)

If fBm is used, the images gain more detail with each octave. Instead of looking blurry and boring they become crisp and interesting. Below are the same noise algorithms utilizing fBm:

![http://gimli.morningside.edu/~tra001/Algorithms/ValueNoise/blacknwhitevaluefbm.png](http://gimli.morningside.edu/~tra001/Algorithms/ValueNoise/blacknwhitevaluefbm.png)

![http://gimli.morningside.edu/~tra001/Algorithms/PerlinNoise/blacknwhiteperlinfbm.png](http://gimli.morningside.edu/~tra001/Algorithms/PerlinNoise/blacknwhiteperlinfbm.png)

![http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhitefbm.png](http://gimli.morningside.edu/~tra001/Algorithms/SimplexNoise/blacknwhitefbm.png)

Fractional Brownian Motion isn't always necessary and considerably slows down a program. However, most noise is not fractal by nature. Fractional Brownian Motion makes it fractal.