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

* CMake 2.8.10 or higher
** Get CMake from http://cmake.org/cmake/resources/software.html

Data
====
To get hold of data, plesae contact Kitware Inc.

Run application
===============
Application expects that data directory is placed relative to the
directory of the application. Such as below:
.
|
|_ data
|
|_ install/vtkkiva

You can provide your own data lookup path by providing the path using '-d' option

vtkkiva -d "your path here"





