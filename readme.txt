minimal opengl project

use SDL2 and glew

#sdl2 install command
sudo apt install libsdl2
sudo apt install libsdl2-dev

#glew install command
sudo apt-get install -y libglew-dev

#############################
##      compilation
#############################

mkdir build
cmake -GNinja ..
ninja
