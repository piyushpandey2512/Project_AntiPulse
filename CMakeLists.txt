cmake_minimum_required(VERSION 3.16.1 FATAL_ERROR)
project(Project_AntiPulse)

# Find Geant4
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Source files
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

# Macro files
file(GLOB My_MACROS "${PROJECT_SOURCE_DIR}/*.mac")
foreach(mac ${My_MACROS})
    configure_file(${mac} ${PROJECT_BINARY_DIR}/ COPYONLY)
endforeach()

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    /home/piyush/Desktop/PhD_Work/Trento_Project/CADMesh/CADMesh-1.1/include
    /usr/local/include
    /usr/include/assimp
)

# Library directories
link_directories(
    /home/piyush/Desktop/PhD_Work/Trento_Project/CADMesh/CADMesh-1.1/build
    /usr/local/lib
    /usr/lib/x86_64-linux-gnu
)

# Add executable
add_executable(AntiPulse Project_AntiPulse.cc GratingHit.cc ${sources} ${headers})

# Link libraries
target_link_libraries(AntiPulse
    ${Geant4_LIBRARIES}
    cadmesh
    assimp
    tet
)

# Optional custom target
add_custom_target(Project_AntiPulse DEPENDS AntiPulse)
