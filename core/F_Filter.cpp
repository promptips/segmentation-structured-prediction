/////////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or       //
// modify it under the terms of the GNU General Public License         //
// version 2 as published by the Free Software Foundation.             //
//                                                                     //
// This program is distributed in the hope that it will be useful, but //
// WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU   //
// General Public License for more details.                            //
//                                                                     //
// Written and (C) by Aurelien Lucchi                                  //
// Contact <aurelien.lucchi@gmail.com> for comments & bug reports      //
/////////////////////////////////////////////////////////////////////////

#include "F_Filter.h"
#include "Config.h"
#include "utils_ITK.h"

#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkLaplacianRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkStructureTensorRecursiveGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkImageFileWriter.h"

//------------------------------------------------------------------------------

// features are computed for 3d cube
static const int nDim = 3;

// features:
// - gradient magnitude
// - LoG
// - eigenvalues of Hessian
// - eigenvalues of structure tensor
static const int numFeatures = 4;

// Use 7 scales
static const int numScales = 7;
static const double scales[numScales] = {1.0, 1.414, 2, 2.828, 4, 5.656, 8};

static const int channelCounts[numFeatures] = {1, 1, nDim, nDim};

#define OUTPUT_FILTER_IMAGES 0

//------------------------------------------------------------------------------

F_Filter::F_Filter(Slice_P& slice)
{
  features = 0;
  sizeFV = 0;
  for(int i = 0; i < numFeatures; i++) {
    sizeFV += channelCounts[i];
  }
  sizeFV *= numScales;
  precomputeFeatures(slice);
  //createSupernodeBasedFeatures(slice);
}


F_Filter::~F_Filter()
{
  /*
  if(features) {
    for(int i = 0; i < sizeFV; i++) {
      delete[] features[i];
    }
    delete[] features;
  }
  */
}

void F_Filter::precomputeFeatures(Slice_P& slice)
{
  typedef uchar TInputPixelType;
  typedef itk::Image< TInputPixelType, nDim > InputImageType;
  typedef itk::Image<float,nDim> FloatImageType;
  typedef itk::SymmetricSecondRankTensor<float,nDim> MatrixPixelType;
  typedef itk::Image<MatrixPixelType,nDim>  MatrixImageType;
  typedef itk::Vector<float,nDim> VectorPixelType;
  typedef itk::Image<VectorPixelType,nDim> VectorImageType;
  typedef itk::Image<uchar,nDim> OutputImageType;
  typedef itk::RescaleIntensityImageFilter<FloatImageType,OutputImageType> RescaleFilterType;
  //typedef itk::PercentileRescaleIntensityImageFilter<FloatImageType,OutputImageType> RescaleFilterType;
  typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType,FloatImageType> VectorElementSelectionFilter;

  RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
  rescaleFilter->ReleaseDataFlagOn();
  rescaleFilter->SetOutputMinimum(0);
  rescaleFilter->SetOutputMaximum(255);
  // for PercentileRescaleIntensityImageFilter
  //rescaleFilter->SetPercentiles(1e-6, 1 - 1e-6);  
  //rescaleFilter->SetVerbose(1);

  InputImageType::Pointer inputImage =
    ImportFilterFromRawData<uchar, InputImageType>(slice.getRawData(),
                            slice.getWidth(), slice.getHeight(), slice.getDepth());

  InputImageType::RegionType region = inputImage->GetLargestPossibleRegion();
  InputImageType::SizeType size = region.GetSize();
  ulong imageSize = size[0]*size[1]*(ulong)size[2];

  printf("[F_Filter] Loading input image (%d,%d,%d) %ld\n", slice.getWidth(), slice.getHeight(), slice.getDepth(), imageSize);

  // allocate memory to store features
  uchar* node_features = new uchar[imageSize];

  uchar** supernode_features = new uchar*[sizeFV];
  ulong nSupernodes = slice.getNbSupernodes();
  node n;
  for(int i = 0; i < sizeFV; i++) {
    supernode_features[i] = new uchar[nSupernodes];
  }
  features = supernode_features; // keep supernode features in memory

  printf("[F_Filter] Computing features at %d scales\n", numScales);

  int featIdx = 0;
  int featType = 0;

  // gradient magnitude
  for(int sc = 0; sc < numScales; ++sc) {

    {  // braces: for using scopes to induce memory deallocation via destructor
      typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<InputImageType,FloatImageType> GradMagFilter;
      GradMagFilter::Pointer gradmagFilter = GradMagFilter::New();
      gradmagFilter->SetSigma(scales[sc]);
      gradmagFilter->SetInput(inputImage);
      rescaleFilter->SetInput(gradmagFilter->GetOutput());
      rescaleFilter->GetOutput()->GetPixelContainer()->SetImportPointer(node_features, imageSize, false);
      rescaleFilter->Update();
      
      createSupernodeBasedFeatures(slice, node_features, featIdx);

#if OUTPUT_FILTER_IMAGES
      typedef itk::ImageFileWriter<OutputImageType> WriterType;
      WriterType::Pointer writer = WriterType::New();
      stringstream sout;
      sout << "gradient_" << sc << ".tif";
      printf("[F_Filter] Writing image %s\n", sout.str().c_str());
      writer->SetFileName(sout.str().c_str());
      writer->SetInput(rescaleFilter->GetOutput());
      writer->Update();
#endif     
 
      ++featIdx;
    }
  }

  // LoG
  for(int sc = 0; sc < numScales; ++sc) {

    {  // braces: for using scopes to induce memory deallocation via destructor
      typedef itk::LaplacianRecursiveGaussianImageFilter<InputImageType,FloatImageType> LaplacianFilter;
      LaplacianFilter::Pointer laplacianFilter = LaplacianFilter::New();
      laplacianFilter->SetSigma(scales[sc]);
      laplacianFilter->SetInput(inputImage);
      rescaleFilter->SetInput(laplacianFilter->GetOutput());
      rescaleFilter->GetOu