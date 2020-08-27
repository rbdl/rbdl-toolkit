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

if (( $? )); then
	exit 1
fi

# populate appdir with need libs
linuxdeploy --appdir=rbdl-toolkit.AppDir -e rbdl-toolkit -i ../extra/rbdl-toolkit.png --create-desktop-file

if (( $? )); then
	exit 1
fi

# pull correct qt dependencies into appdir
if [ "$customqt" == true ]
then
	QMAKE=${CUSTOM_QT_PATH}/bin/qmake linuxdeploy-qt --appdir=rbdl-toolkit.AppDir -p libdefaultgeometryloader.so -p 3dquickrender
else
	linuxdeploy-qt --appdir=rbdl-toolkit.AppDir
fi

if (( $? )); then
	exit 1
fi

#create AppImage
appimagetool rbdl-toolkit.AppDir

exit 0
