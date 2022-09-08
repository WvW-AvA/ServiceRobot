PROJECT(example)

FILE(GLOB example_SRCS
    devil.hpp
    devil.cpp
    main.cpp
)
SOURCE_GROUP("" FILES ${example_SRCS})

ADD_EXECUTABLE(example ${example_SRCS})

ADD_DEPENDENCIES(example frame utility)
TARGET_LINK_LIBRARIES(example frame utility ${Boost_LIBRARIES})
