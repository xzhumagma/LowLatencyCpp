cmake_minimum_required(VERSION 3.0)

project(LowLatencyApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++2a -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_VERBOSE_MAKEFILE on)
#Adds the 'common' and 'exchange' subdirectories to the build, which should each contain their own CMakeLists.txt.
add_subdirectory(common)
add_subdirectory(exchange)
# append 'libcommon' 'libexchange' and 'pthread' to the list of libraries to be linked
list(APPEND LIBS libcommon)
list(APPEND LIBS libexchange)
list(APPEND LIBS pthread)
# add the 'common' and 'exchange' directories to the list of directories to be searched for include files
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/exchange)
# Defines the executable 'exchange_main' which is built from the source file 'exchange/exchange_main.cpp'
add_executable(exchange_main exchange/exchange_main.cpp)
# Links the executable 'exchange_main' to the libraries in the list 'LIBS'
target_link_libraries(exchange_main PUBLIC ${LIBS})

