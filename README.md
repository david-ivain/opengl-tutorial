# OPonGL

Just a simple pong recreation using [OpenGL](https://www.opengl.org/) 3.3 core, [glfw](https://github.com/glfw/glfw) and [glm](https://github.com/g-truc/glm) in c++.

![Screenshot](readme_files/v1_white_rectangles_on_black_bg.png)

## How to use

Here are some simple steps to build and launch the project.

#### Prerequisites:
- git
- cmake (^3.25)
- OpenGL 3.3 compatible graphics

### Mac and Linux\*

#### Prerequisites:
- clang, g++ should work too
- XCode Command Line Tools on Mac

Open the terminal, cd to the project's root and type in the following lines.
```shell
./configure.sh
./build.sh
./run.sh
```

## Features:
- [x] Display the bats and the ball
- [ ] Display arena (background or something)
- [ ] Eye candy graphics (something other than 3 white rectangles, like an actual ball)
- [x] Control 1 bat
- [ ] Multiplayer
- [x] Simple computer behaviour (follow the ball)
- [ ] Complex computer behaviour (bounce prediction)
- [ ] Computer vs computer
- [x] Bat-Wall collision
- [x] Ball-Wall collision
- [x] Ball-Bat collision
- [x] Point
- [x] Score
- [ ] Win condition
- [x] Serve
- [ ] Main menu
- [ ] In-game settings (arena size, bat/ball velocity/size, difficulty, v-sync, etc.)

\* Not tested on Linux and Windows
