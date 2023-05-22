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


// standard libraries
#include <sstream>
#include <time.h>

// Third-party libraries
#include "LKM.h"

// SliceMe
#include "Slice3d.h"
#include "globalsE.h"
#include "utils.h"

#define USE_RUN_LENGTH_ENCODING

Slice3d::Slice3d(unsigned char* a_raw_data,
                 int awidth, int aheight,
                 int adepth,
                 sizeSliceType vstep,
                 int anChannels,
                 bool _loadNeighbors)
{
  init();
  delete_raw_data = false; // caller is responsible for freeing memory
  width = awidth;
  height = aheight;
  depth = adepth;
  raw_data = a_raw_data;
  nChannels = anChannels;
  loadNeighbors = _loadNeighbors;

  if(vstep > depth) {
    supernode_step = depth;
  } else {
    supernode_step = vstep;
  }

  sliceSize = width*height;
}

Slice3d::Slice3d(const char* input_dir,
                 int vstep,
                 int nImgs,
                 bool _loadNeighbors)
{
  width = UNITIALIZED_SIZE;
  height = UNITIALIZED_SIZE;
  depth = UNITIALIZED_SIZE;
  nChannels = 1;
  loadNeighbors = _loadNeighbors;

  inputDir = string(input_dir);
  init();
  loadFromDir(input_dir, nImgs);

  sliceSize = width*height;

  if(vstep > depth) {
    supernode_step = depth;
  } else {
    supernode_step = vstep;
  }
}

Slice3d::Slice3d(const char* input_dir,
                 int awidth,
                 int aheight,
                 int adepth,
                 int vstep,
                 bool _loadNeighbors)
{
  init();
  width = awidth;
  height = aheight;
  depth = adepth;
  nChannels = 1;
  loadNeighbors = _loadNeighbors;

  inputDir = string(input_dir);
  loadFromDir(input_dir, depth);

  sliceSize = width*height;

  if(vstep > depth) {
    supernode_step = depth;
  } else {
    supernode_step = vstep;
  }
}

Slice3d::Slice3d(const char* input_dir,
                 const node& _start,
                 const node& _end,
                 int vstep,
                 bool _loadNeighbors)
{
  init();
  width = _end.x - _start.x;
  height = _end.y - _start.y;
  depth = _end.z - _start.z;
  start_x = _start.x;
  start_y = _start.y;
  start_z = _start.z;
  nChannels = 1;
  loadNeighbors = _loadNeighbors;

  inputDir = string(input_dir);
  loadFromDir(input_dir, depth);

  sliceSize = width*height;

  if(vstep > depth) {
    supernode_step = depth;
  } else {
    supernode_step = vstep;
  }
}

Slice3d::Slice3d() {
  init();
}

void Slice3d::init()
{
  mSupervoxels = 0;
  nbEdges = 0;
  supernodeLabelsLoaded = false;
  maxDegree = -1;
  minPercentToAssignLabel = MIN_PERCENT_TO_ASSIGN_LABEL;
  nLabels = 0;
  raw_data = 0;
  includeOtherLabel = true;
  delete_raw_data = true;
  cubeness = SUPERVOXEL_DEFAULT_CUBENESS;

  start_x = 0;
  start_y = 0;
  start_z = 0;

#ifdef USE_REVERSE_INDEXING
  klabels = 0;
#endif
}

Slice3d::~Slice3d()
{
  if(mSupervoxels) {
    for(map< sidType, supernode* >::iterator it = mSupervoxels->begin();
        it != mSupervoxels->end();it++) {
      delete it->second;
    }
    delete mSupervoxels;
  }
  
  if(delete_raw_data && raw_data) {
    delete[] raw_data;
  }
}

uchar Slice3d::at(int x, int y, int z)
{
  return raw_data[z*sliceSize+y*width+x];
}

int Slice3d::raw2Double(double**& ptr_data)
{
  // supervoxel library needs a cube made of ints so we have to convert the cube
  // ask for enough memory for the texels and make sure we got it before proceeding
  ptr_data = new double* [depth];
  if (ptr_data == 0) {
    printf("[Slice3d] Error while allocating memory for 3d volume\n");
    return -1;
  }

  int i = 0;
  for(int z = 0;z < depth; z++) {
    ptr_data[z] = new double[sliceSize];
    for(int xy = 0; xy < sliceSize; xy++) {
      ptr_data[z][xy] = (double)raw_data[i];
      i++;
    }
  }
  return 0;
}

int Slice3d::raw2RGB(unsigne