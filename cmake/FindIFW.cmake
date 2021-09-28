SET (IFW_FOUND FALSE)

UNSET(IFW_PATH CACHE)


IF(IFW_INSTALL_PATH)
  FIND_PATH(IWF_PATH bin/binarycreator.exe
    PATHS
    IFW_INSTALL_PATH
    NO_DEFAULT_PATH
  )

  IF( IFW_PATH )
    SET(IFW_FOUND TRUE)
    FIND_PROGRAM( IFW_BINARYCREATOR bin/binarycreator.exe
      PATHS
      IFW_PATH
      NO_DEFAULT_PATH
    )
    FIND_PROGRAM( IFW_BINARYCREATOR bin/archivegen.exe
      PATHS
      IFW_PATH
      NO_DEFAULT_PATH
    )
  ELSE( IFW_PATH )
    MESSAGE(WARNING "IFW_PATH is set, but could not find required files. Please provide the correct path to the Qt Installer Framework!")
  ENDIF( IFW_PATH )

ELSE(IFW_INSTALL_PATH)
  MESSAGE(WARNING "IFW_INSTALL_PATH is not set, if you wish to create an application installer please install the Qt Installer Framework!")
ENDIF(IFW_INSTALL_PATH)

MARK_AS_ADVANCED(
  IFW_PATH
  IFW_BINARYCREATOR
  IFW_ARCHIVETOOL
)
