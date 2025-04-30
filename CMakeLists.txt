cmake_minimum_required(VERSION 3.10)
project(MissileSimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(missile_sim 
    main.cpp
)

# If you're on Windows, you might want this to copy the executable to the build directory
set_target_properties(missile_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Optional: Add compiler warnings
if(MSVC)
    target_compile_options(missile_sim PRIVATE /W4)
else()
    target_compile_options(missile_sim PRIVATE -Wall -Wextra)
endif()