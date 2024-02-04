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
const int dz6[6] = { 0,  0,  0,  0, -1, 1};
const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
const int dx10[10] = {-1,  0,  1,  0, -1,  1,  1, -1,  0, 0};
const int dy10[10] = { 0, -1,  0,  1, -1, -1,  1,  1,  0, 0};
const int dz10[10] = { 0,  0,  0,  0,  0,  0,  0,  0, -1, 1};

//typedef unsigned short sidType;
//#define UNDEFINED_LABEL 65535
//#define MAX_SID USHRT_MAX

typedef int sidType;
#define UNDEFINED_LABEL -1
#define MAX_SID INT_MAX

struct sPixel
{
  int x;
  int y;
  int z;
};

class LKM  
{
public:
	LKM(bool _freeMem = true);
	virtual ~LKM();

	void DoSuperpixelSegmentation(
		UINT*&				ubuff,
		const int					width,
		const int					height,
		sidType*&	       				klabels,
		int&						numlabels,
		const int&					STEP,
                const float M = 10.0f);

	void DoSupervoxelSegmentation(
		UINT**&				ubuffvec,
		const int&					width,
		const int&					height,
		const int&					depth,
		sidType**&	       				klabels,
		int&						numlabels,
		const int&					STEP,
                const double cubeness = 20);

        void DoSupervoxelSegmentationForGrayVolume(
                                                   double**		       			ubuffvec,
                                                   const int					width,
                                                   const int					height,
                                                   const int					depth,
                                                   sidType**&					klabels,
                                                   int&						numlabels,
                                                   const int					STEP,
                                                   const double cubeness = 20);

	void SaveLabels(
                        sidType*&					labels,
                        const int					width,
                        const int					height,
                        string				filename,
                        string				path);
        
	void SaveLabels_Text(
                             sidType*&					labels,
                             const int					width,
                             const int					height,
                             string				filename,
                             string				path);

	void SaveLabels(
		const sidType**&       			labels,
		const int&	       			width,
		const int&	       			height,
		const int&	       			depth,
		const string&				filename,
		const string&				path);


private:

	void PerformLKMClustering(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		sidType*&						klabels,
		const int&					STEP,
                const float M);

	void PerformLKMVoxelClustering(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector<double>&				kseedsz,
		sidType**&						klabels,
		const int&					STEP,
                const double cubeness);

        // Al : Added clustering for gray images
	void PerformLKMVoxelClustering(
		vector<double>&				kseedsl,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector<double>&				kseedsz,
		sidType**&     		       		klabels,
		const int&		       		STEP,
                const double cubeness);

	void GetKValues_LABXY(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const int&		       		STEP,
		const bool&		       		perturbseeds);

	void GetKValues_LABXYZ(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector<double>&				kseedsz,
		const int&				STEP);

	void GetKValues_LABXYZ(
		vector<double>&				kseedsl,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector<double>&				kseedsz,
		const int&					STEP);

	void PerturbSeeds(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy);

	void DetectLabEdges(
		double*&				lvec,
		double*&				avec,
		double*&				bvec,
		int&					width,
		int&					height,
		vector<double>&				edges);

	void EnforceConnectivityForLargeImages(
		const int					width,
		const int					height,
		sidType*&      					labels,//input labels that need to be corrected to remove stray single labels
		int&						numlabels);

	void EnforceConnectivityForLargeImages(
		const int&					width,
		const int&					height,
		const int&					depth,
		sidType**&						labels,//input labels that need to be corrected to remove stray single labels
		int&						numlabels);

	void RGB2LAB(const int& r, const int& g, const int& b, double& lval, double& aval, double& bval);

	void DoRGBtoLABConversion(
		UINT*&				ubuff,
		double*&					lvec,
		double*&					avec,
		double*&					bvec);

	void DoRGBtoLABConversion(
		UINT**&						ubuff,
		double**&					lvec,
		double**&					avec,
		double**&					bvec);

	//===========================================================================
	///	FindNextRecursively
	///
	///	Helper function for RelabelStraySuperpixels. Called recursively.
	//===========================================================================
	void FindNextRecursively(
		sidType*&				labels,
		sidType*&						nlabels,
		const int&					height,
		const int&					width,
		const int&					h,
		const int&					w,
		const int&					lab,
		int*&						xvec,
		int*&						yvec,
		int&						count)
	{
		int oldlab = labels[h*width+w];
		for( int i = 0; i < 4; i++ )
		{
			int y = h+dy4[i];int x = w+dx4[i];
			if((y < height && y >= 0) && (x < width && x >= 0) )
			{
				int ind = y*width+x;
				if(nlabels[ind] < 0 && labels[ind] == oldlab )
				{
					xvec[count] = x;
					yvec[count] = y;
					count++;
					nlabels[ind] = lab;
					FindNextRecursively(labels, nlabels, height, width, y, x, lab, xvec, yvec, count);
				}
			}
		}
	}

	//===========================================================================
	///	RelabelStraySuperpixels
	///
	///	Some superpixels may be unconnected, Relabel them. Recursive algorithm
	/// used here, can crash if stack overflows. This will only happen if the
	/// superpixels are very large, otherwise safe.
	//===========================================================================
	void RelabelStraySuperpixels(
		sidType*&				labels,
		const int&					width,
		const int&					height,
		sidType*&	   			        nlabels,
		int&						numlabels,
		const int&					expectedSuperpixelSize)
	{
		int sz = width*height;		
		{for( int i = 0; i < sz; i++ ) nlabels[i] = -1;}

		//------------------
		// labeling
		//------------------
		int lab(0);
		int i(0);
		int adjlabel(0);//adjacent label
		int* xvec = new int[sz];//worst case size
		int* yvec = new int[sz];//worst case size
		{for( int h = 0; h < height; h++ )
		{
			for( int w = 0; w < width; w++ )
			{
				if(nlabels[i] < 0)
				{
					nlabels[i] = lab;
					//-------------------------------------------------------
					// Quickly find an adjacent label for use later if needed
					//-------------------------------------------------------
					{for( int n = 0; n < 4; n++ )
					{
						int x = w + dx4[n];
						int y = h + dy4[n];
						if( (x >= 0 && x < width) && (y >= 0 && y < height) )
						{
							int nindex = y*width + x;
							if(nlabels[nindex] >= 0) adjlabel = nlabels[nindex];
						}
					}}
					xvec[0] = w; yvec[0] = h;
					int count(1);
					FindNextRecursively(labels, nlabels, height, width, h, w, lab, xvec, yvec, count);
					//-------------------------------------------------------
					// If segment size is less then a limit, assign an
					// adjacent label found before, and decrement label count.
					//-------------------------------------------------------
					if(count <= (expectedSuperpixelSize >> 2))
					{
						for( int c = 0; c < count; c++ )
						{
							int ind = yvec[c]*width+xvec[c];
							nlabels[ind] = adjlabel;
						}
						lab--;
					}
					lab++;
				}
				i++;
			}
		}}
		//------------------
		numlabels = lab;
		//------------------
		if(xvec) delete [] xvec;
		if(yvec) delete [] yvec;
	}


	void RelabelSupervoxels(
		con