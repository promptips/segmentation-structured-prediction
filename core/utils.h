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


#include <string>
#include <stdio.h>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <cv.h>
#include <highgui.h>
#ifndef _WIN32
#include <sys/resource.h>
#endif


//SliceMe
#include "Config.h"
#include "Slice.h" // labelType
#include "Slice_P.h"
#include "globalsE.h" // sizeSliceType

// ITK Header Files
#ifdef USE_ITK
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImportImageFilter.h"
#include "itkConnectedComponentFunctorImageFilter.h"
#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkShapeLabelObject.h"
#include "itkLabelMapToBinaryImageFilter.h"
#endif

using namespace std;

#ifndef UTILS_H
#define UTILS_H

//------------------------------------------------------------------------MACROS

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifdef _WIN32
#define REMOVE "del"
#define ZIP "7z"
#else
#define REMOVE "rm"
#define ZIP "zip"
#endif

//---------------------------------------------------------------------FUNCTIONS

void bresenhamLine3d(int* p1, int* p2, float*& xs, float*& ys, float*& zs,
                     int& nb_pts);

int compareBWImages(const char* imageModelName,
                    const char* imageName,
                    float& true_neg,
                    float& true_pos,
                    float& false_neg,
                    float& false_pos,
                    bool normalize = true);
/*
 * Only compare first channel of the 2 given images
 */
int compareBWImages(IplImage *ptrModel,
                    IplImage *ptrImg,
                    float& true_neg,
                    float& true_pos,
                    float& false_neg,
                    float& false_pos,
                    bool normalize = true);

void compareVolumes(uchar* annotationData,
                    uchar* data,
                    int width,
                    int height,
                    int depth,
                    float& true_neg,
                    float& true_pos,
                    float& false_neg,
                    float& false_pos,
                    bool normalize=true,
                    bool useColorAnnotations=false,
                    ulong* TP = 0,
                    ulong* TN = 0);

void compareMultiLabelVolumes(Slice_P& slice_GT,
                              const labelType* labels,
                              const int class_label,
                              float& true_neg,
                              float& true_pos,
                              float& false_neg,
                              float& false_pos,
                              bool normalize,
                              bool useColorAnnotations = false,
                              ulong* TP = 0,
                              ulong* TN = 0);

void compareMultiLabelVolumes_nodeBased(Slice_P& slice_GT,
                                        const labelType* groundtruth,
                                        const labelType* labels,
                                        const int class_label,
                                        float& true_neg,
                                        float& true_pos,
                                        float& false_neg,
                                        float& false_pos,
                                        bool normalize,
                                        bool useColorAnnotations,
                                        ulong* TP,
                         