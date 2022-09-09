PROJECT(atri)

FILE(GLOB atri_SRCS
    atri.hpp
    atri.cpp
    main.cpp
)
SOURCE_GROUP("" FILES ${atri_SRCS})

ADD_EXECUTABLE(atri ${atri_SRCS})

ADD_DEPENDENCIES(atri frame utility)
TARGET_LINK_LIBRARIES(atri frame utility ${Boost_LIBRARIES})
