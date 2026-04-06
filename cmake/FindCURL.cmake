# Try to locate curl-config
find_program(CURL_CONFIG_EXECUTABLE curl-config)

if(NOT CURL_CONFIG_EXECUTABLE)
    message(FATAL_ERROR "curl-config not found — cannot detect CURL")
endif()

# Query version
execute_process(
    COMMAND ${CURL_CONFIG_EXECUTABLE} --version
    OUTPUT_VARIABLE CURL_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Extract version number (e.g. "libcurl 7.81.0" → "7.81.0")
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CURL_VERSION "${CURL_VERSION_STRING}")

# Compare with requested version
if(CURL_FIND_VERSION)
    if(CURL_VERSION VERSION_LESS CURL_FIND_VERSION)
        message(FATAL_ERROR
            "Found CURL version ${CURL_VERSION}, "
            "but version ${CURL_FIND_VERSION} or newer is required."
        )
    endif()
endif()

# Query include and library dirs
execute_process(
    COMMAND ${CURL_CONFIG_EXECUTABLE} --cflags
    OUTPUT_VARIABLE CURL_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${CURL_CONFIG_EXECUTABLE} --libs
    OUTPUT_VARIABLE CURL_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Export variables for find_package()
set(CURL_INCLUDE_DIRS "")
set(CURL_LIBRARIES ${CURL_LIBS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    CURL
    REQUIRED_VARS CURL_LIBRARIES
    VERSION_VAR CURL_VERSION
)
