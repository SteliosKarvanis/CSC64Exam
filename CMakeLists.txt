# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(CSC64Exam VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

file(GLOB SOURCES "src/*.cpp")

# Find OpenMP package
find_package(OpenMP REQUIRED)

# Add executable target with main.c located in the db folder
add_executable(gen_db 
    db/db.c
)

add_executable(process ${SOURCES})

# Include directories
target_include_directories(process PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link OpenMP
if(OpenMP_C_FLAGS)
    target_compile_options(process PRIVATE ${OpenMP_C_FLAGS})
endif()
if(OpenMP_C_LIBRARIES)
    target_link_libraries(process PRIVATE ${OpenMP_C_LIBRARIES})
endif()


set_target_properties(gen_db PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
set_target_properties(process PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)