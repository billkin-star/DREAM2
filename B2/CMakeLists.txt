#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.27)
project(B2)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# root
# Find ROOT package
option(WITH_ROOT "Build example with ROOT data analysis support" ON)
if(WITH_ROOT)
  find_package(ROOT REQUIRED COMPONENTS RIO Tree)
  if(ROOT_FOUND)
    # 加载ROOT的编译配置（设置头文件目录、编译选项等）
    include(${ROOT_USE_FILE})
    # 添加ROOT头文件目录（让编译器找到TTree.h等文件）
    include_directories(${ROOT_INCLUDE_DIRS})
  endif()
else()
  # 禁用ROOT时执行的代码块
  message(STATUS "ROOT support is disabled (WITH_ROOT=OFF).")
endif()

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
# 创建可执行文件B2并链接Geant4库root库

add_executable(B2 main.cc ${sources} ${headers})
target_link_libraries(B2 ${Geant4_LIBRARIES})
if(WITH_ROOT AND ROOT_FOUND)
  target_link_libraries(B2 ${ROOT_LIBRARIES})
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B2. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(B2_SCRIPTS
  # exampleB2.in
  # exampleB2.out
  init_vis.mac
  run_all.mac
  vis.mac
  )

foreach(_script ${B2_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(B2Target DEPENDS B2)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS B2 DESTINATION bin)
