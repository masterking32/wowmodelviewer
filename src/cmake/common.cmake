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
  include_directories(${WMV_BASE_PATH}/src/glew/include)
  list(APPEND src ${WMV_BASE_PATH}/src/glew/src/glew.c)
  add_definitions(-DGLEW_STATIC)
  
  # temporary solution, glew needs opengl lib, and right now, wx one is used...
  set(wxWidgets_USE_UNICODE ON)
  find_package(wxWidgets REQUIRED gl core base)
  
  list(APPEND extralibs ${wxWidgets_LIBRARIES})
endmacro()

macro(use_wxwidgets)
	set(wxWidgets_USE_UNICODE ON)
	find_package(wxWidgets REQUIRED net gl aui xml adv core base)
	list(APPEND extralibs ${wxWidgets_LIBRARIES})
endmacro()

macro(use_cximage)
  include_directories(${WMV_BASE_PATH}/src/CxImage)
  list(APPEND extralibs ${wxWidgets_LIBRARIES} cximage)
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
endmacro()

macro(use_core)
  include_directories(${WMV_BASE_PATH}/src/core)
  link_directories(${WMV_SDK_BASEDIR}/Qt/lib)
  find_package(Qt5Core) 
  find_package(Qt5Gui) # Qt5Gui is needed for QImage
  find_package(Qt5Network)
endmacro()
  
macro(use_casclib)
  include_directories(${CASCLIB_INSTALL_LOCATION}/include)
  link_directories(${CASCLIB_INSTALL_LOCATION}/lib)
endmacro()

macro(use_sqlite)
  list(APPEND src sqlite3.c)
endmacro()