# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(gh-update-checker_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(gh-update-checker_FRAMEWORKS_FOUND_RELEASE "${gh-update-checker_FRAMEWORKS_RELEASE}" "${gh-update-checker_FRAMEWORK_DIRS_RELEASE}")

set(gh-update-checker_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET gh-update-checker_DEPS_TARGET)
    add_library(gh-update-checker_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET gh-update-checker_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${gh-update-checker_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${gh-update-checker_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:nlohmann_json::nlohmann_json;httplib::httplib>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### gh-update-checker_DEPS_TARGET to all of them
conan_package_library_targets("${gh-update-checker_LIBS_RELEASE}"    # libraries
                              "${gh-update-checker_LIB_DIRS_RELEASE}" # package_libdir
                              "${gh-update-checker_BIN_DIRS_RELEASE}" # package_bindir
                              "${gh-update-checker_LIBRARY_TYPE_RELEASE}"
                              "${gh-update-checker_IS_HOST_WINDOWS_RELEASE}"
                              gh-update-checker_DEPS_TARGET
                              gh-update-checker_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "gh-update-checker"    # package_name
                              "${gh-update-checker_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${gh-update-checker_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET gh_update_checker::gh_update_checker
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${gh-update-checker_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${gh-update-checker_LIBRARIES_TARGETS}>
                 )

    if("${gh-update-checker_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET gh_update_checker::gh_update_checker
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     gh-update-checker_DEPS_TARGET)
    endif()

    set_property(TARGET gh_update_checker::gh_update_checker
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${gh-update-checker_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET gh_update_checker::gh_update_checker
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${gh-update-checker_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET gh_update_checker::gh_update_checker
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${gh-update-checker_LIB_DIRS_RELEASE}>)
    set_property(TARGET gh_update_checker::gh_update_checker
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${gh-update-checker_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET gh_update_checker::gh_update_checker
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${gh-update-checker_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(gh-update-checker_LIBRARIES_RELEASE gh_update_checker::gh_update_checker)
