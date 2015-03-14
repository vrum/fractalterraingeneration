# Introduction #
Erosion Modeling is taking a heightmap and simulating erosion to alter it. Often this is used in video games to make the map more playable. Games generally require large relatively flat areas, but still want steep cliffs for aesthetic value.

I implemented three different techniques, as described in [this paper](http://oddlabs.com/download/terrain_generation.pdf). The three are **Thermal Erosion**, **Hydraulic Erosion**, and **Improved Erosion**. For reference, all the images below used this as the base image to which specific erosion algorithms were applied:

![http://gimli.morningside.edu/~tra001/Algorithms/Erosion/baseimage.png](http://gimli.morningside.edu/~tra001/Algorithms/Erosion/baseimage.png)

# Contents #


# Algorithms #

## Thermal Erosion ##
**Thermal Erosion** represents gravity collapsing cliffs. The idea is relatively simple: The slope of a point is the difference in height between that point and its tallest/shortest neighbor. For each point if the slope is greater than the angle _T_, called the talus angle, then gravity wins and soil is moved until the slope is less than _T_.

This is a relatively straight-forward algorithm, but there are a few different ways to handle some issues. For instance, when altering the terrain, changes can be immediately applied or a difference map can be used that applies all changes at once. In my implementation I immediately applied the changes.

The method for checking neighbors can be changed as well. In 2D a point has 8 neighbors, called the Moore Neighborhood. However, only checking four corners (called the Von Neumann Neighborhood) yields roughly the same results while greatly improving speed. The Von Neumann neighborhood uses the four neighbors that make a cross shape, however I used a rotated Von Neumann neighborhood like so:

![http://gimli.morningside.edu/~tra001/Algorithms/Erosion/erosion2.png](http://gimli.morningside.edu/~tra001/Algorithms/Erosion/erosion2.png)

Although the code can be easily changed (two operations) to use a Moore neighborhood instead.

The important variable here is the talus angle, _T_. I set mine to 4/`width_of_the_map`.

Thermal Erosion after 4 iterations:

![http://gimli.morningside.edu/~tra001/Algorithms/Erosion/thermal4.png](http://gimli.morningside.edu/~tra001/Algorithms/Erosion/thermal4.png)

### Comparison ###

#### Pros ####
  * Each iteration is very fast
  * Needs very few iterations
  * Easy to understand and implement
  * Creates flat areas well

#### Cons ####
  * Doesn't maintain cliffs at all
  * Flat areas are often exactly the _T_ angle
  * Not great quality

## Hydraulic Erosion ##
**Hydraulic Erosion** models rainwater picking up sediment from uphill and depositing it downhill. This ends up being considerably more complex than **Thermal Erosion**, but can be roughly broken into four steps:

  1. Rainfall (new water)
  1. Erosion (picking up sediment)
  1. Water Movement (downhill usually)
  1. Evaporation / Deposition (depositing the sediment)

A few coefficients must be defined:
  * `rain_amount` : how much rain is deposited on each cell each iteration
  * `solubility` : how much soil is eroded into sediment each iteration
  * `evaporation` : what percentage of water evaporates each iteration
  * `capacity` : how much sediment a given unit of water can hold

This is usually done over many iterations. To keep track of everything, a few secondary maps are used: a **water map**, a **sediment map**, and a **difference map**. The **difference map** is the same as in **Thermal Erosion**: changes for an iteration are recorded to it then applied all at once after each iteration. In my code I again did not use a **difference map** and instead apply changes immediately.

**1. Rainfall.** This is a very easy step. For each cell (pixel, point, etc.) add a small amount (`rain_amount`) to the **water map**. While it is perfectly possible to use noise to make nicely randomized rain, after a few iterations it makes no difference. We aren't actually simulating a rain storm, we're simulating years of continued erosion.

**2. Erosion.** This step is also pretty easy. For each cell subtract a small amount of soil (`solubility`) and put it in the **sediment map**. This step is nearly as easy as step 1.

**3. Movement.** This is the difficult step. Every cell now has some water on it (some may have more from previous iterations). Water uphill must move to the lowest neighbor downhill, and water must pool and level out when possible. So for any given cell there are a few options:

![http://gimli.morningside.edu/~tra001/Algorithms/Erosion/erosion1.png](http://gimli.morningside.edu/~tra001/Algorithms/Erosion/erosion1.png)

Case 1: The height of the water from the cell + the water and land from the lowest neighbor still does not reach the height of the land in the cell. In this case, all water is moved from the cell to the lowest neighbor.

Case 2: If all water were transfered the water level would be above the land level of the cell, so it can't all be moved. Instead the two water levels must be evened out, subtracting 1/2 the difference from the cell's water and adding 1/2 the difference to the neighbor's water.

Case 3: The cell has no neighbors lower than it, so no water needs to be moved at all.

Every cell will fall into one of these three categories. For maximum correctness water should be distributed to all lower neighbors, but using only the lowest neighbor greatly increases speed and barely affects quality.

**4. Evaporation.** This step is pretty easy. For every cell evaporate a given percentage (`evaporation`) of the water from the **water map**. Now that the water has gone down, the sediment in the water usually exceeds `capacity`. Sediment is subtracted from the **sediment map** and added to the heightmap until it no longer exceeds `capacity`. One interesting optimization is setting `solubility = capacity`. That means any given unit of water will contain the same amount of sediment, so the **sediment map** can be excluded. This cuts memory requirements by 1/3 and makes programming simpler.

These four steps are performed on every cell (pixel, point, etc.) every iteration. Hydraulic erosion usually takes 50-100 iterations to get the desired quality. This is hydraulic erosion after 500 iterations:

![http://gimli.morningside.edu/~tra001/Algorithms/Erosion/hydro500.png](http://gimli.morningside.edu/~tra001/Algorithms/Erosion/hydro500.png)

### Comparison ###
#### Pros ####
  * Very good quality
  * Approximates reality pretty well

#### Cons ####
  * Much slower than Thermal Erosion
  * Requires many more iterations than Thermal Erosion

### Ideas ###
Rain doesn't have to come down evenly, or be rain at all. If consistent noise is used then certain areas will get very little rain (and look more jagged) and some areas will get more rain (and look smoother). This wouldn't look very good on a map of a small area, but for planet-sized maps it would add some interesting variance (although ocean beds would have to be taken into account). It would also be possible to 'plant' a spring at the top of a mountain which would spew extra water each iteration, eventually cutting a river-bed into the landscape. Neither of these ideas would be very difficult to implement, so I will have to come back to them soon.

## Improved Erosion ##
**Improved Erosion** is a relatively minor adjustment to **Thermal Erosion** to create the desired outputs. **Thermal Erosion** flattened cliffs but did nothing to relatively flat areas. We want the opposite of this, to keep cliffs but continue flattening areas that were slightly jagged.

The adjustment is simple: in **Thermal Erosion** soil moves down if the slope is greater than the talus angle _T_. However, if this operation is flipped to only move soil if the slope is <= _T_, then cliffs are left alone and relatively flat areas continue to flatten. This creates plateaus, which are very useful for games.

One pitfall here is to continue using the same _T_ angle. Using 4/map`_`width seems to completely stop erosion. Values of 12/map`_`width or 16/map`_`width will create the desired erosion. It is also important to determine how many iterations are required. Too many iterations creates flat landscapes with thin squiggles running through it. The squiggles are plateaus eroded until they are only 1 or 2 pixels wide (and so become 100% cliffs) and become immune to the erosion.

Here is **Improved Erosion** after 50 iterations:

![http://gimli.morningside.edu/~tra001/Algorithms/Erosion/improved50.png](http://gimli.morningside.edu/~tra001/Algorithms/Erosion/improved50.png)

### Comparison ###

#### Pros ####
  * Much faster than **Hydraulic Erosion**
  * Good quality
  * Great for games
  * Easy to implement

#### Cons ####
  * Good for games, but not necessarily for realism.
    * Looks kind of like a watercolor painting.
  * Slightly slower than **Thermal Erosion** (takes more iterations).
  * Iterations must be determined carefully, or artifacts can arise.

# Overall Ideas #
I think the landscapes might look better with a very small amount of noise added back on after erosion is finished, just to give it a texture.

# Conclusions #
Thermal erosion looks pretty awful, so I don't think I'll ever be using it. Hydraulic Erosion is my favorite, but runs pretty slowly. Improved Erosion is good for games, but doesn't really look all that realistic. I will continue using and experimenting with Improved Erosion and especially Hydraulic Erosion.

I should note that I learned the other algorithms by mashing together information from many different sources. For Erosion Modeling I really only had one primary reference, http://oddlabs.com/download/terrain_generation.pdf which definitely deserves mention as it is very well done.

Click here to see it in action:
http://gimli.morningside.edu/~tra001/Algorithms/Erosion/ImprovedErosion.cgi