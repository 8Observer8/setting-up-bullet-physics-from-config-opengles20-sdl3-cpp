cmake_minimum_required(VERSION 3.20)

project(setting-up-bullet-physics-from-config-opengles20-sdl3-cpp)
# USE the WIN32 key to hide the console like this:
# add_executable(app WIN32)
add_executable(app)

# Check for WebAssembly
if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    target_sources(app
    PRIVATE
        src/main.cpp
    )
endif()

# Check for Windows
if (WIN32)
    target_include_directories(app PRIVATE E:/libs/glad-2.0.6/include)

    target_sources(app
    PRIVATE
        E:/libs/glad-2.0.6/src/glad.c
        src/main.cpp
    )

    # This code copies the "SDL3.dll" file to the build directory
    add_custom_command(TARGET app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:app> $<TARGET_FILE_DIR:app>
    )

    target_link_options(app PRIVATE -static)
endif()

include_directories(${BULLET_INCLUDE_DIR})
find_package(SDL3)
target_link_libraries(app PRIVATE ${BULLET_DYNAMICS_LIBRARY} ${BULLET_COLLISION_LIBRARY} ${BULLET_MATH_LIBRARY} SDL3::SDL3)

target_compile_definitions(app PRIVATE SDL_MAIN_USE_CALLBACKS)
