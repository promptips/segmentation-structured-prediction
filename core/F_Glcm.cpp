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

#include "F_Glcm.h"
#include "Config.h"

#define GLCM_DEFAULT_N_LEVELS 8

F_Glcm::~F_Glcm()
{
}

F_Glcm::F_Glcm()
{
  maxIntensity = 255;
  nItensityLevels = GLCM_DEFAULT_N_LEVELS;
  valToIdx = nItensityLevels/(double)maxIntensity;
}

int F_Glcm::getSizeFeatureVectorForOneSupernode()
{
  return nItensityLevels*nItensityLevels;
}

bool F_Glcm::getFeatureVectorForOneSupernode(osvm_node *x, Slice_P* slice, int supernodeId)
{
  supernode* s = slice->getSupernode(supernodeId);
  double value, n_value;
  int idx1, idx2, idx;
  node n;

  int w = slice->getWidth();
  int h = slice->getHeight();
  int d = slice->getDepth();

  // allocate memory to store data (done inside this function for multi-thread code)
  int sizeFV = getSizeFeatureVectorForOneSupernode();
  double* glcm_data = new double[sizeFV];
  memset(glcm_data, 0, sizeof(double)*sizeFV);

  nodeIterator ni = s->getIterator();
  ni.goToBegin();

  while(!ni.isAtEnd()) {
    ni.get(n);
    ni.next();

    value = slice->getIntensity(n.x, n.y, n.z);
    //idx1 = floor(value * valToIdx);
    idx1 = (int)(value * valToIdx);
    //assert(idx1 <= nItensityLevels);
    if(idx1 == nItensityLevels) {
      idx1 = 