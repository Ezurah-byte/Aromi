# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Set the paths to extra components
set(EXTRA_COMPONENT_DIRS
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
    $ENV{IDF_PATH}/examples/common_components/led_strip
   # $ENV{IDF_PATH}/components/led_strip
)

# Include the project configuration
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi_prov_mgr)
