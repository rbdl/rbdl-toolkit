#!/bin/bash

# skript to be deployed inside the toolkit-pluginbuilder.AppImage
# it performs all the required build steps and takes the Plugin Source tree as its argument

MOUNT=$APPDIR
echo $MOUNT
echo $PWD

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <Plugin_Source_Directory>"
	exit 1
fi

TOOLKIT_BUILD=$MOUNT/build/toolkit_build
TOOLKIT_SRC=$MOUNT/build/rbdl-toolkit
QT_PATH=$MOUNT/build/qt

# create plugin builddir
mkdir -p .pluginbuild
cd .pluginbuild

echo "Start Build"
