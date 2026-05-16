# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/gh_update_checker-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${gh-update-checker_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${gh_update_checker_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET gh_update_checker::gh_update_checker)
    add_library(gh_update_checker::gh_update_checker INTERFACE IMPORTED)
    message(${gh_update_checker_MESSAGE_MODE} "Conan: Target declared 'gh_update_checker::gh_update_checker'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/gh_update_checker-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()