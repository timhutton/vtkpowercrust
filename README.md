vtkpowercrust
=============

A [VTK](http://vtk.org) port of the PowerCrust surface reconstruction algorithm

<img src="TestPowerCrust.png" />

(input points, output surface, output medial surface)

Usage:
------
Assuming your project is already using VTK, just add vtkPowerCrustSurfaceReconstruction.cxx and .h to your project.

History:
--------
* 2001: Paper and code by Nina Amenta, Sunghee Choi and Ravi Krishna Kolluri: [http://www.cs.utexas.edu/users/amenta/powercrust/welcome.html](https://web.archive.org/web/20090430025346/http://www.cs.utexas.edu/users/amenta/powercrust/welcome.html)
* 2002-06-28: Tim Hutton ports to VTK.
* 2003-03-31: Corrections for cross-platform compatibility (and VTK 4.2 support) from Bruce Lamond.
* 2003-12-15: An instance can now be used multiple times, some memory leaks corrected. These changes from Dieter Pfeffer and Oliver Moss at <a href="http://www.eprosoft.de">www.eprosoft.de</a>.
* 2014-04-28: Arash Akbarinia cleans up the code and fixes memory leaks.
* 2017-11-14: Bugfix from Alessandro Volz.
