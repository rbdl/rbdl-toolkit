RBDL - Toolkit
=============

# About

This Application is intended to be the replacement tool for meshup and puppeteer. It is currently in it's development phase! 

The basic functionality of displaying animations like meshup already works!

# Build

Use cmake to build and install

## Dependencies

* RBDL with minimum version 3.0.0
* Eigen3
* Qt5 version >= 5.9
* Qt3D and assimp for model loading

## Building On Ubuntu

Since our research group is mostly using Ubuntu here is a quick one line install command for all
dependencies except RBDL.

`sudo apt install qt5* qt3d* libeigen3-dev libassimp-dev`

# Command Line Options
*rbdl-toolkit* supports command line options. To see all availible options execute ``rbdl-toolkit --help``, plugins may add options
so make sure all plugins you want to load are enabled in the settigs!

Options may be specified multiple times in order to load multiple models for example.

# Plugins

This Application is extendable with qt plugins. The Interface is not yet finished but you can take a look at the toolkit_inteface.h and
the AnimationPlugin to get an idea of how it will work!

# Settings

The behavior of the toolkit can be adjusted by editing the settings of the application. This can be done by
editing the settings file. On linux systems it will be located at ```~/.config/ORB/rbdl-toolkit.conf```.

For example, it is possible to tell *rbdl-toolkit* to use a different seperator when reading csv files, by setting
the csv_seperator parameter to the ascii value of the desired seperator.

# Animation
To load a model animation the AnimationPlugin needs to be active. *rbdl-toolkit* does not support the custom
file header format used by Meshup. So you will need to use a standart csv format.

# Extras
In the extra folders are some python skripts that might be usefull when converting models from meshup to
rbdl-toolkit.

