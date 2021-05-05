PROJECT(PythonPlugin)
	
SET(Plugin_SRCS
	PythonPlugin.cc
)

SET(Python_SRCS
  EmbeddedPython.cc
)

SET(PythonPlugin_MOC_HDRS
	include/toolkit_interfaces.h
	PythonPlugin.h
	EmbeddedPython.h
)

SET(CMAKE_AUTORCC ON )
SET(CMAKE_AUTOUIC ON )
SET(CMAKE_AUTOMOC ON )

find_package(Python COMPONENTS Interpreter Development REQUIRED)
FIND_PACKAGE(pybind11 REQUIRED)
IF (USE_QT6)
  MESSAGE(WARNING "Python Addon requires support for QTermWidget which is not availbe for Qt6 yet, so the python plugin can not be built!")
  RETURN()
ENDIF (USE_QT6)
FIND_PACKAGE (QTermWidget5 0.8.0 REQUIRED)
ADD_DEFINITIONS (${QT_DEFINITIONS})

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${RBDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${EIGEN3_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${pybind11_INCLUDE_DIRS})

ADD_DEFINITIONS (-DQT_PLUGIN)
ADD_DEFINITIONS (-DQT_SHARED)

ADD_LIBRARY (embeddedpython STATIC ${Python_SRCS})
TARGET_COMPILE_DEFINITIONS(embeddedpython PRIVATE -DQT_NO_KEYWORDS ${pybind11_DEFINITIONS})
TARGET_COMPILE_OPTIONS(embeddedpython PRIVATE -fPIC)
TARGET_LINK_OPTIONS(embeddedpython PRIVATE
  -export-dynamic
  -static
  -Wl,--whole-archive pybind11::embed -Wl,--no-whole-archive)
#SET_TARGET_PROPERTIES(embeddedpython PROPERTIES CXX_STANDARD 11)
TARGET_LINK_LIBRARIES(embeddedpython PUBLIC
  	toolkitlib
	pybind11::embed
)

ADD_LIBRARY (pythonplugin SHARED ${Plugin_SRCS})
TARGET_COMPILE_OPTIONS(pythonplugin PRIVATE -fPIC)
TARGET_LINK_OPTIONS(pythonplugin PRIVATE -export-dynamic)
#SET_TARGET_PROPERTIES(pythonplugin PROPERTIES CXX_STANDARD 11)
TARGET_LINK_LIBRARIES (pythonplugin
	${QT_LIBRARIES} 
	${QT_QTXML_LIBRARY} 
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::3DCore
	Qt5::3DExtras
	Qt5::3DRender
	Qt5::3DInput
	${RBDL_LIBRARY}
	qtermwidget5
	toolkitlib
	embeddedpython
)

INSTALL( TARGETS pythonplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR})	
