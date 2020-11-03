PROJECT (tinyxml)

SET(TINYXML_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(TINYXML_LIBRARY tinyxml)

INCLUDE_DIRECTORIES (
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tinyxml
)

SET( TINYXML_SRCS
  src/tinystr.cpp
  src/tinyxml.cpp
  src/tinyxmlerror.cpp
  src/tinyxmlparser.cpp
)

ADD_LIBRARY( tinyxml STATIC
  ${TINYXML_SRCS}
)
