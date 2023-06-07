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

#ifndef INFERENCE_GLOBALS_H
#define INFERENCE_GLOBALS_H

#include "globalsE.h"

#ifdef _WIN32
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#endif

//------------------------------------------------------------------------------

typedef float nodeCoeffType;
typedef float edgeCoeffType;

//------------------------------------------------------------------------------

// set this to 1 to lear an offset for each class
// comment line to deactivate offset (do not set to 0)
#define W_OFFSET 0

#define INCLUDE_LOCAL_EDGES 1
//#define INCLUDE_LOCAL_EDGES 0

#define INCLUDE_GLOBAL_CLASSIFIER_IN_UNARY_TERM 1

#define SAMPLING_MUL_COEFF 10.0
//#define SAMPLING_MUL_COEFF 2.0

#define LOSS_VOC 2
#define LOSS_NODE_BASED 3

#define MAX_DISTANCE_DT 7

//------------------------------------------------------------------------------
// DO NOT EDIT

// macros used to index w vector
#if USE_LONG_RANGE_EDGES
#define SVM_FEAT_INDEX0(param) (((param)->nDistances*(param)->nGradientLevels*(param)->nOrientations*(param)->nClasses*(param)->nClasses) + \
                                (param)->n