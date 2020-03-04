RBDL - Toolkit
==============

At this stage I have re implemented most of the functionality that meshup
provides. You can now use it to visualize your results. 

# Build

Use cmake to build and install, make sure to recursively clone the repository.
```
git clone --recursive https://github.com/ORB-HD/rbdl-toolkit
```

## Dependencies

* RBDL with minimum version 3.0.0
* Eigen3
* Qt5 version >= 5.13
* Qt3D and assimp for model loading

## Building On Ubuntu

**Warning** *rbdl-toolkit* builds with qt5.9 but the rendering plugin will
not work due to bugs in qt.

Since our research group is mostly using Ubuntu here is a quick one line
install command for all dependencies except RBDL.

`sudo apt install qt5* qt3d* libeigen3-dev libassimp-dev`

Ubuntu installs qt5.9 per default and rbdl-toolkit will work with the exception
of the rendering plugin.  In order use the rendering plugin you will have to
install qt your self. To do this download the open source version of qt from
the website and follow the installation instructions. Make sure to install
at version *5.13.2* other versions are not guaranteed to work, since Qt3D is 
at the moment subject to many changes!

Afterwards build *rbdl-toolkit* with cmake and adding the CMAKE_PREFIX_PATH variable set to 
your [QT_INSTALL_DIR]/[QT_VERSION]/[COMPILER]/lib/cmake. 
For me that looked like this:

```code
cmake -DCUSTOM_QT_PATH=~/Qt5.13/5.13.2/gcc_64/lib ..
make
```

# Settings

The behavior of the toolkit can be adjusted by editing the settings of the
application. This can be done by editing the settings file. On Linux systems
it will be located at ```~/.config/ORB/rbdl-toolkit.conf```.

Plugins should document their settings in their README files. 

# Command Line Options

*rbdl-toolkit* supports command line options. To see all available options
execute ``rbdl-toolkit --help``, plugins may add options so make sure all
plugins you want to load are enabled in the settings!

Options may be specified multiple times in order to load multiple models
for example.

# Plugins

This Application is extendable with qt plugins. There are two types of Plugins, core plugins and
optional plugins. Their interfaces are the same, but a core plugin can not be disabled while the
application is running. While the optional plugins can be enabled and disabled. 

Core Plugins:
- Animation
- Camera Operator
- Forces and Torques

Optional Plugins:
- Rendering (Picture and Video rendering)

**Info** To also build the optional plugins you will need to set the cmake option
`TOOLKIT_BUILD_OPTIONAL_PLUGINS` to `ON`. This might mean that you will require
additional dependencies like `libffmpeg`.

# Extras

In the extra folders are some python scripts that might be useful when
converting model meshes from meshup to rbdl-toolkit.

# Issues

If you find bugs or have other questions please file a GitHub issue on this repository.

