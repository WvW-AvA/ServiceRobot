PROJECT(validator)

FILE(GLOB validator_SRCS
    validator.hpp
    validator.cpp
    main.cpp
)
SOURCE_GROUP("" FILES ${validator_SRCS})

ADD_EXECUTABLE(validator ${validator_SRCS})

ADD_DEPENDENCIES(validator frame utility)
TARGET_LINK_LIBRARIES(validator frame utility ${Boost_LIBRARIES})
