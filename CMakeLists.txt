cmake_minimum_required(VERSION 3.10)
project(MyHeaderOnlyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the header-only files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find GoogleTest package
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add test sources
file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp)

# Create the test executable
add_executable(runTests ${TEST_SOURCES})

# Link with GTest and pthread
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)

target_compile_options(runTests PRIVATE -pedantic -Wall -Wextra -Werror -O2)

# Add a custom target to run tests after build
add_custom_command(TARGET runTests
    POST_BUILD
    COMMAND runTests
    COMMENT "Running unit tests..."
)
