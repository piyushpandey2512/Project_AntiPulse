# cmake requirement 
cmake_minimum_required(VERSION 3.16.1 FATAL_ERROR)

# Name of the project
project(Project_AntiPulse)

# Find required packages: Geant4 (UI and visualization)
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Gather source and header files
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

# Import macro files into build directory
file(GLOB My_MACROS "*.mac")
file(COPY ${My_MACROS} DESTINATION ${PROJECT_BINARY_DIR})

# Add CADMesh include and library directories
set(CADMESH_INCLUDE_DIR "/home/piyush/Desktop/PhD_Work/Trento_Project/CADMesh/CADMesh-1.1/include")
set(CADMESH_LIB_DIR "/home/piyush/Desktop/PhD_Work/Trento_Project/CADMesh/CADMesh-1.1/build")

include_directories(${CADMESH_INCLUDE_DIR})
link_directories(${CADMESH_LIB_DIR})

# Add executable
add_executable(AntiPulse Project_AntiPulse.cc ${sources} ${headers})

# Link to Geant4 and CADMesh
target_link_libraries(AntiPulse ${Geant4_LIBRARIES} cadmesh)

# Optional: Create custom target
add_custom_target(Project_AntiPulse DEPENDS AntiPulse)

