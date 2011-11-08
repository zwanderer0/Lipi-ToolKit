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
 * FILE DESCR: Contains common macro Definitions.
 *
 * CONTENTS:
 *
 * AUTHOR:     Vijayakumara M.
 *
 * DATE:       December 01, Aug,  2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __COMMONMACORS_H
#define __COMMONMACORS_H

#include "LTKTypes.h"


/** @file LTKmacros.h
* @brief Contains the macors used in reco/shaperec/pca and reco/shaperec/dtw modules.
*/
//macors used in reco/shaperec/pca and reco/shaperec/dtw modules.

/**
* @brief Defines the sequence of preprocessor functions to be executed
*/
#define PREPROCSEQUENCE "PreprocSequence"

/**
* @brief Defines the scale to be used to compute the number of iterations for LVQ training
*/
#define LVQITERATIONSCALE "LVQIterationScale"

/**
* @brief Defines initial value of alpha used in LVQ training
*/
#define LVQINITIALALPHA "LVQInitialAlpha"

/**
* @brief Defines the distance measure to be used in LVQ training.
*/
#define LVQDISTANCEMEASURE "LVQDistanceMeasure"

/**
* @brief Defines the name of the preprocessor dll file
*/
#define PREPROC "preproc"

/**
* @brief Defines the string compare functions
*/
#ifdef _WIN32
#define LTKSTRCMP _stricmp
#else
#define LTKSTRCMP strcasecmp
#endif

/**
* @brief Defines the name of the lipiengine dll file
*/
#define LIPIENGINE_MODULE_STR "lipiengine"

/**
* @brief Defines the name of the logger dll file
*/
#define LOGGER_MODULE_STR "logger"

/**
* @brief Defines the Lipi Root environment string
*/
#define LIPIROOT "$LIPI_ROOT"

/**
* @brief Defines the project is dynamic, where the numshapes can take any number of values
*/
#define DYNAMIC "Dynamic"

/**
* @brief Defines the header version for the model data file
*/
#define HEADERVERSION		"1.0.0"


#define VERSION_STR_LEN_SMALL	4

/**
* @brief Defines the length of the version information
*/
#define VERSION_STR_LEN			10

/**
* @brief Defines the checksum header string length
*/
#define CKSUM_HDR_STR_LEN		10

/**
* @brief Defines the starting error number
*/
#define ERROR_START_NO			100

/**
* @brief Defines the maximum error number
*/
#define MAX_ERROR_NO			228

/**
* @brief Defines the maximum file path length
*/
#define MAX_PATH_LEN			255

/**
* @brief Defines the maximum string length
*/
#define MAX_STRLEN				255

/**
* @brief Defines the lipi root environment string used in LipiEngine.cfg
*/
#define LIPIROOT_ENV_STRING	"LIPI_ROOT"

/**
* @brief Defines the type of project used in LipiEngine.cfg
*/
#define PROJECT_TYPE_STRING "ProjectType"

#define PROJECT_TYPE_SHAPEREC	"SHAPEREC"

/**
* @brief Defines the shape recognizer name used in LipiEngine.cfg
*/
#define SHAPE_RECOGNIZER_STRING "ShapeRecMethod"

/**
* @brief Defines the word recognizer name used in LipiEngine.cfg
*/
#define WORD_RECOGNIZER_STRING "WordRecognizer"

/**
* @brief Defines the path of the project directory in the Lipi tree
*/
#define PROJECTS_PATH_STRING	SEPARATOR + "projects" + SEPARATOR

/**
* @brief Defines the path of the profile directory in the Lipi tree
*/
#define PROFILE_PATH_STRING		SEPARATOR + "config" + SEPARATOR

/**
* @brief Defines the name of the default profile
*/
#define DEFAULT_PROFILE "default"

/**
* @brief Defines the name of profile config file
*/
#define PROFILE_CFG_STRING		"profile.cfg"

/**
* @brief Defines the name of project config file
*/
#define PROJECT_CFG_STRING		"project.cfg"

/**
* @brief Defines the NumShapes attributes in the project config file
*/
#define PROJECT_CFG_ATTR_NUMSHAPES_STR	"NumShapes"

/**
* @brief Defines the name of the create shape recognizer function
*/
#define CREATESHAPERECOGNIZER_FUNC_NAME	"createShapeRecognizer"

/**
* @brief Defines the name of the delete shape recognizer function
*/
#define DELETESHAPERECOGNIZER_FUNC_NAME	"deleteShapeRecognizer"

/**
* @brief Defines the name of the createWordRecognizer function
*/
#define CREATEWORDRECOGNIZER_FUNC_NAME	"createWordRecognizer"

/**
* @brief Defines the name of the deleteWordRecognizer function
*/
#define DELETEWORDRECOGNIZER_FUNC_NAME	"deleteWordRecognizer"

/**
* @brief Defines the name of the resampleTraceGroup function
*/
#define RESAMPLE_TRACE_GROUP_FUNC		"resampleTraceGroup"

/**
* @brief Defines the name of the resampleTraceGroup1 function
*/
#define RESAMPLE_TRACE_GROUP_ONE_FUNC	"resampleTraceGroup1"

/**
* @brief Defines the name of the preprocess function
*/
#define	PREPROCESS_FUNC					"preprocess"

/**
* @brief Defines the name of the normalizeSize function
*/
#define NORMALIZE_FUNC				    "normalizeSize"

/**
* @brief Defines the name of the removeDuplicate function
*/
#define REMOVE_DUPLICATE_POINTS_FUNC	"removeDuplicatePoints"

/**
* @brief Defines the name of the smoothenTraceGroup function
*/
#define SMOOTHEN_TRACE_GROUP_FUNC		"smoothenTraceGroup"

/**
* @brief Defines the name of the dehookTraces function
*/
#define DEHOOKTRACES_FUNC				"dehookTraces"

/**
* @brief Defines the name of the normalizeOrientation function
*/
#define NORMALIZE_ORIENTATION_FUNC		"normalizeOrientation"

/**
* @brief Defines the Delimiter sequence for tokenize the strings
*/
#define DELEMITER_SEQUENCE " {},"

/**
* @brief Defines the Delimiter character for separating the function name with class name
*/
#define DELEMITER_FUNC "::"

/**
* @brief Defines the Delimiter sequence for tokenize the header information
*/
#define TOKENIZE_DELIMITER	"<>=\n\r"

/**
* @brief Defines the string "CKS" (ChecKSum)
*/
#define CKS					"CKS"

/**
* @brief Defines the project name used for header information
*/
#define PROJNAME			"PROJNAME"

/**
* @brief Defines the project type used for header information
*/
#define PROJTYPE			"PROJTYPE"

/**
* @brief Defines the recognizer name used for header information
*/
#define RECNAME				"RECNAME"

/**
* @brief Defines the recognizer version used for header information
*/
#define	RECVERSION			"RECVERSION"

/**
* @brief Defines the string CREATETIME for header information
*/
#define CREATETIME			"CREATETIME"

/**
* @brief Defines the string MODTIME for header information
*/
#define MODTIME				"MODTIME"

/**
* @brief Defines the platform name (Windows or Linux)
*/
#define PLATFORM			"PLATFORM"

/**
* @brief Defines the length of the comment used in header
*/
#define COMMENTLEN			"COMMENTLEN"

/**
* @brief Defines the data offset length used in header
*/
#define DATAOFFSET			"DATAOFFSET"

/**
* @brief Defines the number of shapes used in header information
*/
#define NUMSHAPES			"NUMSHAPES"

/**
* @brief Defines the size of integer used in header information
*/
#define SIZEOFINT			"SIZEOFINT"

/**
* @brief Defines the size of unsigned integer used in header information
*/
#define SIZEOFUINT			"SIZEOFUINT"

/**
* @brief Defines the size of short integer used in header information
*/
#define SIZEOFSHORTINT		"SIZEOFSHORTINT"

/**
* @brief Defines the size of float used in header information
*/
#define SIZEOFFLOAT			"SIZEOFFLOAT"

/**
* @brief Defines the size of character used in header information
*/
#define SIZEOFCHAR			"SIZEOFCHAR"

/**
* @brief Defines the header length
*/
#define HEADERLEN			"HEADERLEN"

/**
* @brief Defines the dataset for header information
*/
#define DATASET				"DATASET"

/**
* @brief Defines the comment information in header
*/
#define COMMENT				"COMMENT"

/**
* @brief Defines the type of byte order (Little Endian or Big Endian)
*/
#define BYTEORDER			"BYTEORDER"

/**
* @brief Defines the operating system version
*/
#define OSVERSION			"OSVERSION"

/**
* @brief Defines the type of processor architecture
*/
#define PROCESSOR_ARCHITEC	"PROCESSOR_ARCHITEC"

/**
* @brief Defines the Header version for header information
*/
#define HEADERVER			"HEADERVER"


#define PCA_ALGO			"PCA"
#define WINDOWS				"Windows"
#define LINUX				"Linux"

//Defined twice
#define BYTEORDER			"BYTEORDER"

/**
* @brief Defines the create preprocessor method name
*/
#define CREATEPREPROCINST	"createPreprocInst"

/**
* @brief Defines the delete preprocessor method name
*/
#define DESTROYPREPROCINST	"destroyPreprocInst"

#define FI_MIN				0
#define FI_MAX				2

/**
* @brief Defines the minimum value in which the prototype reduction factor can take
*/
#define PROTOTYPEREDFACT_MIN 0

/**
* @brief Defines the maximum value in which the prototype reduction factor can take
*/
#define PROTOTYPEREDFACT_MAX 100

/**
* @brief Defines the input file type is the ink file
*/
#define INK_FILE				"ink"

/**
* @brief Defines the input file type is the feature file
*/
#define FEATURE_FILE			"feature"

/**
* @brief Defines the DTW type of distance to be calculated
*/
#define DTW_DISTANCE 			"dtw"

/**
* @brief Defines the euclidean type of distance to be calculated
*/
#define EUCLIDEAN_DISTANCE 		"eu"

/**
* @brief Defines the delimiter for the feature extractor
*/
#define FEATURE_EXTRACTOR_DELIMITER		"|"

/**
* @brief Defines the delimiter for the space (assuming only signle space between
* class and the features in MDT file
*/
#define CLASSID_FEATURES_DELIMITER		" "

/**
* @brief Defines the code for PointFloat Feature Extractor
*/
#define POINT_FLOAT_SHAPE_FEATURE_EXTRACTOR  			101

/**
* @brief Defines the code for L7 Feature Extractor
*/
#define L7_SHAPE_FEATURE_EXTRACTOR  			102

/**
* @brief Defines the create preprocessor method name
*/
#define CREATE_SHAPE_FEATURE_EXTRACTOR	"createShapeFeatureExtractor"

/**
* @brief Defines the delete preprocessor method name
*/
#define DELETE_SHAPE_FEATURE_EXTRACTOR	"deleteShapeFeatureExtractor"

#define FE_NAME	"FE_NAME"

#define FE_VER	"FE_VER"

#define PREPROC_SEQ "PREPROC_SEQ"

#define DOT_SIZE_THRES "DOT_SIZE_THRES"

#define ASP_RATIO_THRES "ASP_RATIO_THRES"

#define DOT_THRES "DOT_THRES"

#define PRESER_REL_Y_POS "PRESER_REL_Y_POS"

#define PRESER_ASP_RATIO "PRESER_ASP_RATIO"

#define NORM_LN_WID_THRES "NORM_LN_WID_THRES"

#define RESAMP_POINT_ALLOC "RESAMP_POINT_ALLOC"

#define SMOOTH_WIND_SIZE "SMOOTH_WIND_SIZE"

#define TRACE_DIM "TRACE_DIM"

#define PROTOTYPE_SELECTION_CLUSTERING "hier-clustering"

#define PROTOTYPE_SELECTION_LVQ	"lvq"

#define PROTO_RED_FACTOR_AUTOMATIC "automatic"

#define PROTO_RED_FACTOR_NONE	"none"

#define PROTO_RED_FACTOR_COMPLETE	"complete"

#define DTW_EU_FILTER_ALL	"all"

#define NAME_POINT_FLOAT_SHAPE_FEATURE_EXTRACTOR "PointFloatShapeFeatureExtractor"

#define NAME_L7_SHAPE_FEATURE_EXTRACTOR "L7ShapeFeatureExtractor"

#define NAME_NPEN_SHAPE_FEATURE_EXTRACTOR "NPenShapeFeatureExtractor"

#define NEW_LINE_DELIMITER '\n'

#define DELTE_SHAPE_FTR_POINTER_FUNC	"deleteShapeFeaturePtr"

#define MDT_FOPEN_MODE  "MDT_OPEN_MODE"

/**
@brief Defines the log file name
*/
#define LOG_FILE_NAME           "LogFile"

/**
@brief Defines the log level
*/
#define LOG_LEVEL               "LogLevel"

#define DEFAULT_LOG_FILE        "lipi.log"

#define DEFAULT_LOG_LEVEL       LTKLogger::LTK_LOGLEVEL_ERR

#define LOG_LEVEL_DEBUG         "DEBUG"

#define LOG_LEVEL_ERROR         "ERR"

#define LOG_LEVEL_INFO          "INFO"

#define LOG_LEVEL_ALL           "ALL"

#define LOG_LEVEL_VERBOSE       "VERBOSE"

#define LOG_LEVEL_OFF           "OFF"

#define MDT_UPDATE_FREQUENCY "NNMDTFileUpdateFreq"
/**
  @brief Used in computation of Confidence
*/
#define MIN_NEARESTNEIGHBORS 2

#define EUCLIDEAN_FILTER_OFF -1

#define CONF_THRESHOLD_FILTER_OFF 0

#define NUM_CHOICES_FILTER_OFF -1

#define LTK_START_SHAPEID 0

#define ADAPT_SCHEME	"AdaptScheme"

#define NAME_ADD_LVQ_ADAPT_SCHEME "AddLVQ"

#define MAX_PROTOTYPES "MaxNumPrototypes"

#define ADAPT_CONF_LOWER_BOUND "ConfidenceLowerBound"

#define ADAPT_CONF_UPPER_BOUND "ConfidenceUpperBound"

#define ADAPT_MISMATCH_HIGH_CONF "MismatchHighConfidence"

#define ADAPT_MISMATCH_LOW_CONF "MismatchLowConfidence"

#define ADAPT_MATCH_HIGH_CONF "MatchHighConfidence"

#define ADAPT_MATCH_LOW_CONF "MatchLowConfidence"

#define ADAPT_MIN_NUMBER_SAMPLES_PER_CLASS "MinimumNumberOfSamplesPerClass"
#define DEFAULT_PROFILE        "default"

#define DEFAULT_CHANNEL_NAME	"X"

#define DEFAULT_DATA_TYPE		DT_FLOAT

#define X_CHANNEL_NAME          "X"

#define Y_CHANNEL_NAME			"Y"

#define DEFAULT_SAMPLING_RATE   100

#define DEFAULT_X_DPI           2000

#define DEFAULT_Y_DPI           2000

#define DEFAULT_LATENCY         0.0

#define DEFAULT_ERROR_CODE      -1

#define LIPIENGINE_CFG_STRING  "lipiengine.cfg"

#define DEFAULT_SHAPE_RECO_CHOICES 5

#define DEFAULT_SHAPE_RECO_MIN_CONFID 0.0

#define EMPTY_STRING " "

#define MDT_FILE_OPEN_MODE "NNMDTFileOpenMode"

#define MIN_PERCENT_EIGEN_ENERGY 1

#define MAX_PERCENT_EIGEN_ENERGY 100

#define MIN_EIGENSPREADVALUE 1

#endif // #ifndef __COMMONMACORS_H

