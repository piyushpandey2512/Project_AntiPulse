#cmake requirement 
cmake_minimum_required(VERSION 3.16.1 FATAL_ERROR)

#Name of the project
project(Project_AntiPulse)

#Mention packages required: Geant4, interface and visualization)
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Mention all source and header files in folder 
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

#For automatically import macro in build
file(GLOB My_MACROS "*.mac")

file(COPY ${My_MACROS} DESTINATION ${PROJECT_BINARY_DIR})
	

add_executable(AntiPulse Project_AntiPulse.cc ${sources} ${headers})

# Add CADMesh include directory
include_directories(/home/piyush/Desktop/PhD_Work/Trento_Project/CADMesh/CADMesh-1.1/build/include)

# Verify that the CADMesh library exists
set(CADMesh_LIB_PATH "/home/piyush/Desktop/PhD_Work/Trento_Project/CADMesh/CADMesh-1.1/build/libcadmesh.so")
if(NOT EXISTS ${CADMesh_LIB_PATH})
    message(FATAL_ERROR "CADMesh library not found at ${CADMesh_LIB_PATH}. Please build CADMesh before proceeding.")
endif()

# Link the target to the libraries of Geant4 and CADMesh
target_link_libraries(AntiPulse ${Geant4_LIBRARIES} ${CADMesh_LIB_PATH})

add_custom_target(Project_AntiPulse DEPENDS AntiPulse)
