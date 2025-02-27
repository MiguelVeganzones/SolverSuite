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
	-fvisibility=hidden
	-ggdb3
	-mavx
	-march=native
	-pedantic
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
	-fno-math-errno
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

# Build messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${OUTPUT_DIR}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Boost logging: ${BOOST_LOGGING}")
message(STATUS "Fast math: ${FFAST_MATH}")
