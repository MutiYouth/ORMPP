#######################################################
## Build Tree Export

# Export Targets
export(TARGETS ${ormpp_components_list} FILE ORMPPTargets.cmake)
export(PACKAGE ORMPP)

# Version information
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

# Build tree config
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake @ONLY IMMEDIATE)


#######################################################
## Install headers / targets

# This relative path allows installed files to be relocatable.
set(ORMPP_CMAKE_CONFIG_INSTALL_DIR lib/cmake/${PROJECT_NAME})
file(RELATIVE_PATH REL_INCLUDE_DIR
        "${CMAKE_INSTALL_PREFIX}/${ORMPP_CMAKE_CONFIG_INSTALL_DIR}"
        "${CMAKE_INSTALL_PREFIX}/include"
        )

# todo: check this install. 23.1.25
install(
        TARGETS ${ormpp_components_list}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)


install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/
        DESTINATION include 
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${ORMPP_CMAKE_CONFIG_INSTALL_DIR}
)
install(
        EXPORT ${PROJECT_NAME}Targets DESTINATION ${ORMPP_CMAKE_CONFIG_INSTALL_DIR}
)

# uninstall target
if (NOT TARGET uninstall)
    configure_file(
            "${CMAKE_CURRENT_LIST_DIR}/cmake_uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif ()