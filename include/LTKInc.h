/*****************************************************************************************
* Copyright (c) 2007 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*****************************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2009-04-01 12:32:40 +0530 (Wed, 01 Apr 2009) $
 * $Revision: 751 $
 * $Author: royva $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Contains list of standard include files
 *
 * CONTENTS:   
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __LTKINC_H
#define __LTKINC_H

#include <exception>
#include <iostream>
#include <fstream>

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <numeric> //STLOPT
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <ctime>

using namespace std;

/** @file LTKInc.h
*/
#define SUCCESS 0
#define FAILURE 1

#ifdef WIN32
    #define SEPARATOR "\\"
#else
    #define SEPARATOR "/"
#endif

#define COMMENTCHAR '#'
#define LIST_FILE_DELIMITER " \t\r"

#define LTKReturnError(error)	errorCode = error; return error;

#define SHAPESETFILE "shapeset.cfg"
#define PROFILEFILE "profile.cfg"
#define WORDFILE "word.cfg"
#define WORDPROFILEFILE "wordprofile.cfg"
#define STATSFILE "stats.txt"
#define NETWORKFILE "network.txt"
#define MODELSLISTFILE "modelslist.txt"
#define DICTIONARYFILE "dictionary.txt"

//File Extensions
#define CONFIGFILEEXT ".cfg"
#define DATFILEEXT ".mdt"
#define POSFILEEXT ".pos"

#define PI 3.1415926F
#define EPS 0.00001F
#define EPS1 0.001F

//Isolated character recognizers
#define PCA "pca"
#define DTW "dtw"
#define POS "pos"
#define ZONE "zone"
#define HOLISTIC "holistic"
#define BCC "bcc"
#define EigDef "eigdef"
#define NN "nn"
#define HMM "hmm"
#define ACTIVEDTW "activedtw"

//Feature Extractors
#define POINT_FLOAT "pointfloat"
#define L7 "l7"
#define L7RADIUS "L7Radius"
#define NPEN "npen"
#define NPEN_WINDOW_SIZE "NPenWindowSize"

//Word recognizers
#define DP "dp"
#define BOXFLD "boxfld"

//PCA Config  values
#define REVERSEFEATUREVECTORS "ReverseFeatureVectors"
#define TRACEDIMENSION "ResampTraceDimension"
#define NUMEIGENVECTORS "NumEigenVectors"
#define SIZETHRESHOLD "NormLineWidthThreshold"
#define ASPECTRATIOTHRESHOLD "NormPreserveAspectRatioThreshold"
#define DOTTHRESHOLD "NormDotSizeThreshold"
#define LOOPTHRESHOLD "LoopThreshold"
#define HOOKLENGTHTHRESHOLD1 "HookLengthThreshold1"
#define HOOKLENGTHTHRESHOLD2 "HookLengthThreshold2"
#define HOOKANGLETHRESHOLD "HookAngleThreshold"
#define SMOOTHFILTERLENGTH "SmoothWindowSize"
#define DISTANCEMEASURE "DistanceMeasure"
#define QUANTIZATIONSTEP "QuantizationStep"
#define CONFIDENCEMAPPINGFILE "ConfidenceMappingFile"
#define PRESERVEASPECTRATIO "NormPreserveAspectRatio"
#define PRESERVERELATIVEYPOSITION "NormPreserveRelativeYPosition"
#define RESAMPLINGMETHOD "ResampPointAllocation"


//NN Config values
#define FEATURES "Features"
#define PROTOTYPEDISTANCE "NNPrototypeDistanceMeasure"
#define NEARESTNEIGHBORS "NNRecoNumNearestNeighbors"
#define FEATUREPOINTDISTANCE "FeaturePointDistance"
#define DTWBANDING "NNDTWBandingRadius"
#define FEATUREEXTRACTOR "FeatureExtractor"
#define FEATUREREPRESENTATION "FeatureRepresentation"
#define LENGTHBASED "lengthbased"
#define POINTBASED "pointbased"
#define INTERPOINTDISTBASED "interpointdistbased"

//ACTIVEDTW Config values
#define RETAINPERCENTEIGENENERGY "ActiveDTWRetainPercentEigenEnergy"
#define EIGENSPREADVALUE "ActiveDTWEigenSpreadValue"
#define USESINGLETON "ActiveDTWUseSingleton"
#define MINCLUSTERSIZE "ActiveDTWMinClusterSize"
#define MAXCLUSTERSIZE "ActiveDTWMaxClusterSize"
#define MDTFILEUPDATEFREQ "ActiveDTWMDTFileUpdateFreq"

//holistic config values
#define NUMNNS "numnns"
#define WORSTCASEVALUE "worstcasevalue"
#define NUMRESAMPLEPOINTS1 "NumResamplePoints1"
#define NUMRESAMPLEPOINTS2 "NumResamplePoints2"
#define SMOOTHFILTERLENGTH1 "SmoothFilterLength1"
#define SMOOTHFILTERLENGTH2 "SmoothFilterLength2"
#define SWANGNORMFACTOR "SwangNormFactor"
#define SWANGHOOKTHRESH "SwangHookThresh"
#define HORIZSTROKEANGLETHRESH "HorizStrokeAngleThresh"
//#define CONFIDENCEMAPPINGFILE "ConfidenceMappingFile"
//for bcc
#define NUMCHOICES "NumChoices"
#define COMBINATIONSCHEME "Combination"


#define NORMALIZETRACES "normalizetraces"
#define RESAMPLETRACES "resampletraces"
#define CENTERTRACES "centertraces"
#define REORDERTRACES "reordertraces"
#define REORIENTTRACES "reorienttraces"
#define NUMXGRIDS "numxgrids"
#define NUMYGRIDS "numygrids"
#define DISTRIBUTIONTHRESHOLD "distributionthreshold"
#define MINSHAPESUBSET "minshapesubset"
#define XTOLERANCE "xtolerance"
#define YTOLERANCE "xtolerance"
#define NUMXZONES "numxzones"
#define NUMYZONES "numyzones"
#define DENSITYTHRESH "densitythresh"
#define SAMPLETHRESH "samplethresh"
#define MAXBOXCOUNT "MaxBoxCount"
#define BOXEDSHAPEPROJECT "BoxedShapeProject"
#define BOXEDSHAPEPROFILE "BoxedShapeProfile"
#define DPSHAPEPROJECT "DPShapeProject"
#define DPSHAPEPROFILE "DPShapeProfile"
#define NUMSYMBOLS "NumSymbols"
#define NUMSHAPECHOICES "NumShapeChoices"
#define MINSHAPECONFID "MinShapeConfid"


//settings for DTW
#define FLEXIBILITYINDEX "FlexibilityIndex"
#define PROTOTYPESELECTION "NNTrainPrototypeSelectionMethod"
#define BANDING "Banding"
#define NUMFEATURES "NumFeatures"
#define PROTOREDFACTOR "NNTrainPrototypeReductionFactorPerClass"
#define NUMCLUSTERS "NNTrainNumPrototypesPerClass"
#define DTWEUCLIDEANFILTER "NNRecoDTWEuFilterOutputSize"
#define REJECT_THRESHOLD "NNRecoRejectThreshold"
#define ADAPTIVE_kNN "NNRecoUseAdaptiveKNN"

//settings of word recognizer
#define REC_UNIT_INFO "rec_unit_info"
#define USE_GRAMMAR   "use_grammar"
#define USE_WORDLIST  "use_wordlist"
#define USE_POSITIONAL INFO "use_positional_info"
#define REC_MODE "rec_mode"
#define DICTIONARY_MODE "dictionary_mode"

//For Word recognizer 

#define LTK_TRUE               0x01 
#define LTK_FALSE              0x00

//Reset parameters for Recognition context
#define LTK_RST_INK        0x02
#define LTK_RST_RECOGNIZER 0x04
#define LTK_RST_ALL        0xff

//Recognition units
#define REC_UNIT_UNKNOWN  0x08 //No segment info
#define REC_UNIT_SYMBOL   0x10 //Symbol
#define REC_UNIT_CHAR     0x11 //Char
#define REC_UNIT_WORD     0x12 //Word 

//Recognition mode for the recognizer
#define REC_MODE_BATCH     0x14 //batch mode
#define REC_MODE_STREAMING 0x16 //streaming (trace by trace)


//Dictionary options
#define DICT_DRIVEN			0x20	//recognizes only the words in the dictio
#define DICT_ASSISTED		0x21
#define DICT_OFF			0x22



#endif	//#ifndef __LTKINC_H


