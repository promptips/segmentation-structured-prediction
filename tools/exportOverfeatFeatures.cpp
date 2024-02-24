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

/*
 * Create superpixel features from features extracted from Overfeat  
 */

//-------------------------------------------------------------------- INCLUDES

#include <argp.h>
#include <cv.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

// SliceMe
#include "Slice3d.h"
#include "utils.h"
#include "globals.h"
#include "Config.h"

using namespace std;

//--------------------------------------------------------------------- GLOBALS

struct arguments
{
  char* output_filename;
  char* config_file;
  char* feature_typename;
};

struct arguments a_args;

//-------------------------------------------