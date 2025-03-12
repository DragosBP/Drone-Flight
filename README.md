# Drone Flight Minigame

> Implemented using **OpenGL Framewrk**

3D game of controlling a small drone and flying through different goals all while avoiding obstacles

## Features

### Terrain

Terrain is generated using a noise function and altering its height in a **Shader** so it has a smoother look. The color of it is based on the heinght of it. Additionally, it has colission so that the drone can't fly under it.

### Obstacles

There are 2 types of obstacles:
- **rocks**: short, long obstacles;
- **trees**: tall, thinner obstacles;
- **goals**: drone needs to fly through them.

They are placed at the beggining of the game, randomly, making sure that there are no obstacles clipping throogh one another. Except goals, they can have random sizes.

Each obstacle has its own colission box that is suited for the type of obstacle (Ex. goals have the middle empty, only the margin have collisions) and their sizes.

### Drone

A drone that can be flown around map and can:
- ascend and descend
- lateral movement
- rotate around its Y axis

The drone has its own colission box to ensure that it cannot clip through terrain or obstacles. Additionally, it has colission detections and, in case it crashes into an obstacle, it bounces off.

### Camera

The camera has 3 mode:
- **free view**: it can fly around the map, not carring about the drone.
- **third person**: it follows the drone from a 3rd person POV;
- **first person**: it follows the drone, from a 1st person POV.

### Goals

Each gole has 3 types:
- **uncompleted**: white color, it does nothing
- **next**: green color, the player gains a point if it flies through it
- **completed**: red color, the drone gain a point from it

In case the goal is green it gives the point when the dron has 1/3 of its size through it.

Additionally, there is a error that can be enabled that hover above the drone and point to the **next goal**.

### Minimap

In the bottom right corner there is a minimap that shows a top-down view of the world and follows the drone as it moves.


## Gameplay

The scope of the game is to fly the drone through the objectives as fast as possible.

## Additional setting

Some additional settings are:
- size of the map;
- number of trees/rocks/goals that are spawned;
- the sizes that the trees/rocks can be
