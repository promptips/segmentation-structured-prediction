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

#ifndef COLORMAP_H
#define COLORMAP_H

#include "Supernode.h"

#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

//------------------------------------------------------------------------------

#define COLORMAP_SIZE 64

/* Colormap from Matlab :
autumn=colormap(autumn)*255;
save('autumn.txt','-ascii','-tabs','autumn');
bone=colormap(bone)*255;
save('bone.txt','-ascii','-tabs','bone');
jet=colormap(jet)*255;
save('jet.txt','-ascii','-tabs','jet');
*/

enum eColorMapType
  {
    COLORMAP_PROBS,
    COLORMAP_AUTUMN,
    COLORMAP_BONE,
    COLORMAP_JET
  };

extern float colormap_autumn[COLORMAP_SIZE*3];

extern float colormap_bone[COLORMAP_SIZE*3];

extern float colormap_jet[COLORMAP_SIZE*3];

class Colormap
{
 public:

  static Colormap* pInstance;

  static Colormap* Instance()
  {    
    if (pInstance == 0)  // is it the first call?
      {
        //printf("[Colormap] Error : you have to load a configuration file\n");
        //exit(-1);
        pInstance = new Colormap(); // create unique instance
      }
    return pInstance; // address of unique instance
  }

  std::map<ulong, labelType>& get() { return classIdxToLabel; }

  void set(const char* colormapFilename)
  {
    printf("[Colormap] Setting colormap = %s\n", colormapFilename);
    getClassToLabelMap(colormapFilename, classIdxToLabel);
  }

 private:
  std::map<ulong, labelType> classIdxToLabel;

  // FIXME : functions below were duplicated fr