cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Debug)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_CXX_FLAGS "-O3 -Wno-unsequenced")
set(CMAKE_CXX_FLAGS "-O3 -pg")

add_definitions(-DBOOST_LOG_DYN_LINK -DBOOST_SPIRIT_DEBUG -DBOOST_SPIRIT_DEBUG_ -DBOOST_SPIRIT_DEBUG_FLAGS_TREES)


project(oric)
find_package(Boost 1.71.0 COMPONENTS program_options log)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

set (LIB_SOURCES
        oric.cpp
        memory.cpp
        machine.cpp
        frontend.cpp
        monitor.cpp
        config.cpp
        snapshot.cpp
        oric.hpp
)


include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} .)

include(chip/CMakeLists.txt)
include(tape/CMakeLists.txt)

add_executable(oric main.cpp ${LIB_SOURCES} )
target_link_libraries(oric ${Boost_LIBRARIES} ${SDL2_LIBRARIES} SDL2_image)

add_library(oric_lib STATIC ${LIB_SOURCES})
target_link_libraries(oric_lib ${Boost_LIBRARIES} ${SDL2_LIBRARIES} SDL2_image)

install(TARGETS oric RUNTIME DESTINATION bin)

enable_testing()
add_subdirectory(tests)



