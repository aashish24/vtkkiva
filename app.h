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

  void init();
  void start();

private:
  vtkSmartPointer<vtkActor> createGlobe();
  void loadAndDrawCountryBorders();
  void loadAndDrawLoans();
  void loasAndDrawLenders();

  vtkSmartPointer<vtkActor> Globe;
  vtkSmartPointer<vtkRenderer> Renderer;
  vtkSmartPointer<vtkRenderWindow> RenWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> Interactor;
  vtkSmartPointer<vtkCallbackCommand> ResetCameraCallback;
  vtkSmartPointer<vtkLegendBoxActor> LegendActor;
};

#endif // APP_H
