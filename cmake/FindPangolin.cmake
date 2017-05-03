# - Try to find libPangolin
#
#  Pangolin_FOUND - system has libPangolin
#  Pangolin_INCLUDE_DIR - the libPangolin include directories
#  Pangolin_LIBRARY - link these to use libPangolin

FIND_PATH(
  Pangolin_INCLUDE_DIR_SRC
  NAMES pangolin/pangolin.h
  PATHS
    ${PROJECT_SOURCE_DIR}/submodules/Pangolin/include
    /usr/include
    /usr/local/include
)

FIND_PATH(
  Pangolin_INCLUDE_DIR_BUILD
  NAMES pangolin/config.h
  PATHS
    ${PROJECT_SOURCE_DIR}/submodules/Pangolin/release/src/include
    ${PROJECT_SOURCE_DIR}/submodules/Pangolin/build/src/include
    /usr/include
    /usr/local/include
)

FIND_LIBRARY(
  Pangolin_LIBRARY
  NAMES pangolin
  PATHS
    ${PROJECT_SOURCE_DIR}/submodules/Pangolin/release/src/
    ${PROJECT_SOURCE_DIR}/submodules/Pangolin/build/src/
    /usr/lib
    /usr/local/lib
) 

IF(Pangolin_INCLUDE_DIR_SRC AND Pangolin_INCLUDE_DIR_BUILD AND Pangolin_LIBRARY)
  SET(Pangolin_INCLUDE_DIR ${Pangolin_INCLUDE_DIR_SRC};${Pangolin_INCLUDE_DIR_BUILD})
  SET(Pangolin_FOUND TRUE)
ENDIF()

IF(Pangolin_FOUND)
   IF(NOT Pangolin_FIND_QUIETLY)
      MESSAGE(STATUS "Found Pangolin: ${Pangolin_LIBRARY}")
   ENDIF()
ELSE()
   IF(Pangolin_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Pangolin")
   ENDIF()
ENDIF()