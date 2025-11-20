# CMakeLists.txt
# This file is used to build the Sentinel application.
# It is used to configure the build environment and the dependencies.
# It is used to build the application.

# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.22)

# --- Cross-Platform Configuration ---
# Detect operating system
if(WIN32)
    set(SENTINEL_PLATFORM "Windows")
    set(SENTINEL_PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(SENTINEL_PLATFORM "macOS")
    set(SENTINEL_PLATFORM_MACOS TRUE)

elseif(UNIX)
    set(SENTINEL_PLATFORM "Linux")
    set(SENTINEL_PLATFORM_LINUX TRUE)
endif()


project(Sentinel LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Windows 10 API level for all Windows builds
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00) # Windows 10
endif()

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- GPU Acceleration Support ---
option(SENTINEL_USE_GPU "Enable experimental GPU compute paths" OFF)

# --- Performance Optimization ---
include(CheckCXXCompilerFlag)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        # MSVC specific flags
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL /DNDEBUG")
    else()
        # Flags for other compilers (GCC, Clang, etc.)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
        
        # You can still have OS-specific flags here
        if(APPLE)
             set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
        endif()
    endif()
endif()

# =============================================================================
# DEPENDENCY MANAGEMENT
# =============================================================================
include(FetchContent)

# --- JSON (vcpkg) ---
find_package(nlohmann_json REQUIRED)

# --- Dependencies via vcpkg ---
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# --- JWT-CPP (header only) ---
FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG v0.7.0
)
FetchContent_MakeAvailable(jwt-cpp)

# --- GoogleTest ---
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# Find top-level dependencies needed by multiple components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts Network Quick Qml QuickWidgets Test)

# Automatically run MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (MSVC)
    add_compile_options(/bigobj)
endif()

# Add the library and application subdirectories
add_subdirectory(libs/core)
add_subdirectory(libs/gui)
add_subdirectory(apps/sentinel_gui)
add_subdirectory(apps/stream_cli)
# add_subdirectory(tests)  # TODO: Update tests for V2 architecture
enable_testing()
add_subdirectory(tests/marketdata)

# --- Print Configuration Summary ---
message(STATUS "=== Sentinel Build Configuration ===")
message(STATUS "Platform: ${SENTINEL_PLATFORM}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "GPU Experimental: ${SENTINEL_USE_GPU}")
message(STATUS "=====================================")