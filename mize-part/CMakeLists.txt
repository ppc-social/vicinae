# =============================================================================
# mize-part — builds libmize_part_vicinae.so
#
#   cmake -S . -B build && cmake --build build
#
# Upstream tree under ../ is not modified. Wire vicinae/Qt/deps yourself
# (see comments at the bottom / chat summary).
# =============================================================================
cmake_minimum_required(VERSION 3.16)

set(MIZE_PART_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
get_filename_component(VICINAE_ROOT "${MIZE_PART_DIR}/.." ABSOLUTE)
get_filename_component(MIZE_CORE_INCLUDE "${VICINAE_ROOT}/../../mize/core/src/include" ABSOLUTE)
if (NOT EXISTS "${MIZE_CORE_INCLUDE}/mize.h")
  get_filename_component(MIZE_CORE_INCLUDE "${VICINAE_ROOT}/../mize/core/src/include" ABSOLUTE)
endif()

project(mize_part_vicinae LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message(STATUS "mize-part: VICINAE_ROOT=${VICINAE_ROOT}")
message(STATUS "mize-part: mize.h from ${MIZE_CORE_INCLUDE}")

# --- Qt (minimum for the skeleton; expand as server.cpp grows) ---
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2)

add_library(mize_part_vicinae SHARED
  server.cpp
  src/mize-part-vicinae.cpp
)

set_target_properties(mize_part_vicinae PROPERTIES
  OUTPUT_NAME "mize_part_vicinae"
  PREFIX "lib"
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(mize_part_vicinae PRIVATE
  ${MIZE_CORE_INCLUDE}
  ${MIZE_PART_DIR}
  ${VICINAE_ROOT}/src/server/src
  ${VICINAE_ROOT}/src/server
  # add more include roots as you pull in vicinae objects
)

target_link_libraries(mize_part_vicinae PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Quick
  Qt6::Qml
  Qt6::QuickControls2
  # TODO: link vicinae server objects / static libs / Qt extras here
)

if (NOT WIN32 AND NOT APPLE)
  target_compile_options(mize_part_vicinae PRIVATE
    -fvisibility=hidden
    -fvisibility-inlines-hidden
  )
  target_link_options(mize_part_vicinae PRIVATE
    -Wl,--version-script=${MIZE_PART_DIR}/mize-part.map
    -Wl,--allow-shlib-undefined
  )
endif()

# mize C API symbols (opt, method, thread, set_config, …) come from libmize.so
# at load time — do not link libmize here.
