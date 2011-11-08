
#ifndef __HTKTRAIN_H__
#define __HTKTRAIN_H__


#include "LTKStringUtil.h"

using namespace std;



void createHMMPrototype(string outputFilePath,int numberOfStates,int numberOfGaussians,int vecSize,float selfTransProb,string prototypeName);
int htkTrain(const string& lipiRootPath,const string& tempDir,int numPointsPerState,int numGaussiansPerState,int vecSize,const string& strProfileDir);

#endif //#ifndef __HTKTRAIN_H__