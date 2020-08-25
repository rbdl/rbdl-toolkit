#!/bin/bash

customqt=false
for arg in "$@"
do
	if [ "$arg" == "-customqt" ]
	then
		customqt=true
	fi
done


# install application to appdir
make install DESTDIR=rbdl-toolkit.AppDir

# populate appdir with need libs
linuxdeploy --appdir=rbdl-toolkit.AppDir -e rbdl-toolkit -i ../extra/rbdl-toolkit.png --create-desktop-file

# pull correct qt dependencies into appdir
if [ "$customqt" == true ]
then
	QMAKE=${CUSTOM_QT_PATH}/bin/qmake linuxdeploy-qt --appdir=rbdl-toolkit.AppDir
else
	linuxdeploy-qt --appdir=rbdl-toolkit.AppDir
fi

#create AppImage
appimagetool rbdl-toolkit.AppDir
