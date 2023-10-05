# How to build #

To build this project you will need [vcpkg](https://vcpkg.io/) and [cmake](https://cmake.org/) (minimum version required is 3.27.0)

```shell

git clone https://github.com/AutoPilot2004/Dark.git
cd Dark
mkdir build
cd build

```

## Windows and Linux ##

For multi-config generators:

```shell

cmake .. -DCMAKE_TOOLCHAIN_FILE="path/to/vcpkg.cmake" [-G <"Visual Studio 17 2022", ...>]
cmake --build . [--config <Debug/Release/RelWithDebInfo>]

```

For single-config generators:

```shell

cmake .. -DCMAKE_TOOLCHAIN_FILE="path/to/vcpkg.cmake" [-DCMAKE_BUILD_TYPE=<Debug/Release/RelWithDebInfo>] [-G <"Ninja", ...>]
cmake --build .

```