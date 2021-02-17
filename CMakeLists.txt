cmake_minimum_required(VERSION 3.1...3.19)


set(MAIN_PROJECT_NAME CxxProp)
set(MAIN_PROJECT_VERSION 1.0)
project(${MAIN_PROJECT_NAME}
        VERSION ${MAIN_PROJECT_VERSION}
        DESCRIPTION "${MAIN_PROJECT_NAME} - a simple library for creating a property manager."
        LANGUAGES CXX)


add_subdirectory(src) 

install(
    TARGETS CxxProp
    EXPORT CxxPropConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    EXPORT "CxxPropConfig" 
    FILE "CxxPropConfig.cmake" 
    NAMESPACE CxxProp::
    DESTINATION lib/cmake/CxxProp
)

export(
    TARGETS CxxProp
    NAMESPACE CxxProp::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/CxxPropConfig.cmake"
)

install(
    EXPORT CxxPropConfig
    DESTINATION "${CMAKE_INSTALL_DATADIR}/CxxProp/cmake"
    NAMESPACE CxxProp::
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${MAIN_PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${MAIN_PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${MAIN_PROJECT_NAME}Config.cmake" "${MAIN_PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "lib/cmake/${MAIN_PROJECT_NAME}"
) 

option(ADD_TESTS "Add tests" OFF)

if(${ADD_TESTS}  AND  ( CMAKE_PROJECT_NAME STREQUAL ${MAIN_PROJECT_NAME} )) 
    add_subdirectory(tests)
endif()

