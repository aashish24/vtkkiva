/*=========================================================================

  Program:   Visualization Toolkit
  Module:    main.cpp

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "app.h"

int main(int argc, char** argv)
{
  App app;
  app.init(argc, argv);
  app.start();
  return 0;
}
