# User additions - safe from CubeMX overwrites
# Add extra source files here (paths are relative to project root)
# Example: add Core/Src/general_loop.c to the main target

if(TARGET ${CMAKE_PROJECT_NAME})
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        Core/Src/general_loop.c
    )
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        Core/Inc
    )
endif()
