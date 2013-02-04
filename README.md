vtkkiva
=======
Geoinformatics application built on top of VTK

License
=======
  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

Build instructions
=================
* Requires VTK 6 (from the VTK repository)
** Get VTK from git://vtk.org/VTK.git
** Get GDAL from www.gdal.org
** MAKE SURE TO turn ON Module_vtkIOGDAL (You can find this option in the advanced mode)

* CMake 2.8.10 or higher
** Get CMake from http://cmake.org/cmake/resources/software.html

Data
====
To get hold of the data, plesae contact Kitware Inc.

Run application
===============
Application expects that data directory is placed relative to the
directory of the application.  You can provide your own data lookup path 
by providing a path to the application using its '-d' option

> vtkkiva -d "your path here" (default ../Data"  -t value (Render every value ms; Default is 16ms)





