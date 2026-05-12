# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\cogent-cli_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\cogent-cli_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\cogent-gui_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\cogent-gui_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\cogent_core_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\cogent_core_autogen.dir\\ParseCache.txt"
  "cogent-cli_autogen"
  "cogent-gui_autogen"
  "cogent_core_autogen"
  )
endif()
