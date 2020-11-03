PROJECT(urdf)


INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor/tinyxml/include
)

SET( URDF_SRCS
  src/check_urdf.cpp
  src/joint.cpp
  src/link.cpp
  src/model.cpp
  src/pose.cpp
  src/twist.cpp
  src/urdf_model_state.cpp
  src/urdf_sensor.cpp
  src/world.cpp
)

ADD_LIBRARY( urdf STATIC
  ${URDF_SRCS}
)

TARGET_LINK_LIBRARIES( urdf
  tinyxml
 )
