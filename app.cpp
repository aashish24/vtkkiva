/*=========================================================================

  Program:   Visualization Toolkit
  Module:    app.cpp

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "app.h"

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkColorSeries.h>
#include <vtkCommand.h>
#include <vtkCompositePolyDataMapper.h>
#include <vtkDiskSource.h>
#include <vtkDelimitedTextReader.h>
#include <vtkDistanceToCamera.h>
#include <vtkDoubleArray.h>
#include <vtkGDALVectorReader.h>
#include <vtkGeoSphereTransform.h>
#include <vtkGeoMath.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkLegendBoxActor.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPNGReader.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkStringToCategory.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>

#if 0
  #include <vtkPolyDataWriter.h>
#endif

void func(vtkObject*, unsigned long eid, void* clientdata, void* calldata);

//////////////////////////////////////////////////////////////////////////////
void ResetCamera(vtkCamera* camera)
{
  assert(camera != NULL);

  // Get current position
  camera->SetFocalPoint(0.0, 0.0, 0.0);
  camera->SetPosition(0.0, vtkGeoMath::EarthRadiusMeters() * 8.0, 0.0);
  camera->SetViewUp(0.0, 0.0, 1.0);
}

//////////////////////////////////////////////////////////////////////////////
void ResetCameraCommand (vtkObject* caller, long unsigned int eventId,
                         void* clientData, void* callData)
{
  vtkRenderer* ren =
      static_cast<vtkRenderer*>(caller);

  double* bounds = static_cast<double*>(clientData);
  ResetCamera(ren->GetActiveCamera());

  ren->ResetCameraClippingRange();
  ren->GetRenderWindow()->Render();
}

//////////////////////////////////////////////////////////////////////////////
App::App()
{
}

//////////////////////////////////////////////////////////////////////////////
App::~App()
{
  // Do nothing
}

//////////////////////////////////////////////////////////////////////////////
void App::init()
{
  this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->RenWindow = vtkSmartPointer<vtkRenderWindow>::New();
  this->Interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  this->ResetCameraCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  this->LegendActor = vtkSmartPointer<vtkLegendBoxActor>::New();

  this->RenWindow->AddRenderer(this->Renderer);
  this->Interactor->SetRenderWindow(this->RenWindow);
  this->Renderer->AddObserver(vtkCommand::ResetCameraEvent,
                              this->ResetCameraCallback);

  // Do not draw globe for now
  this->Globe = this->createGlobe();
  this->Renderer->AddActor(this->Globe);
  this->Renderer->AddActor(this->LegendActor);
  this->Renderer->SetBackground(0.0, 0.0, 0.0);
  this->loadAndDrawCountryBorders();
  this->loadAndDrawLoans();
  this->loasAndDrawLenders();
}

//////////////////////////////////////////////////////////////////////////////
void App::start()
{
  this->Interactor->Initialize();

  // Set the style
  vtkSmartPointer<vtkInteractorStyleSwitch> InteractorStyleSwitch =
    vtkInteractorStyleSwitch::SafeDownCast(this->Interactor->GetInteractorStyle());
  InteractorStyleSwitch->SetCurrentStyleToTrackballCamera();

  // Set the camera
  ResetCamera(this->Renderer->GetActiveCamera());
  this->Renderer->ResetCameraClippingRange();
  this->ResetCameraCallback->SetCallback(ResetCameraCommand);

  this->RenWindow->Render();
  this->Interactor->Start();
}

//////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkActor> App::createGlobe()
{
  // Create the geometry
  vtkNew<vtkTexturedSphereSource> source;
  source->SetRadius(vtkGeoMath::EarthRadiusMeters());
  source->SetThetaResolution(100);
  source->SetPhiResolution(100);

  // Read the texture
  // Skip textures for now since they have issues
#if 0
  vtkNew<vtkPNGReader> pngReader;
  pngReader->SetFileName("../data/land_shallow_topo_2048.png");

  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(pngReader->GetOutputPort());
#endif

  // Put it all together
  vtkNew<vtkActor> globeActor;
  vtkNew<vtkPolyDataMapper> globeMapper;

  globeMapper->SetInputConnection(source->GetOutputPort());
  globeActor->SetMapper(globeMapper.GetPointer());

#if 0 // See above
  globeActor->SetTexture(texture.GetPointer());
#endif

  globeActor->GetProperty()->SetColor(0.6, 0.6, 0.6);
  return globeActor.GetPointer();
}

//////////////////////////////////////////////////////////////////////////////
void App::loadAndDrawCountryBorders()
{
  vtkNew<vtkGDALVectorReader> reader;
  reader->SetFileName("../data/countries.shp");
  reader->Update();

  // Get the data
  vtkNew<vtkGeoSphereTransform> transform;
  vtkSmartPointer<vtkMultiBlockDataSet> mbds = reader->GetOutput();

  // Put it toghether
  vtkNew<vtkMultiBlockDataSet> newMbds;

  unsigned int numberOfBlocks = mbds->GetNumberOfBlocks();
  for (unsigned int i = 0; i < numberOfBlocks; ++i)
  {
    vtkSmartPointer<vtkPolyData> polyData =
      vtkPolyData::SafeDownCast(mbds->GetBlock(i));

    vtkNew<vtkPoints> newPoints;
    vtkNew<vtkPolyData> line;

    transform->TransformPoints(polyData->GetPoints(), newPoints.GetPointer());
    line->ShallowCopy(polyData);
    line->SetPoints(newPoints.GetPointer());
    newMbds->SetBlock(i, line.GetPointer());
  }

  vtkNew<vtkActor> actor;
  vtkNew<vtkCompositePolyDataMapper> mapper;
  mapper->SetInputDataObject(newMbds.GetPointer());
  mapper->SetResolveCoincidentTopologyZShift(0.001);
  mapper->SetResolveCoincidentTopologyToShiftZBuffer();

#if 0
  // Create a decent look up table
  vtkNew<vtkLookupTable> lut;
  lut->SetTableRange(1.0, 8.0);
  lut->SetValueRange(0.8, 1.0);
  lut->SetHueRange(0.9, 0.9);
  lut->SetSaturationRange(0.0, 0.7);
  lut->SetNumberOfColors(8);
  lut->Build();
  mapper->SelectColorArray("mapcolor8");
  mapper->SetScalarModeToUseCellFieldData();
  mapper->SetScalarVisibility(1);
  mapper->UseLookupTableScalarRangeOn();
  mapper->SetLookupTable(lut.GetPointer());
  mapper->SetColorModeToMapScalars();
#endif

  actor->SetMapper(mapper.GetPointer());
  actor->GetProperty()->SetBackfaceCulling(1);
  actor->GetProperty()->SetLineWidth(2.0);
  actor->GetProperty()->SetColor(0.8, 0.8, 0.8);
  this->Renderer->AddActor(actor.GetPointer());
}

//////////////////////////////////////////////////////////////////////////////
void App::loadAndDrawLoans()
{
  vtkNew<vtkPolyData> loansPolyData;
  vtkNew<vtkPoints> loansPoints;
  vtkNew<vtkPoints> loansGeoPoints;
  vtkNew<vtkCellArray> loansVerts;

  vtkNew<vtkDelimitedTextReader> csvReader;
  csvReader->SetFileName("../data/loans.csv");

  vtkNew<vtkStringToCategory> stringToCat;
  stringToCat->SetInputConnection(csvReader->GetOutputPort());
  stringToCat->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_ROWS, "Field 3");
  stringToCat->Update();

  // Data
  vtkSmartPointer<vtkTable> output =
    vtkTable::SafeDownCast(stringToCat->GetOutputDataObject(0));
  vtkIdType noOfRows = output->GetNumberOfRows();

  vtkSmartPointer<vtkAbstractArray> latArray = output->GetColumn(0);
  vtkSmartPointer<vtkAbstractArray> lonArray = output->GetColumn(1);

  // Categories
  vtkSmartPointer<vtkTable> stringTable = vtkTable::SafeDownCast(
    stringToCat->GetOutputDataObject(1));
  vtkSmartPointer<vtkStringArray> categories =
      vtkStringArray::SafeDownCast(stringTable->GetColumnByName("Strings"));

  // Additional data
  vtkSmartPointer<vtkAbstractArray> loanAmounts = output->GetColumn(2);
  loanAmounts->SetName("LoanAmounts");
  vtkSmartPointer<vtkIntArray> loanCategory =
    vtkIntArray::SafeDownCast(output->GetColumnByName("category"));
  loanCategory->SetName("LoanCategory");

  for (vtkIdType i = 0; i < noOfRows; ++i)
  {
    loansPoints->InsertNextPoint(
      lonArray->GetVariantValue(i).ToDouble(),
      latArray->GetVariantValue(i).ToDouble(),
      loanAmounts->GetVariantValue(i).ToDouble() * 1000.0);
    loansVerts->InsertNextCell(1);
    loansVerts->InsertCellPoint(i);
  }
  loansPolyData->GetPointData()->AddArray(loanAmounts);
  loansPolyData->GetPointData()->SetScalars(loanCategory);

  vtkNew<vtkGeoSphereTransform> transform;
  transform->TransformPoints(loansPoints.GetPointer(),
                             loansGeoPoints.GetPointer());
  loansPolyData->SetPoints(loansGeoPoints.GetPointer());
  loansPolyData->SetVerts(loansVerts.GetPointer());

#if 0
  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(loansPolyData.GetPointer());
  writer->SetFileName("loans.vtk");
  writer->Write();
#endif

  vtkNew<vtkActor> loansActor;

#if 1 // Draw GL points
  vtkNew<vtkPolyDataMapper> loansMapper;
  loansMapper->SetInputDataObject(loansPolyData.GetPointer());

  double loanTypes[2];
  loanCategory->GetRange(loanTypes);
  vtkNew<vtkLookupTable> loansLookupTable;
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorSchemeByName("Brewer Qualitative Set3");
  colorSeries->BuildLookupTable(loansLookupTable.GetPointer());

  vtkIdType noOfLoanCategories = stringTable->GetNumberOfRows();
  for (vtkIdType i = 0; i < noOfLoanCategories; ++i)
  {
    loansLookupTable->SetAnnotation(i, categories->GetValue(i));
  }
  loansLookupTable->Build();
  loansMapper->SetLookupTable(loansLookupTable.GetPointer());
  loansMapper->SetColorModeToMapScalars();
  loansMapper->SetScalarVisibility(1);

  loansActor->SetMapper(loansMapper.GetPointer());
  loansActor->GetProperty()->SetPointSize(10);
#else // Draw glyphs
  vtkNew<vtkSphereSource> sphere;
  sphere->SetRadius(100000.0);
  sphere->SetThetaResolution(4.0);
  sphere->SetThetaResolution(4.0);

  vtkNew<vtkDistanceToCamera> distanceToCamera;
  distanceToCamera->SetInputDataObject(loansPolyData.GetPointer());
  distanceToCamera->SetScreenSize(10.0);
  distanceToCamera->SetRenderer(this->Renderer);

  vtkNew<vtkGlyph3DMapper> loansMapper;
  loansMapper->SetInputDataObject(loansPolyData.GetPointer());
  loansMapper->SetSourceConnection(sphere->GetOutputPort());
  loansMapper->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");

  loansActor->SetMapper(loansMapper.GetPointer());
  loansActor->GetProperty()->SetBackfaceCulling(1);
#endif

  loansActor->GetProperty()->SetOpacity(0.02);
  this->Renderer->AddActor(loansActor.GetPointer());
  this->RenWindow->SetPointSmoothing(1);

  double color[3];
  this->LegendActor->SetNumberOfEntries(noOfLoanCategories);
  for (vtkIdType i = 0; i < noOfLoanCategories; ++i)
  {
    vtkNew<vtkSphereSource> geom;
    geom->Update();
    loansLookupTable->GetColor(i, color);
    this->LegendActor->SetEntry(i, geom->GetOutput(),
                                categories->GetValue(i), color);
  }
}

//////////////////////////////////////////////////////////////////////////////
void App::loasAndDrawLenders()
{
  vtkNew<vtkPolyData> lendersPolyData;
  vtkNew<vtkPoints> lendersPoints;
  vtkNew<vtkPoints> lendersGeoPoints;
  vtkNew<vtkCellArray> lendersVerts;

  vtkNew<vtkDelimitedTextReader> csvReader;
  csvReader->SetFileName("../data/lenders.csv");
  csvReader->Update();

  vtkSmartPointer<vtkTable> table =
    vtkTable::SafeDownCast(csvReader->GetOutputDataObject(0));

  vtkIdType noOfRows = table->GetNumberOfRows();
  vtkSmartPointer<vtkAbstractArray> latArray = table->GetColumn(0);
  vtkSmartPointer<vtkAbstractArray> lonArray = table->GetColumn(1);

  // Additional data
  vtkSmartPointer<vtkAbstractArray> lendersDataArray = table->GetColumn(2);
  lendersDataArray->SetName("Foo");

  for (vtkIdType i = 0; i < noOfRows; ++i)
  {
    lendersPoints->InsertNextPoint(
      lonArray->GetVariantValue(i).ToDouble(),
      latArray->GetVariantValue(i).ToDouble(),
      lendersDataArray->GetVariantValue(i).ToDouble() * 5000.0);
    lendersVerts->InsertNextCell(1);
    lendersVerts->InsertCellPoint(i);
  }
  lendersPolyData->GetPointData()->AddArray(lendersDataArray);

  vtkNew<vtkGeoSphereTransform> transform;
  transform->TransformPoints(lendersPoints.GetPointer(),
                             lendersGeoPoints.GetPointer());
  lendersPolyData->SetPoints(lendersGeoPoints.GetPointer());
  lendersPolyData->SetVerts(lendersVerts.GetPointer());

#if 0
  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(lendersPolyData.GetPointer());
  writer->SetFileName("lenders.vtk");
  writer->Write();
#endif

  vtkNew<vtkActor> lendersActor;

#if 1 // Draw GL points
  vtkNew<vtkPolyDataMapper> lendersMapper;
  lendersMapper->SetInputDataObject(lendersPolyData.GetPointer());
  lendersActor->SetMapper(lendersMapper.GetPointer());
  lendersActor->GetProperty()->SetPointSize(10);
  lendersActor->GetProperty()->SetColor(0.9, 0.4, 0.4);
  lendersActor->GetProperty()->SetOpacity(0.02);

#else // Draw glyphs
  vtkNew<vtkSphereSource> sphere;
  sphere->SetRadius(100000.0);
  sphere->SetThetaResolution(4.0);
  sphere->SetThetaResolution(4.0);

  vtkNew<vtkDistanceToCamera> distanceToCamera;
  distanceToCamera->SetInputDataObject(lendersPolyData.GetPointer());
  distanceToCamera->SetScreenSize(10.0);
  distanceToCamera->SetRenderer(this->Renderer);

  vtkNew<vtkGlyph3DMapper> lendersMapper;
  lendersMapper->SetInputDataObject(lendersPolyData.GetPointer());
  lendersMapper->SetSourceConnection(sphere->GetOutputPort());
  lendersMapper->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");

  lendersActor->SetMapper(lendersMapper.GetPointer());
  lendersActor->GetProperty()->SetBackfaceCulling(1);
#endif

  lendersActor->GetProperty()->SetOpacity(0.02);
  this->Renderer->AddActor(lendersActor.GetPointer());
  this->RenWindow->SetPointSmoothing(1);
}
