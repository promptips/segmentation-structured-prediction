// LKM.h: interface for the LKM class.
//
// Copyright (C) Radhakrishna Achanta
// All rights reserved
// Email: firstname.lastname@epfl.ch
//////////////////////////////////////////////////////////////////////

#if !defined(_LKM_H_INCLUDED_)
#define _LKM_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <limits.h>
#include <vector>
#include <string>
#include <stack>
using namespace std;

#ifndef WINDOWS
#include "supervoxel_globals.h"
#endif

const int dx4[4] = {-1,  0,  1,  0};
const int dy4[4] = { 0, -1,  0,  1};
const int dx6[6] = {-1,  0,  1,  0,  0, 0};
const int dy6[6] = { 0, -1,  0,  1,  0, 0};
const int dz6[6] = { 0,  0,  0,  