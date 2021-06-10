make install DESTDIR=RBDLToolkit_Deploy

${WINDEPLOYQT_EXECUTABLE} -3dcore -3drenderer -3dinput -3danimation -3dextras -3dquick -winextras "RBDLToolkit_Deploy\Program Files (x86)\RBDL-Toolkit\bin\rbdl-toolkit.exe"
copy "${RBDL_LIBRARY}" ".\RBDLToolkit_Deploy\Program Files (x86)\RBDL-Toolkit\bin\"
copy "${RBDL_LUAMODEL_LIBRARY}" ".\RBDLToolkit_Deploy\Program Files (x86)\RBDL-Toolkit\bin\"
copy "${RBDL_URDFREADER_LIBRARY}" ".\RBDLToolkit_Deploy\Program Files (x86)\RBDL-Toolkit\bin\"
copy "${CUSTOM_QT_PATH}/bin/Qt53DLogic.dll" ".\RBDLToolkit_Deploy\Program Files (x86)\RBDL-Toolkit\bin\"
cd %builddir%
