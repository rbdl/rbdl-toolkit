#!/bin/bash

QT_DIR=${CUSTOM_QT_PATH}
APPDIR=${CMAKE_BINARY_DIR}/rbdl-toolkit.app

make install
if (( $? )); then
    exit 1
fi

rsync -r --exclude="*.dSYM" $QT_DIR/plugins/renderers $APPDIR/Contents/PlugIns
rsync -r --exclude="*.dSYM" $QT_DIR/plugins/renderplugins $APPDIR/Contents/PlugIns

$QT_DIR/bin/macdeployqt $APPDIR -dmg
