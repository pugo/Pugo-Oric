

if (DEFINED CMAKE_TOOLCHAIN_FILE)
    message("Using vcpkg")

    find_package(Boost REQUIRED COMPONENTS circular_buffer)
    find_package(argparse CONFIG REQUIRED)
    find_package(glad CONFIG REQUIRED)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(spdlog CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(yaml-cpp CONFIG REQUIRED)

    set(BOOST_LIBRARIES Boost::circular_buffer)
    set(SDL3_LIBRARIES SDL3::SDL3)
    set(GLAD_TARGET glad::glad)
else ()
    message("Not using vcpkg")

    find_package(Boost 1.71.0 REQUIRED)
    find_package(argparse CONFIG REQUIRED)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(spdlog CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(yaml-cpp REQUIRED)

    set(BOOST_LIBRARIES Boost::boost)
    set(SDL3_LIBRARIES SDL3::SDL3)

    add_library(glad_vendor STATIC
            third_party/glad/src/gl.c
    )
    target_include_directories(glad_vendor
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
    )
    target_link_libraries(glad_vendor PUBLIC OpenGL::GL)
    set(GLAD_TARGET glad_vendor)
endif ()

set(AURIC_COMMON_LIBRARIES
        ${BOOST_LIBRARIES}
        spdlog::spdlog
)

add_subdirectory(chip)
add_subdirectory(disk)
add_subdirectory(tape)

add_subdirectory(frontends)

# Static library (for reuse by tests, etc.)
add_library(auric_lib STATIC
        oric.cpp
        debugger_controller.cpp
        memory.cpp
        machine.cpp
        monitor.cpp
        config.cpp
        snapshot.cpp
)

target_include_directories(auric_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(auric_lib PUBLIC chip frontend tape
                      ${AURIC_COMMON_LIBRARIES}
                      argparse::argparse
                      ${SDL3_LIBRARIES}
                      ${YAML_CPP_LIBRARIES})

# Link everything here (chip/tape/frontend libs defined in their subdirs)
target_link_libraries(auric_lib
        PUBLIC
        chip
        disk
        frontend
        tape
        ${AURIC_COMMON_LIBRARIES}
        argparse::argparse
)

# Main executable
add_executable(auric main.cpp)
target_link_libraries(auric PRIVATE auric_lib)

install(TARGETS auric RUNTIME DESTINATION bin)
