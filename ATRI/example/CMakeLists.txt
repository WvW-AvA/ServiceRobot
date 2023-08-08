PROJECT(atri)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定为C++11 版本
set(CMAKE_CXX_STANDARD 11)

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

add_compile_options(-g  -Wall -O2)

ADD_DEPENDENCIES(atri frame utility)
TARGET_LINK_LIBRARIES(atri frame utility ${Boost_LIBRARIES})
