cmake_minimum_required(VERSION 3.30)
project(SolverSuite CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

# Build type configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(DEBUG_CXX_FLAGS
	-O0
	-W
	-Wall
	-Wconversion
	-Wdangling-else
	-Wdouble-promotion
	-Wduplicated-branches
	-Wduplicated-cond
	-Werror
	-Wextra
	-Wfloat-equal
	-Wformat
	-Winvalid-pch
	-Wlogical-op
	-Wmisleading-indentation
	-Wnull-dereference
	-Wodr
	-Wpointer-arith
	-Wrestrict
	-Wreturn-local-addr
	-Wshadow
	-Wswitch-default
	-Wswitch-enum
	-Wuninitialized
	-Wvla
	-fbounds-check
	-fconcepts-diagnostics-depth=3
	-fdiagnostics-color=always
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-fdiagnostics-show-template-tree
	-ffinite-math-only
	-fmax-errors=5
	-fno-exceptions
	-fno-omit-frame-pointer
	-fvisibility=hidden
	-ggdb3
	-mavx
	-march=native
	-pedantic
	# -fno-rtti
)

set(RELEASE_CXX_FLAGS
	-fdiagnostics-color=always
	-fdiagnostics-show-template-tree
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-ggdb3
	-O3
	-Wall
	-Wextra
	-Wreturn-local-addr
	-Wshadow
	-ffinite-math-only
	-Wconversion
	-Wuninitialized
	-Wmisleading-indentation
	-Werror
	-pedantic
	-mavx
	-march=native
	-fstrength-reduce
	-fbounds-check
	-fconcepts-diagnostics-depth=3
	-fno-exceptions
	-fno-rtti
	-fopt-info-vec-all
	-fvisibility=hidden
)

set(FULL_RELEASE_CXX_FLAGS
	-fdiagnostics-color=always
	-fdiagnostics-show-template-tree
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-O3
	-Wall
	-Wextra
	-Wshadow
	-Wconversion
	-Wmisleading-indentation
	-Werror
	-fstrength-reduce
	-pedantic
	-mavx
	-march=native
	-fconcepts-diagnostics-depth=3
	-fno-exceptions
	-fno-math-errno
	-fno-rtti
	-fno-trapping-math
	-fopt-info-vec-all
	-fvisibility=hidden
)

set(SANITIZER_FLAGS
	-fsanitize=address
	-fsanitize=bounds
	-fsanitize=float-cast-overflow
	-fsanitize=float-divide-by-zero
	-fsanitize=integer-divide-by-zero
	-fsanitize=leak
	-fsanitize=null
	-fsanitize=signed-integer-overflow
	-fsanitize=undefined
)

# Set output directory based on build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CXX_FLAGS ${DEBUG_CXX_FLAGS})
    set(OUT_TAIL_DIR "debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CXX_FLAGS ${RELEASE_CXX_FLAGS})
    set(OUT_TAIL_DIR "release")
elseif(CMAKE_BUILD_TYPE STREQUAL "FullRelease")
    set(CXX_FLAGS ${FULL_RELEASE_CXX_FLAGS})
    set(OUT_TAIL_DIR "full_release")
else()
    message(ERROR "Unknown build type!")
endif()
message(STATUS "CXX Flags: ${CXX_FLAGS}")

# Output directories
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${OUT_TAIL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

include_directories(
    ../solver_suite
    ../solver_suite/Allocators/
    ../solver_suite/DataTypes/
    ../solver_suite/Utility/
    ../solver_suite/Plotting/
    ../solver_suite/Solvers/
)

# Optional features
if (NOT DEFINED ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(default_enable_sanitizers ON)
else()
    set(default_enable_sanitizers OFF)
endif()

if(ENABLE_SANITIZERS)
    list(APPEND CXX_FLAGS ${SANITIZER_FLAGS})
    list(APPEND LINK_FLAGS ${SANITIZER_FLAGS})
endif()

set(EXAMPLES_DIR "./")

# Generate executable for all .t.cpp
file(GLOB EXAMPLE_FILES "${EXAMPLES_DIR}/*.t.cpp")
foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
    get_filename_component(TARGET_NAME ${EXAMPLE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${EXAMPLE_FILE})
    target_compile_options(${TARGET_NAME} PRIVATE ${CXX_FLAGS})
    target_link_options(${TARGET_NAME} PRIVATE ${LINK_FLAGS})
endforeach()

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
set(PLT_CXX_FLAGS "${CXX_FLAGS};-Wno-cpp")
file(GLOB_RECURSE PLOTTING_FILES ../solver_suite/Plotting/*.cpp)
add_library(plotting STATIC ${PLOTTING_FILES})
add_compile_definitions(USE_ROOT_PLOTTING)
target_compile_options(plotting PRIVATE -pthread -m64 -Wno-cpp)
target_link_libraries(plotting PRIVATE ${ROOT_LIBRARIES})

# Generate executable for all .t.plt.cpp
file(GLOB PLT_EXAMPLE_FILES "${EXAMPLES_DIR}/*.t.plt.cpp")
foreach(PLT_EXAMPLE_FILE ${PLT_EXAMPLE_FILES})
    get_filename_component(TARGET_NAME ${PLT_EXAMPLE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${PLT_EXAMPLE_FILE})
    target_compile_options(${TARGET_NAME} PRIVATE ${PLT_CXX_FLAGS})
    target_link_options(${TARGET_NAME} PRIVATE ${LINK_FLAGS})
    target_link_libraries(${TARGET_NAME} PRIVATE plotting)
endforeach()

# Build messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${OUTPUT_DIR}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
