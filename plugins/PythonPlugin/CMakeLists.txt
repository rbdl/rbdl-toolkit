PROJECT(PythonPlugin)
	
SET(Plugin_SRCS
	PythonPlugin.cc
)

SET(Python_SRCS
  EmbeddedPython.cc
)

SET(PythonPlugin_MOC_HDRS
	PythonSocket.h
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

QT5_WRAP_CPP ( PYTHON_MOC_SRCS
	${PythonPlugin_MOC_HDRS}
)

ADD_LIBRARY (pythonsocket STATIC PythonSocket.cc
	${PYTHON_MOC_SRCS}
)

TARGET_LINK_LIBRARIES (pythonsocket
	${QT_LIBRARIES}
	${QT_QTXML_LIBRARY}
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::3DCore
	Qt5::3DExtras
	Qt5::3DRender
	Qt5::3DInput
	embeddedpython
)

ADD_LIBRARY (embeddedpython STATIC ${Python_SRCS})
TARGET_COMPILE_DEFINITIONS(embeddedpython PRIVATE -DQT_NO_KEYWORDS ${pybind11_DEFINITIONS})
TARGET_LINK_LIBRARIES(embeddedpython PUBLIC
  toolkitlib
	pybind11::embed
	pythonsocket
	${Python_LIBRARIES}
)

ADD_LIBRARY (pythonplugin SHARED ${Plugin_SRCS})
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
	pythonsocket
)

INSTALL( TARGETS pythonplugin LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR})	
