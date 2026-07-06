cmake_minimum_required(VERSION 3.16)
project(sitl_golden LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Optionen -------------------------------------------------------------
# QUAT_IMPL / SAFETY_IMPL:  ref     -> mitgelieferte Referenz-Ports (kein MATLAB)
#                           codegen -> von MATLAB Coder generierte Quellen
set(QUAT_IMPL   "ref" CACHE STRING "ref | codegen")
set(SAFETY_IMPL "ref" CACHE STRING "ref | codegen")
# Wurzel der Codegen-Ausgabe (siehe matlab/gen_lib_codegen.m). Nur bei *_IMPL=codegen.
set(CODEGEN_ROOT "${CMAKE_SOURCE_DIR}/codegen" CACHE PATH "MATLAB Coder output root")
# Golden-vs-Impl Toleranz. Default 1e-9 (= MATLAB-vs-Golden). Fuer den Codegen-
# Diff eng lassen; ULP-Divergenz soll auffallen, nicht kaschiert werden.
set(GOLDEN_TOL "1e-9" CACHE STRING "abs tolerance golden vs impl")

# Wo die Golden-CSVs liegen. Default: mitgelieferter data/-Ordner (kein Flag noetig).
# Auf deinen eigenen Ordner zeigen mit  -DGOLDEN_DIR=../test
set(GOLDEN_DIR "${CMAKE_SOURCE_DIR}/data" CACHE PATH "Ordner mit test_data_quat*.csv")

# --- GoogleTest via FetchContent -----------------------------------------
include(FetchContent)
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)

# --- Implementierungs-Quellen waehlen ------------------------------------
if(QUAT_IMPL STREQUAL "ref")
  set(QUAT_SRC src/quat_helpers_ref.cpp)
else()
  # Beispiel-Pfade; an gen_lib_codegen.m anpassen. Header muessen die
  # extern-"C"-Signaturen aus include/quat_helpers.h erfuellen (Shim ggf. noetig).
  file(GLOB QUAT_SRC
    "${CODEGEN_ROOT}/lib/dcm2quat_local/*.c*"
    "${CODEGEN_ROOT}/lib/quat2dcm_local/*.c*"
    "${CODEGEN_ROOT}/lib/quatMul/*.c*"
    "${CODEGEN_ROOT}/lib/quatConj/*.c*"
    "${CODEGEN_ROOT}/lib/quatRotate/*.c*")
endif()

if(SAFETY_IMPL STREQUAL "ref")
  set(SAFETY_SRC src/safety_helpers_ref.cpp)
else()
  file(GLOB SAFETY_SRC
    "${CODEGEN_ROOT}/lib/safety_overspeed/*.c*"
    "${CODEGEN_ROOT}/lib/safety_battery/*.c*")
endif()

# --- Test-Targets ---------------------------------------------------------
add_executable(test_quat_golden test/test_quat_golden.cpp ${QUAT_SRC})
target_include_directories(test_quat_golden PRIVATE include test)
target_compile_definitions(test_quat_golden PRIVATE
  GOLDEN_DIR="${GOLDEN_DIR}" GOLDEN_TOL=${GOLDEN_TOL})
target_link_libraries(test_quat_golden PRIVATE GTest::gtest_main)
gtest_discover_tests(test_quat_golden)

add_executable(test_safety test/test_safety.cpp ${SAFETY_SRC})
target_include_directories(test_safety PRIVATE include test)
target_link_libraries(test_safety PRIVATE GTest::gtest_main)
gtest_discover_tests(test_safety)
