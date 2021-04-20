RBDL - Toolkit [![Build Status](https://orb.iwr.uni-heidelberg.de/buildbot/buildStatus/icon?job=rbdl-toolkit-release)](https://orb.iwr.uni-heidelberg.de/buildbot/job/rbdl-toolkit-release/)
==============

This application is intended to be used to visualize results and data of rigid body dynamics systems. It is build on top of [RBDL](https://github.com/ORB-HD/rbdl-orb) and uses Qt3D as it's graphics backend. RBDL-Toolkit is designed to be extendable via plugins and to make developing plugins easy there is a [plugin template](https://github.com/ORB-HD/toolkit-plugin-template). 

RBDL-Toolkit now supports loading and displaying of Lua and URDF models!

# How to get this software

This software is provided as an [AppImage](https://github.com/ORB-HD/rbdl-toolkit/releases/download/v1.1.1/rbdl-toolkit-x86_64.AppImage) for Linux and as an [DMG](https://github.com/ORB-HD/rbdl-toolkit/releases/download/v1.1.1/rbdl-toolkit.dmg) for MACOS. It may even be possible to get this running on Windows by using the Linux Subsystem, and the AppImage, but it has not be tested yet.

```
wget https://github.com/ORB-HD/rbdl-toolkit/releases/latest/download/rbdl-toolkit-x86_64.AppImage
wget https://github.com/ORB-HD/rbdl-toolkit/releases/latest/download/toolkit-buildplugin-x86_64.AppImage
chmod +x rbdl-toolkit-x86_64.AppImage
./ rbdl-toolkit-x86_64.AppImage
```

If you need to compile this code from source, please see the Wiki page on [Build from Source](https://github.com/ORB-HD/rbdl-toolkit/wiki/Build-From-Source)

# Wiki

For tutorials and functionality extensions to the toolkit take a look at our [wiki](https://github.com/ORB-HD/rbdl-toolkit/wiki). It will cover the basics of creating a new plugin and other information on how to use qt3d.

# Extras

In the extra folders are some python scripts that might be useful when converting model meshes from meshup to rbdl-toolkit.

# Issues

If you find bugs or have other questions please file a GitHub issue on this repository.

# License

This work is licensed with the EUPL v1.2.
