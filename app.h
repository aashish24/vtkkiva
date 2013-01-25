/*=========================================================================

  Program:   Visualization Toolkit
  Module:    app.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef APP_H
#define APP_H

#include <vtkSmartPointer.h>

#include <string>

class vtkActor;
class vtkCallbackCommand;
class vtkLegendBoxActor;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

class App
{
public:
  App();
  ~App();

  void init(const int& argc, char** argv);
  void start();

private:
  vtkSmartPointer<vtkActor> createGlobe();
  void loadAndDrawCountryBorders();
  void loadAndDrawLoans();
  void loasAndDrawLenders();

  std::string DataLookupPath;

  vtkSmartPointer<vtkActor> Globe;
  vtkSmartPointer<vtkRenderer> Renderer;
  vtkSmartPointer<vtkRenderWindow> RenWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> Interactor;
  vtkSmartPointer<vtkCallbackCommand> ResetCameraCallback;
  vtkSmartPointer<vtkLegendBoxActor> LegendActor;
};

#endif // APP_H
