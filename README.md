# Geometry Wars

## Usage
You can move around using the "W A S D" keys. You shoot with "Left Mouse Click" and you can use your special weapon with "Right Mouse Click"

There is also an ImGui menu that you can use to change settings. And see the alive entities, you can also delete entities in this menu.

```
git clone https://github.com/githubskribble24/geometrywars.git

1. Run the .SLN file and build the program

If you are planning on running the game with the .exe file make sure you have the "config.txt" file in the same folder.
```

### Changing config
The config file looks like this
```
Window 1920 720 144 1
Font fonts/tech.ttf 24 255 255 255
Player 32 32 5 5 5 5 255 0 0 4 8
Enemy 32 32	3 3 255 255 255 2 3 8 90 120
Bullet 10 10 20 255 255 255 255 255 255 2 20 90
```

It is using the following format
```
Window WIDTH HEIGHT FPS FULLSCREEN=1|NOT_FULLSCREEN=0
Font FONT_PATH FONT_SIZE FONT_COLOR_R FONT_COLOR_G FONT_COLOR_B
Player SHAPE_RADIUS COLLISION_RADIUS SPEED FILL_COLOR_R FILL_COLOR_G FILL_COLOR_B OUTLINE_COLOR_R OUTLINE_COLOR_G OUTLINE_COLOR_B OUTLINE_THICKNESS SHAPE_VERTICES
Enemy SHAPE_RADIUS COLLISION_RADIUS SPEED_MIN SPEED_MAX OUTLINE_COLOR_R OUTLINE_COLOR_G OUTLINE_COLOR_B OUTLINE_THICKNESS MIN_VERTICES MAX_VERTICES SMALL_LIFESPAN SPAWN_INTERVAL
Bullet SHAPE_RADIUS COLLISION_RADIUS SPEED FILL_COLOR_R FILL_COLOR_G FILL_COLOR_B  OUTLINE_COLOR_R OUTLINE_COLOR_G OUTLINE_COLOR_B OUTLINE_THICKNESS SHAPE_VERTICES SMALL_LIFESPAN
```

