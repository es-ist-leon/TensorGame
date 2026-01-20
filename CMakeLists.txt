cmake_minimum_required(VERSION 3.14)
project(TensorLearningGame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# Source files
set(SOURCES
    src/main.cpp
    src/tensor/Tensor.cpp
    src/tensor/TensorDB.cpp
    src/gui/Application.cpp
    src/gui/TensorVisualizer.cpp
    src/gui/UIComponents.cpp
    src/lessons/LessonManager.cpp
    src/lessons/Lesson1_Basics.cpp
    src/lessons/Lesson2_Creation.cpp
    src/lessons/Lesson3_Operations.cpp
    src/lessons/Lesson4_Database.cpp
    src/lessons/Lesson5_Broadcasting.cpp
    src/lessons/Lesson6_NeuralNetworks.cpp
    src/sandbox/Sandbox.cpp
    src/quiz/Quiz.cpp
    src/progress/Achievements.cpp
    src/editor/CodeEditor.cpp
)

# Header files
set(HEADERS
    src/tensor/Tensor.hpp
    src/tensor/TensorDB.hpp
    src/gui/Application.hpp
    src/gui/TensorVisualizer.hpp
    src/gui/UIComponents.hpp
    src/gui/Colors.hpp
    src/lessons/LessonManager.hpp
    src/lessons/Lesson.hpp
    src/sandbox/Sandbox.hpp
    src/quiz/Quiz.hpp
    src/progress/Achievements.hpp
    src/editor/CodeEditor.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Platform specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
