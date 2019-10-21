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

# Plugins

This Application is extendable with qt plugins. The Interface is not yet finished but you can take a look at the toolkit_inteface.h and
the AnimationPlugin to get an idea of how it will work!

# Settings
The behavior of the toolkit can be adjusted by editing the settings of the application. This can be done by
editing the settings file. On linux systems it will be located at ```~/.config/ORB/rbdl-toolkit.conf```.

