set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++2a -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB SOURCES "*/*.cpp")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/trading)
# the library name is libtrading, the type of the library is static. 
# The "strategy/risk_manager.cpp" is an additional source code files that should be included in the library.
add_library(libtrading STATIC ${SOURCES} strategy/risk_manager.cpp)
