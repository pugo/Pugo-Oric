
add_library(utils
        circular_buffer.hpp
        sha1.hpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(utils
        PUBLIC
        ${AURIC_COMMON_LIBRARIES}
)

