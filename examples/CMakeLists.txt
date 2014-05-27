cmake_minimum_required(VERSION 2.8)

project ( vtkPowerCrustSurfaceReconstruction )

#add_definitions ( "-Wall -Wextra " )

find_package ( VTK REQUIRED )
include ( ${VTK_USE_FILE} )

add_executable ( vtkPowerCrustSurfaceReconstructionExample MACOSX_BUNDLE vtkPowerCrustSurfaceReconstructionExample.cxx ../vtkPowerCrustSurfaceReconstruction.cxx )
target_link_libraries ( vtkPowerCrustSurfaceReconstructionExample ${VTK_LIBRARIES} )


add_subdirectory ( data )
