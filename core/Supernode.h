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

#ifndef SUPERNODE_H
#define SUPERNODE_H

// standard libraries
#include <stdio.h>
#include <string.h>

// SliceMe
#include "globalsE.h"

//------------------------------------------------------------------------------

//typedef unsigned short sidType;
typedef int sidType; //should be signed (some functions like getSupernodeLabel return -1)
typedef uchar labelType;
typedef float probType;

//------------------------------------------------------------------------------

/**
 * Node structure using bit-fields to reduce amount of memory
 * A node is stored on 64 bit