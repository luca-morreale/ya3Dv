cmake_minimum_required(VERSION 3.10.2)

# Maybe stop from CMAKEing in the wrong place
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Source and build directories cannot be the same. Go use the /build directory.")
endif()

### Configure output locations
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



# ----------------------- GCC FLAGS ----------------------------

### Compiler options
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 ) # Emit a compile flags file to support completion engines

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using Clang (linux or apple) or GCC
  message("Using clang/gcc compiler flags")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  SET(BASE_CXX_FLAGS "-std=c++17 -lstdc++fs -Wall -Wextra -Werror -g3")
  SET(DISABLED_WARNINGS " -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-deprecated-declarations -Wno-missing-braces")
  SET(TRACE_INCLUDES " -H -Wno-error=unused-command-line-argument")

  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message("Setting clang-specific options")
    SET(BASE_CXX_FLAGS "${BASE_CXX_FLAGS} -ferror-limit=5 -fcolor-diagnostics")
    SET(CMAKE_CXX_FLAGS_DEBUG          "-fsanitize=address -fno-limit-debug-info")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(BASE_CXX_FLAGS "${BASE_CXX_FLAGS} -fmax-errors=5")
    message("Setting gcc-specific options")
    SET(DISABLED_WARNINGS "${DISABLED_WARNINGS} -Wno-maybe-uninitialized -Wno-format-zero-length -Wno-unused-but-set-parameter -Wno-unused-but-set-variable")
  endif()


  SET(CMAKE_CXX_FLAGS "${BASE_CXX_FLAGS} ${DISABLED_WARNINGS}")
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TRACE_INCLUDES}") # uncomment if you need to track down where something is getting included from
  SET(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -g3")
  SET(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
  SET(CMAKE_CXX_FLAGS_RELEASE        "-march=native -O3 -DNDEBUG")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
  message("Using Visual Studio compiler flags")
  set(BASE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(BASE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") # parallel build
  SET(DISABLED_WARNINGS "${DISABLED_WARNINGS} /wd\"4267\"")  # ignore conversion to smaller type (fires more aggressively than the gcc version, which is annoying)
  SET(DISABLED_WARNINGS "${DISABLED_WARNINGS} /wd\"4244\"")  # ignore conversion to smaller type (fires more aggressively than the gcc version, which is annoying)
  SET(DISABLED_WARNINGS "${DISABLED_WARNINGS} /wd\"4305\"")  # ignore truncation on initialization
  SET(CMAKE_CXX_FLAGS "${BASE_CXX_FLAGS} ${DISABLED_WARNINGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")
  add_definitions (-DNOMINMAX)
else()
  # unrecognized
  message( FATAL_ERROR "Unrecognized compiler [${CMAKE_CXX_COMPILER_ID}]" )
endif()


# --------------------------------------------------------------

# ------------------ EXTERNAL LIB INCLUDES ---------------------

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Eigen3 REQUIRED)

find_library(GLAD_LIB glad externals/libs)
find_library(STB_LIB stb externals/libs)
find_library(IMGUI_LIB imgui externals/libs)
find_library(POLYSCOPE_LIB polyscope externals/libs)


include_directories(include)
include_directories(externals/include)
include_directories(externals/include/imgui)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# option A
find_package(glfw3 REQUIRED)
# option B
# find_package(X11 REQUIRED)
# find_library(GLFW3_LIB glfw3 externals/libs)

set(LIBS ${POLYSCOPE_LIB}
        ${IMGUI_LIB}

        # option A
        glfw
        # option B
        # ${X11_X11_LIB}
        # ${CMAKE_THREAD_LIBS_INIT}

        ${GLAD_LIB}
        ${STB_LIB}
        ${CMAKE_DL_LIBS}

        ${Boost_LIBRARIES}
        stdc++fs
    )

# --------------------------------------------------------------



# Create an executable
add_executable(
        polyscopedemo
        src/demo_app.cpp
        )

# Link settings
target_link_libraries(polyscopedemo ${LIBS} )

# --------------------- Visualizer FILES -----------------------

SET(SRCS src/object_3d.cpp
        src/window_ui.cpp
        src/utils.cpp
        src/filesystem.cpp
        src/interpolations.cpp
        src/mesh_io.cpp
        src/L2DFileDialog.cpp
        )

# --------------------------------------------------------------


# ---------------------- Visualizer ----------------------------

# Create an executable
add_executable(visualizer src/main.cpp ${SRCS} )

# Link settings
target_link_libraries(visualizer ${LIBS} )

# --------------------------------------------------------------

