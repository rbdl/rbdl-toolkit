#!/bin/bash

# skript to be deployed inside the toolkit-pluginbuilder.AppImage
# it performs all the required build steps and takes the Plugin Source tree as its argument

# Todo: maybe add some more options to this for multiple threads and cleanup

MOUNT=$APPDIR
#echo $MOUNT
#echo $PWD
#echo $PATH

if [ "$#" -ne 1 ]; then
	echo "Usage: $APPIMAGE <Plugin_Source_Directory>"
	exit 1
fi

PLUGIN_SRC=$PWD/$1
PLUGIN_BUILD=.$PLUGIN_SRC-build
TOOLKIT_BUILD=$MOUNT/build/toolkit_build
TOOLKIT_SRC=$MOUNT/build/rbdl-toolkit
QT_PATH=$MOUNT/build/qt

#modify path to use buildtools deployed in the appimage
PATH=$MOUNT/usr/bin:$PATH

# create plugin builddir
mkdir -p $PLUGIN_BUILD
cd $PLUGIN_BUILD

echo "Start Build"
cmake -DCUSTOM_QT_PATH=$QT_PATH -DTOOLKIT_BUILD_PATH=$TOOLKIT_BUILD -DTOOLKIT_SOURCE_PATH=$TOOLKIT_SRC $PLUGIN_SRC
make -j 4
cp *.so ..
