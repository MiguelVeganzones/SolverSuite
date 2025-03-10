cmake_minimum_required(VERSION 3.30)
project(SolverSuite CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_BUILD_TYPE Release)
set(OUT_TAIL_DIR "release")

set(RELEASE_CXX_FLAGS
	-O3
	-Wall
	-Wconversion
	-Wdangling-pointer
	-Werror
	-Wextra
	-Wmisleading-indentation
	-Wreturn-local-addr
	-Wshadow
	-Wuninitialized
	-fbounds-check
	-fconcepts-diagnostics-depth=3
	-fdiagnostics-color=always
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-fdiagnostics-show-template-tree
	-ffinite-math-only
	-fno-exceptions
	-fno-omit-frame-pointer
	-fno-rtti
	-fopt-info-vec-all
	-fstrength-reduce
	-fvisibility=hidden
	-march=native
	-mavx
	-pedantic
)

# Output directories
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${OUT_TAIL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

include_directories(
    ../solver_suite/Allocators/
    ../solver_suite/DataTypes/
    ../solver_suite/Utility/
    ../solver_suite/Plotting/
    ../solver_suite/Solvers/
)

set(BENCHMARKS_DIR "./")

find_package(benchmark REQUIRED)
# Generate executable for all .b.cpp
file(GLOB BENCHMARK_FILES "${BENCHMARKS_DIR}/*.b.cpp")
foreach(BENCHMARK_FILE ${BENCHMARK_FILES})
    get_filename_component(TARGET_NAME ${BENCHMARK_FILE} NAME_WE)
    set(TARGET_NAME "${TARGET_NAME}_bench")
    add_executable(${TARGET_NAME} ${BENCHMARK_FILE})
    target_compile_options(${TARGET_NAME} PRIVATE ${CXX_FLAGS})
    target_link_options(${TARGET_NAME} PRIVATE ${LINK_FLAGS})
    target_link_libraries(${TARGET_NAME} benchmark::benchmark)
endforeach()


file(GLOB PROFILE_FILES "${BENCHMARKS_DIR}/*.b.perf.cpp")
foreach(PROFILE_FILE ${PROFILE_FILES})
    get_filename_component(TARGET_NAME ${PROFILE_FILE} NAME_WE)
    set(TARGET_NAME "${TARGET_NAME}_perf")
    add_executable(${TARGET_NAME} ${PROFILE_FILE})
    target_compile_options(${TARGET_NAME} PRIVATE "${CXX_FLAGS};-ggdb3")
    target_link_options(${TARGET_NAME} PRIVATE ${LINK_FLAGS})
endforeach()

# Build messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${OUTPUT_DIR}")
