# Asteroids
A recreation of the arcade classic [Asteroids](https://en.wikipedia.org/wiki/Asteroids_(video_game))!
This game is programmed in C++ with the SDL2 library for graphics. A Windows binary is available in the releases.

## Installation
To install the game, simply download the binary provided in the latest release. Then, open the folder and run the `asteroids.exe` file.

## How to play
Asteroids is an endless game, meaning that the goal is to stay in the game as long as possible before losing all your lives.
Points are earned for destroying asteroids and flying saucers.

### Movement
The player's movement is controlled only by a thruster, which is mounted at the back of the player's ship and can only provide forward thrust.
To apply thrust, use the up arrow key. 
The player's ship can also be rotated counterclockwise and clockwise with the left and right arrow keys, respectively.
When not applying thrust, the player's velocity will slowly decrease until it reaches 0.

### Shooting
To shoot, use the space bar. This will shoot a bullet in the direction the player is facing. The bullet will travel until it hits an asteroid or saucer, or until it's maximum travel distance is reached.

### Point values
An asteroid can be of three different sizes. Every asteroids starts out large, and will break into two smaller ones when broke until the smallest size is reached.
The smaller the asteroid, the more points will be awarded when it is destroyed.

Additionally, a flying saucer will fly in every once in a while. The saucer will shoot at the player with increasing accuracy as the game goes on.
The saucer is a dangerous enemy, and therefore grants a large reward!

```
Large asteroid: 20 points
Medium asteroid: 50 points
Small asteroid: 100 points
Flying saucer: 1,000 points
```

### Lives
The player starts out with 5 lives. These lives can be lost by crashing into an asteroid, crashing into a flying saucer, or being hit by a flying saucer's bullet.
Additionally, a new life will be granted every 10,000 points.

## Other features
### Highscores
This game tracks the top 10 highscores on your computer. If you score a highscore after a game, you will be prompted to enter your name and the highscore will be recorded.
These highscores can be viewed in the game as well.

### Collision detection
Although it may seem like collisions are being detected using squares drawn around each shape, the collisions between the player and all asteroids are detected with a line segment intersection algorithm
Each asteroid has a set of predefined vertices that are rotated with the asteroid, which the player has as well.
These vertices and line segments can be viewed in the debug mode.

### Debug mode
Pressing the 'z' key during a game will enable debug mode, which will display all vertices, line segments, and sprite bounds.
Although this mode worsens performance, it is very interesting to look at. Below is an example.


<img src="https://user-images.githubusercontent.com/64991518/229324659-daec08f5-0511-43f8-a782-02baf67f6056.png" width="60%">

## Gameplay demo
https://user-images.githubusercontent.com/64991518/229324356-b7d8d368-de07-4ddd-b824-2cbd9d8a14d1.mp4

