
add_library(${MAIN_PROJECT_NAME}
            properties.cpp
            ../include/cxxprop/properties.hpp)

add_library(${MAIN_PROJECT_NAME}::${MAIN_PROJECT_NAME} ALIAS ${MAIN_PROJECT_NAME})

target_compile_features(${MAIN_PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${MAIN_PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${PROJECT_BIN_DIR}>/include
    $<INSTALL_INTERFACE:include>)

source_group(
  TREE "$../include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

