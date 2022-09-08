PROJECT(src)

SET(CSERVER_INC_DIR "${ROOT_PATH}/include/cserver")

FILE(GLOB cserver_SRCS
  main.cpp
  platform.hpp
  platform.cpp
  "${CSERVER_INC_DIR}/grader.hpp"
  "${CSERVER_INC_DIR}/message.hpp"
  message.cpp
  "${CSERVER_INC_DIR}/param.hpp"
  param.cpp
  "${CSERVER_INC_DIR}/serialization.hpp"
  serialization.cpp
  "${CSERVER_INC_DIR}/xmllabel.hpp"
  xmllabel.cpp
)

FILE(GLOB utility_SRCS
  sharedlib.cpp
)
AUX_SOURCE_DIRECTORY(tinyxml tinyxml_SRCS)
AUX_SOURCE_DIRECTORY(sfsexp sfsexp_SRCS)

FILE(GLOB frame_SRCS
  "${CSERVER_INC_DIR}/plug.hpp"
  plug.cpp
  "${CSERVER_INC_DIR}/message.hpp"
  message.cpp
  "${CSERVER_INC_DIR}/param.hpp"
  param.cpp
  "${CSERVER_INC_DIR}/serialization.hpp"
  serialization.cpp
  "${CSERVER_INC_DIR}/xmllabel.hpp"
  xmllabel.cpp
)

FILE(GLOB grader_SRCS
    evaluate.h
    evaluate.cpp
    aspgrader.hpp
    aspgrader.cpp
)

SOURCE_GROUP("" FILES ${cserver_SRCS})
SOURCE_GROUP("tinyxml" FILES ${tinyxml_SRCS})
SOURCE_GROUP("sfsexp" FILES ${sfsexp_SRCS})
SOURCE_GROUP("" FILES ${utility_SRCS} ${tinyxml_SRCS} ${sfsexp_SRCS})
SOURCE_GROUP("" FILES ${frame_SRCS})
SOURCE_GROUP("" FILES ${grader_SRCS})

INCLUDE_DIRECTORIES(${ROOT_PATH}/include)

ADD_EXECUTABLE(cserver ${cserver_SRCS})
ADD_LIBRARY(utility STATIC ${utility_SRCS} ${tinyxml_SRCS} ${sfsexp_SRCS})
ADD_LIBRARY(frame STATIC ${frame_SRCS})
ADD_LIBRARY(asp SHARED ${grader_SRCS})
IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  SET_TARGET_PROPERTIES(utility PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

ADD_DEPENDENCIES(cserver utility)
ADD_DEPENDENCIES(frame utility)
ADD_DEPENDENCIES(asp utility)
IF(WIN32)
  TARGET_LINK_LIBRARIES(cserver utility ${Boost_LIBRARIES})
  TARGET_LINK_LIBRARIES(frame utility)
ELSE(WIN32)
  TARGET_LINK_LIBRARIES(cserver utility dl ${Boost_LIBRARIES} pthread)
  TARGET_LINK_LIBRARIES(frame utility ${Boost_LIBRARIES} pthread)
ENDIF(WIN32)
TARGET_LINK_LIBRARIES(asp utility ${Boost_LIBRARIES})
