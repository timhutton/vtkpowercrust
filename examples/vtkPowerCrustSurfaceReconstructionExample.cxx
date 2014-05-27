/*=========================================================================

    An example for vtkPowerCrustSurfaceReconstruction algorithm.
    Copyright (C) 2014  Arash Akbarinia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/

#include "../vtkPowerCrustSurfaceReconstruction.h"

#include "vtkPLYReader.h"
#include "vtkPLYWriter.h"
#include "vtkSmartPointer.h"

#include <sstream>

int main ( int argc, char** argv )
{
  if ( argc < 2 )
    {
      std::cerr << argv[0] << " usage: InputFile1.ply InputFile2.ply InputFileN.ply" << std::endl;
      std::cerr << "There are a few sample PLY files available in the data folder." << std::endl;
      return 1;
    }

  for ( int i = 1; i < argc; i++ )
    {
      std::string InputFilename = argv[i];

      std::cout << "Reading: " << argv[i] << std::endl;
      vtkSmartPointer<vtkPLYReader> PlyReader = vtkSmartPointer<vtkPLYReader>::New();
      PlyReader->SetFileName ( InputFilename.c_str() );
      PlyReader->Update();
      std::cout << "Number of points " << PlyReader->GetOutput()->GetPoints()->GetNumberOfPoints() << std::endl;

      vtkSmartPointer<vtkPowerCrustSurfaceReconstruction> SurfaceReconstructor = vtkSmartPointer<vtkPowerCrustSurfaceReconstruction>::New();
      SurfaceReconstructor->SetInput ( PlyReader->GetOutput() );
      SurfaceReconstructor->Update();

      std::string OutFileName = "Output_";
      std::ostringstream convert;
      convert << i;
      OutFileName.append ( convert.str() );
      OutFileName.append ( ".ply" );
      std::cout << "Writing: " << OutFileName << std::endl;

      vtkSmartPointer<vtkPLYWriter> PlyWriter = vtkSmartPointer<vtkPLYWriter>::New();
      PlyWriter->SetFileName ( OutFileName.c_str() );
      PlyWriter->SetInput ( SurfaceReconstructor->GetOutput() );
      PlyWriter->Write();
    }

  return 0;
}

