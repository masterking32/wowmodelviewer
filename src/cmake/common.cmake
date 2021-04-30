###################
#   common part   #
###################

# set build type you want
# available values are None Debug Release RelWithDebInfo MinSizeRel (will strip exe for minimal output size)
SET(CMAKE_BUILD_TYPE RelWithDebInfo)

# define base repo path to use it cross folder
SET(WMV_BASE_PATH ${CMAKE_CURRENT_LIST_DIR}/../..)

# define cmake folder to be reusable cross scripts
SET(WMV_CMAKE_FOLDER ${WMV_BASE_PATH}/src/cmake)

# add wmv cmake directory to search path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${WMV_CMAKE_FOLDER}")

# define policies to avoid warnings
include(${WMV_CMAKE_FOLDER}/policies.cmake)

# clean up a bit weird backslashes in sdk path
string (REPLACE "\\" "/" WMV_SDK_BASEDIR $ENV{WMV_SDK_BASEDIR})
SET(ENV{WMV_SDK_BASEDIR} ${WMV_SDK_BASEDIR})

# Qt5 stuff
# init cmake with our qt install directory
set(CMAKE_PREFIX_PATH $ENV{WMV_SDK_BASEDIR}/Qt/lib/cmake)

#############################
#  platform specific part   #
#############################
if(WIN32)
  include(${WMV_BASE_PATH}/src/cmake/windows.cmake)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
  include(${WMV_BASE_PATH}/src/cmake/linux.cmake)
elseif(APPLE)
  include(${WMV_BASE_PATH}/src/cmake/macos.cmake)    
endif()

######################################
# macro to be reused across projects #
######################################
macro(use_glew)
  include_directories(${WMV_BASE_PATH}/src/3rdparty)
  add_definitions(-DGLEW_STATIC)
  list(APPEND extralibs opengl32 ${WMV_BASE_PATH}/src/3rdparty/libs/glew32s.lib)
endmacro()

macro(use_cximage)
  include_directories(${WMV_BASE_PATH}/src/3rdparty/CxImage)
  list(APPEND extralibs cximage)
endmacro()

macro(use_wow)
  use_core() # if you use wow lib, you are underneath using core lib
  use_casclib() # if you use wow lib, you are underneath using casc lib 
  include_directories(${WMV_BASE_PATH}/src/games/wow)
  link_directories(${WMV_SDK_BASEDIR}/Qt/lib)
  find_package(Qt5Core)
  find_package(Qt5Xml)
  find_package(Qt5Gui)
  find_package(Qt5Network)
  list(APPEND extralibs wow)
endmacro()

macro(use_log)
  include_directories(${WMV_BASE_PATH}/src/log)
  list(APPEND extralibs log)
endmacro()

macro(use_core)
  use_log() # if you use core lib, you are underneath using log lib
  include_directories(${WMV_BASE_PATH}/src/core)
  link_directories(${WMV_SDK_BASEDIR}/Qt/lib)
  find_package(Qt5Core) 
  find_package(Qt5Gui) # Qt5Gui is needed for QImage
  find_package(Qt5Network)
  list(APPEND extralibs core)
endmacro()
  
macro(use_casclib)
  include_directories(${CASCLIB_INSTALL_LOCATION}/include)
  link_directories(${CASCLIB_INSTALL_LOCATION}/lib)
  list(APPEND extralibs casc)
endmacro()

macro(use_sqlite)
  list(APPEND src sqlite3.c)
endmacro()

macro(setup_wmv_plugin)
  #add_definitions(-DBUILDING_PLUGIN)
  set_property(TARGET ${NAME} PROPERTY FOLDER "plugins")
  set(BIN_DIR "${WMV_BASE_PATH}/bin/plugins/")
  if(WIN32)
    install(TARGETS ${NAME} RUNTIME DESTINATION ${BIN_DIR})
  endif(WIN32)
endmacro()
