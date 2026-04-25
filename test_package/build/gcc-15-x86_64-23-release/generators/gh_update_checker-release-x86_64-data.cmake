########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(gh-update-checker_COMPONENT_NAMES "")
if(DEFINED gh-update-checker_FIND_DEPENDENCY_NAMES)
  list(APPEND gh-update-checker_FIND_DEPENDENCY_NAMES nlohmann_json httplib)
  list(REMOVE_DUPLICATES gh-update-checker_FIND_DEPENDENCY_NAMES)
else()
  set(gh-update-checker_FIND_DEPENDENCY_NAMES nlohmann_json httplib)
endif()
set(nlohmann_json_FIND_MODE "NO_MODULE")
set(httplib_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(gh-update-checker_PACKAGE_FOLDER_RELEASE "/home/zb_bamboo/.conan2/p/b/gh-up1db8acfaff592/p")
set(gh-update-checker_BUILD_MODULES_PATHS_RELEASE )


set(gh-update-checker_INCLUDE_DIRS_RELEASE "${gh-update-checker_PACKAGE_FOLDER_RELEASE}/include")
set(gh-update-checker_RES_DIRS_RELEASE )
set(gh-update-checker_DEFINITIONS_RELEASE )
set(gh-update-checker_SHARED_LINK_FLAGS_RELEASE )
set(gh-update-checker_EXE_LINK_FLAGS_RELEASE )
set(gh-update-checker_OBJECTS_RELEASE )
set(gh-update-checker_COMPILE_DEFINITIONS_RELEASE )
set(gh-update-checker_COMPILE_OPTIONS_C_RELEASE )
set(gh-update-checker_COMPILE_OPTIONS_CXX_RELEASE )
set(gh-update-checker_LIB_DIRS_RELEASE "${gh-update-checker_PACKAGE_FOLDER_RELEASE}/lib")
set(gh-update-checker_BIN_DIRS_RELEASE )
set(gh-update-checker_LIBRARY_TYPE_RELEASE UNKNOWN)
set(gh-update-checker_IS_HOST_WINDOWS_RELEASE 0)
set(gh-update-checker_LIBS_RELEASE )
set(gh-update-checker_SYSTEM_LIBS_RELEASE )
set(gh-update-checker_FRAMEWORK_DIRS_RELEASE )
set(gh-update-checker_FRAMEWORKS_RELEASE )
set(gh-update-checker_BUILD_DIRS_RELEASE )
set(gh-update-checker_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(gh-update-checker_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${gh-update-checker_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${gh-update-checker_COMPILE_OPTIONS_C_RELEASE}>")
set(gh-update-checker_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gh-update-checker_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gh-update-checker_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gh-update-checker_EXE_LINK_FLAGS_RELEASE}>")


set(gh-update-checker_COMPONENTS_RELEASE )