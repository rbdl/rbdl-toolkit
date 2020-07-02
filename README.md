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
* Qt5 version == 5.13.2
* Qt3D and assimp for model loading

## Building On Ubuntu

**Warning** The most stable experience is currently provided with QT version 5.13.2 other version might work
but are not guarranted. This is due to the Qt3D Api no being stable yet.

Since our research group is mostly using Ubuntu here is a quick one line
install command for all dependencies except RBDL.

`sudo apt install qt5* qt3d* libeigen3-dev libassimp-dev`

Ubuntu installs qt5.9 per default and rbdl-toolkit might work with the exception
of the rendering plugin.  In order use the rendering plugin you will have to
install qt your self. To do this [download](https://download.qt.io/archive/qt/5.13/5.13.2/) the open source version of qt from
the website and follow the installation instructions. Make sure to install
at version *5.13.2* other versions are not guaranteed to work, since Qt3D is 
at the moment subject to many changes!

Afterwards build *rbdl-toolkit* with cmake and adding the CUSTOM_QT_PATH variable set to 
your [QT_INSTALL_DIR]/[QT_VERSION]/[COMPILER]/lib. 
For me that looked like this:

```code
cmake -DCUSTOM_QT_PATH=~/Qt5.13/5.13.2/gcc_64/lib ..
make
```

# Settings

The behavior of the toolkit can be adjusted by editing the settings of the
application. To easly change the Settings the Toolkit provides a nice settings editor which
can be accessed via the ui. The settings are stored on disc, on Linux systems it 
will be located at ```~/.config/ORB/rbdl-toolkit.conf```.

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
- Motion Marker Plugin
- Inverse Kinematics Plugin

**Info** To also build the optional plugins you will need to set the cmake option
`TOOLKIT_BUILD_OPTIONAL_PLUGINS` to `ON`. This might mean that you will require
additional dependencies like `libffmpeg`.

# Wiki

If you intend to add new functionality to the toolkit take a look at our [wiki](https://github.com/ORB-HD/rbdl-toolkit/wiki).
It will cover the basics of creating a new plugin and other information on how to use qt3d.

# Extras

In the extra folders are some python scripts that might be useful when
converting model meshes from meshup to rbdl-toolkit.

# Issues

If you find bugs or have other questions please file a GitHub issue on this repository.

