PROJECT(atri)

FILE(GLOB atri_SRCS
    atri.hpp
    debuglog.hpp
    parser.hpp
    atri.cpp
    main.cpp
    parser.cpp
)
SOURCE_GROUP("" FILES ${atri_SRCS})
ADD_EXECUTABLE(atri ${atri_SRCS})

add_compile_options(-g  -Wall -O3)

ADD_DEPENDENCIES(atri frame utility)
TARGET_LINK_LIBRARIES(atri frame utility ${Boost_LIBRARIES} -lunwind)
