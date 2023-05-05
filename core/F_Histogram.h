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

#ifndef F_HISTO_H
#define F_HISTO_H

#include "Histogram.h"
#include "Slice_P.h"
#include "Slice.h"
#include "Slice3d.h"
#include "Feature.h"

//-------------------------------------------------------------------------TYPES

enum eHistogramType
{
  NO_NEIGHBORS = 0,
  INCLUDE_NEIGHBORS,
  INCLUDE_NEIGHBORS_IN_SEPARATE_BINS,
  INCLUDE_NEIGHBORS_PLUS_LOCATION
};

//---------------------------