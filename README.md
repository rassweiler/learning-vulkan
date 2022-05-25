# Vulkan Linux Learning

## Setup

Install dependencies for vulkan development from this [Vulkan Tutorial](https://vulkan-tutorial.com/Development_environment#page_Linux)

Skip the makefile section as this project uses cmake.

~~The build script can be modified for a different IDE (Uses CodeLite as is).~~

I'm currently using Code - OSS with the c++ and cmake extensions from this [Tutorial](https://code.visualstudio.com/docs/cpp/cmake-linux). If you are also running Arch you will need to manually install the C++ extension from Microsoft (They use a different store from what I gather).

### Arch + Code - OSS

```zsh
sudo pacman -S vulkan-devel

sudo pacman -S glfw-x11

sudo pacman -S glm glew

sudo pacman -S shaderc

sudo pacman -S libxi libxxf86vm

git clone https://github.com/rassweiler/learning-vulkan.git
```

- Download [c++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) for Code - OSS and install it manually (Install from VSIX menu option).
- Select a compiler kit: Open command menu (ctrl-shift-p) and run `Cmake: Select A Kit`.
- Select a build variant: Open command menu (ctrl-shift-p) and run `Cmake: Select Variant`.
- Configure Cmake: Open command menu (ctrl-shift-p) and run `Cmake: Configure`.
- Build Project: Open command menu (ctrl-shift-p) and run `Cmake: Build` (Or press build button at bottom of window).

### Other IDE

If not using Code - OSS then run the cmake command for your generator of choice


## References
- [Vulkan Tutorial](https://vulkan-tutorial.com/Development_environment#page_Linux)
- [Brendan Galea](https://www.youtube.com/c/BrendanGalea/videos)