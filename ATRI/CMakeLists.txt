PROJECT(planner)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6.0)

SET(ROOT_PATH ${planner_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH "${ROOT_PATH}/bin")
SET(LIBRARY_OUTPUT_PATH "${ROOT_PATH}/lib")
SET(BOOST_ROOT "" CACHE PATH "Boost root path")
MESSAGE(STATUS "ROOT PATH IS ${ROOT_PATH}")
MESSAGE(STATUS "BIN PATH IS ${OUTPUT_BIN_DIR}")
MESSAGE(STATUS "LIB PATH IS ${OUTPUT_LIB_DIR}")


SET(Boost_USE_STATIC_LIBS        OFF)
SET(Boost_USE_MULTITHREADED      ON)
SET(Boost_USE_STATIC_RUNTIME     OFF)
FIND_PACKAGE(Boost COMPONENTS chrono thread system date_time regex REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ROOT_PATH}/include)
INCLUDE_DIRECTORIES (${ROOT_PATH}/src)

IF(WIN32)
ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
ENDIF(WIN32)
ADD_DEFINITIONS(-DTIXML_USE_STL)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)

IF (Boost_FOUND)
  MESSAGE( STATUS "BOOST DIR : ${Boost_INCLUDE_DIRS}")
  MESSAGE( STATUS "BOOST LIBDIR : ${Boost_LIBRARIES}")
ENDIF()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(example)
ADD_SUBDIRECTORY(tools/gen_tests)
