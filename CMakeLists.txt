cmake_minimum_required(VERSION 3.2)
project(PathPlanning)


if (${MINGW})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I/opt/local/include -L/opt/local/lib -lcairo -O2 -Wall -Wextra")
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

enable_testing()

add_subdirectory(Src)
add_subdirectory(Tests)