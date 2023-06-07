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

#ifndef INFERENCE_H
#define INFERENCE_H

#include <string>
#include <vector>

#include "energyParam.h"
#include "graphInference.h"
#include "svm_struct_api_types.h"

// SliceMe
#include "Feature.h"
#include "Slice_P.h"
#include "Slice.h"

//------------------------------------------------------------------------------

GraphInference* createGraphInferenceInstance(int algo_type,
                                             Slice_P* slice,
                                             const EnergyParam& param,
                                             Feature* _feature);

GraphInference* createGraphInferenceInstance(int algo_type,
                                             Slice_P* slice,
                                             const EnergyParam& param,
                                             Feature* feature,
                                             labelType* groundTruthLabels,
                                             double* lossPerLabel);

GraphInference* createGraphInferenceInstance(int algo_type,
                                             Slice_P* slice,
                                             const EnergyParam& param,
                                             Feature* feature,
                                             labelType* groundTruthLabels,
                                             double* lossPerLabel,
                                             map<sidType, node