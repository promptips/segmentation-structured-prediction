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

#include <fstream>

// SliceMe
#include "Config.h"
#include "F_LoadFromFile.h"
#include "oSVM.h"
#include "utils.h"

//------------------------------------------------------------------------------

#define OUTPUT_FEATURES_TO_TXT_FILE 0

#define UPSIDE_DOWN_FEATURES 0

//------------------------------------------------------------------------------

F_LoadFromFile::F_LoadFromFile() {
  featureSize = 0;
  nFeatures = 0;
  featurePath = "";
  initialized = false;

#if UPSIDE_DOWN_FEATURES
  printf("[F_LoadFromFile] UPSIDE_DOWN_FEATURES is set to 1\n");
#endif
}

void F_LoadFromFile::loadFeatureFilenames(const char* filename,
                                          vector<string>* lFeatureFilenames)
{
  ifstream ifs(filename);
  if(ifs.fail()) {
    printf("[F_LoadFromFile] Error while loading %s\n", filename);
    exit(-1);
  }
  
  // Load path containing files
  getline(ifs, featurePath);

  // Load name of the files containing the features
  string fn;
  while(!ifs.eof()) {
    getline(ifs,fn);
    if(!fn.empty()) {
      lFeatureFilenames->push_back(fn);
    }
  }
  ifs.close();
}

string F_LoadFromFile::getAbsoluteFeaturePath(const string& featureFilename,
                                              const string& inputDir)
{
  string fullpath = featureFilename;
  string inputDir_rel = inputDir;
  if(inputDir_rel[inputDir_rel.length()-1] == '/')
    inputDir_rel = inputDir_rel.substr(0,inputDir_rel.length()-1);
  inputDir_rel = inputDir_rel.substr(inputDir_rel.rfind('/'));
  fullpath += inputDir_rel;
  fullpath += "/";
  return fullpath;
}

void F_LoadFromFile::init(Slice3d& slice3d, const char* filename,
                          vector<sidType>& lNodes)
{
  if(initialized) {
    printf("[F_LoadFromFile] Warning: Features were already loaded.\n");
    return;
  }

  vector<string> lFeatureFilenames;
  loadFeatureFilenames(filename, &lFeatureFilenames);

  if(lFeatureFilenames.size() == 0) {
    // no feature filename specified. Check if there is a file with a name
    // matching filename
    string featName = getNameFromPathWithoutExtension(slice3d.getName());
    string fullpath = featurePath + "/";
    fullpath += featName;
    printf("[F_LoadFromFile] Checking binary file %s\n", fullpath.c_str());
    if(fileExists(fullpath)) {
      lFeatureFilenames.push_back(featName);
      loadTextFeatures(slice3d, lFeatureFilenames);
    } else {
      string fullpath_with_ext = fullpath + ".bin";
      featName += ".bin";
      if(fileExists(fullpath_with_ext)) {
        printf("[F_LoadFromFile] Loading features from binary file %s\n", fullpath_with_ext.c_str());
        vector<string> lFiles;
        lFiles.push_back(featName);
        loadSupervoxelBasedFeaturesFromBinary(slice3d, lFiles, lNodes);
      } else {
        printf("[F_LoadFromFile] No features to be loaded in %s\n", filename);
      }
    }
  } else {
    string ext = getExtension(lFeatureFilenames[0]);
    if(ext == "tif") {
      PRINT_MESSAGE("[F_LoadFromFile] Loading features from TIF file %s\n", lFeatureFilenames[0].c_str());
      loadSupervoxelBasedFeaturesFromTIF(slice3d, lFeatureFilenames, lNodes);
    } else {
      if(ext == "bin") {
        printf("[F_LoadFromFile] Loading features from binary file %s\n", lFeatureFilenames[0].c_str());
        loadSupervoxelBasedFeaturesFromBinary(slice3d, lFeatureFilenames, lNodes);
      } else {
        loadTextFeatures(slice3d, lFeatureFilenames);
      }
    }
  }

  initialized = true;
}

void F_LoadFromFile::loadSupervoxelBasedFeaturesFromTIF(Slice3d& slice3d,
                                                        const vector<string>& lFeatureFilenames,
                                                        vector<sidType>& lNodes)
{
  featureSize = lFeatureFilenames.size();
  nFeatures = lNodes.size();
  PRINT_MESSAGE("[F_LoadFromFile] Allocating memory for %ld nodes and %d features\n",
                lNodes.size(), featureSize);
#if !USE_SPARSE_STRUCTURE
  features = new fileFeature