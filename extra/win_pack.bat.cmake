"${CMAKE_BINARY_DIR}/win_install.bat" "${INSTALLER_PACKAGE_DATA_DIR}"
dir "${INSTALLER_DIR}"
"${IFW_BINARYCREATOR}" --offline-only -c config/config.xml -p package RBDLToolkitInstaller.exe
