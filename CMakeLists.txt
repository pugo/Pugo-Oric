cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O3 -Wno-unsequenced")

add_definitions(-DBOOST_LOG_DYN_LINK -DBOOST_SPIRIT_DEBUG -DBOOST_SPIRIT_DEBUG_ -DBOOST_SPIRIT_DEBUG_FLAGS_TREES)


project(oric)
find_package(Boost 1.71.0 COMPONENTS program_options log)
find_package(SDL2 REQUIRED)

set(ORIC_SOURCES oric.cpp memory.cpp machine.cpp frontend.cpp monitor.cpp
        oric.hpp)

if(Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} .)

   include(chip/CMakeLists.txt)
   include(tape/CMakeLists.txt)

   add_executable(oric ${ORIC_SOURCES})

   target_link_libraries(oric ${Boost_LIBRARIES} ${SDL2_LIBRARIES})
endif()


install(TARGETS oric RUNTIME DESTINATION bin)







#target_link_libraries(SDL2Test )
