/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
* Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
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
 * $LastChangedDate: 2008-02-29 15:07:25 +0530 (Fri, 29 Feb 2008) $
 * $Revision: 437 $
 * $Author: sharmnid $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR: Definitions for HMM Shape Recognition module
 *
 * CONTENTS:
 *
 * AUTHOR:     Saravanan R.
 *
 * DATE:       January 23, 2007
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#ifndef __HMMSHAPERECOGNIZER_H
#define __HMMSHAPERECOGNIZER_H

/**			Include files		*/
#include "LTKInc.h"
#include "LTKTypes.h"
#include "LTKMacros.h"
#include "LTKShapeRecognizer.h"
#include "LTKShapeRecoUtil.h"
#include "LTKStringUtil.h"
#include "LTKShapeSample.h"
#include "LTKOSUtil.h"
#include "LTKOSUtilFactory.h"
#include "FSN.h"
#include "Decoder.h"


/**			Forward declaration of classes		*/
class LTKTraceGroup;
class LTKLoggerInterface;
class LTKPreprocessorInterface;
class LTKShapeSample;
class LTKShapeFeatureExtractor;
class LTKShapeFeature;


#define SUPPORTED_MIN_VERSION	"2.0.0"

#define HMM_DEF_PREPROC_SEQ "{CommonPreProc::normalizeSize,CommonPreProc::resampleTraceGroup,CommonPreProc::normalizeSize}"
#define HMM_DEF_FEATURE_EXTRACTOR "NPenShapeFeatureExtractor"
#define HMM_DEF_REJECT_THRESHOLD 0.0


typedef int (*FN_PTR_CREATELTKLIPIPREPROCESSOR)(const LTKControlInfo& , LTKPreprocessorInterface** );
typedef int (*FN_PTR_DELETELTKLIPIPREPROCESSOR)(LTKPreprocessorInterface* );


/**
 * @ingroup HMMShapeRecognizer.h
 * @brief The Header file for the HMMShapeRecognizer
 * @class HMMShapeRecognizer
 *<p> <p>
 */

class HMMShapeRecognizer: public LTKShapeRecognizer
{

        /** @name private data members */
        //@{
    private:

        FN_PTR_DELETELTKLIPIPREPROCESSOR m_deleteLTKLipiPreProcessor;
        //Function pointer for deleteLTKLipiPreProcessor

        unsigned short m_numShapes;
        /**< @brief Number of shapes
         *     <p>
         *     It Defines the number of shapes to be recognized
         *
         *	DEFAULT: 0
         *
         *	Note: If the project is dynamic, then this numShapes was set to 0
         *		  If the project is not dynamic, then the numShapes was read from project configuration file
         *	</p>
         */

        unsigned short m_traceDimension;
        /**< @brief Trace Dimension
         *	<p>
         *	The number of points after resampling the character
         *
         * 	DEFAULT: LTKPreprocDefaults::PREPROC_DEF_TRACE_DIMENSION
         * </p>
         */
      
        string m_featureExtractorName;
        /**< @brief The Feature Extractor
         * <p>
         *
         *	DEFAULT:
         *
         * </p>
         */

        bool m_projectTypeDynamic;
        /**< @brief Project Dynamic
         * 	<p>
         * 	if projectTypeDynamic  	= true, then the project is dynamic ie, the numShapes can take any number of value
         *		         	= false, then the project is not dynamic ie, the numShape can take value specified in project.cfg file
         *
         *	DEFAULT: false
         * 	</p>
         */

        LTKPreprocessorInterface *m_ptrPreproc;
        /**< @brief Pointer to preprocessor instance
         *	<p>
         *	Instance which is used to call the preprocessing methods before recognition
         *
         *	DEFAULT: NULL
         *	</p>
         */

        string m_hmmCfgFilePath;
        /**< @brief Full path of HMM configuration file
         * 	<p>
         *	Assigned value in the HMMShapeRecognizer::initialize function
         * 	</p>
         */

        string m_hmmMDTFilePath;
        /**< @brief Full path of Model data file
         *	<p>
         *   	Assigned value in the HMMShapeRecognizer::initialize function
         * 	</p>
         */


		string m_hmmModelsListFilePath;


		string m_hmmDictionaryFilePath;


		string m_hmmNetworkFilePath;


		FSN* m_ptrRecoNetwork;

		Decoder* m_ptrViterbiDecoder;

         LTKShapeRecoUtil m_shapeRecUtil;
        /**< @brief Pointer to LTKShapeRecoUtil class
         *	<p>
         *	Instance which used to call Shape Recognizer Utility functions
         *
         *	DEFAULT: NULL
         */

        

        string m_lipiRootPath;
        /**< @brief Path of the Lipi Root
         *	<p>
         *	DEFAULT: LipiEngine::getLipiPath()
         *	</p>
         */

        LTKShapeFeatureExtractor *m_ptrFeatureExtractor;
        /**< @brief Pointer to LTKShapeFeatureExtractor class
         *	<p>
         *	DEFAULT: NULL
         *	</p>
         */


		 /**< @brief Pointer to LTKOSUtil interface
         *	<p>
         *
         *	</p>
         */
        LTKOSUtil*  m_OSUtilPtr;


		// preproc lin handle
        void *m_libHandler;

        // feature extractor lin handle
        void *m_libHandlerFE;



        string m_preProcSeqn;
        /**< @brief Preprocessor Sequence
         *	<p>
         *	This string will holds what sequence the preprocessor methods to be executed
         *	</p>
         */

     
        LTKCaptureDevice m_captureDevice;


        vector<stringStringPair> m_preprocSequence;

  
  
        //Cache Parameters used by Adapt
        vector<LTKShapeRecoResult> m_vecRecoResult;
        /**< @brief Store Recognize results 
         *	 used by subsequent call to Adapt
         *	<p>
         *	
         *
         *	</p>
         */

        float m_rejectThreshold;
        /**< @brief Threshold on the confidence to reject a test sample
         *	<p>
         *
         *	</p>
         */

      //@}

        string m_currentVersion;

    public:

        /** @name Constructors and Destructor */
        //@{

        HMMShapeRecognizer(const LTKControlInfo& controlInfo);

        /**
         * Destructor
         */
        ~HMMShapeRecognizer();

       
        int train(const string& trainingInputFilePath,
                const string& mdtHeaderFilePath, 
                const string &comment,const string &dataset, 
                const string &trainFileType=INK_FILE) ;

        /**
         * This method loads the Training Data of the HMM classifier.
         * @param
         * @return LTKInc::SUCCESS : if the model data was loaded successfully
         * @exception
         */
        int loadModelData();

        /**
         * This method unloads all the training data
         * @param none
         * @return LTKInc::SUCCESS : if the model data was unloaded successfully
         * @exception none
         */
        int unloadModelData();

        /**
         * This method sets the device context for the recognition
         *
         * @param deviceInfo The parameter to be set
         * @return
         * @exception
         */
        int setDeviceContext(const LTKCaptureDevice& deviceInfo);

      
        int recognize(const LTKTraceGroup& traceGroup, 
                const LTKScreenContext& screenContext,
                const vector<int>& subSetOfClasses, 
                float confThreshold,
                int  numChoices, 
                vector<LTKShapeRecoResult>& outResultVector);

		int recognize(const vector<LTKShapeFeaturePtr>& shapeFeatureVector,
			const vector<int>& subSetOfClasses,
			float confThreshold,
			int  numChoices,
			vector<LTKShapeRecoResult>& outResultVector);
     

    private:
        
        /**
         * This method do the map between the module name and function names from the cfg file
         *
         * Semantics
         *
         *	- Read the Preprocess Sequence from the hmm.cfg
         *
         *	- Split the sequence into tokens with delimiter LTKMacros::DELEMITER_SEQUENCE using LTKStringUtil::tokenizeString
         *
         *	- Split each token with delimiter LTKMacrosDELEMITER_FUNC using LTKStringUtil::tokenizeString
         *		to get the Module name and Function name
         *
         *	- Store the Module name and the Function name into a structure
         *
         *
         * @param none
         * @return LTKInc::SUCCESS		: if functions are successfully mapped,
         * @return errorCodes	: if contains any errors
         * @exception none
         */
        int mapPreprocFunctions();

        /**
         * This method will assign default values to the members
         *
         * Semantics
         *
         *	- Assign Default values to all the data members
         *
         *
         * @param  none
         *
         * @return none
         */
        void assignDefaultValues();

        /** Reads the hmm.cfg and initializes the instance variable of the classifier with the user defined
         * values.
         *
         * Semantics
         *
         *	- Open the hmm.cfg using LTKConfigFileReader
         *
         *	- Incase of file open failure (hmm.cfg), default values of the classifier parameters are used.
         *
         *	- The valid values of the classifier parameters are cached in to the class data members.
         *        LTKConfigFileReader::getConfigValue is used to get the value fora key defined in the config file
         *
         *	- Exception is thrown if the user has specified an invalid valid for a parameter
         *
         *
         * @param  none
         * @return SUCCESS   : If the Config file read successfully
         * @return errorCode : If it contains some errors
         * @exception LTKErrorList::ECONFIG_FILE_RANGE			The config file variable is not within the correct range
         */
        int readClassifierConfig();

       

        /**
         * This method creates a custom feature extractor instance and stores it's address in
         * HMMShapeRecognizer::m_ltkFE. The local distance function pointer is also initialized.
         *
         * Semantics
         *
         *
         *	- Intialize the HMMShapeRecognizer::m_ptrFeatureExtractor with address of the feature extractor instance created
         *        using LTKShapeFeatureExtractorFactory::createFeatureExtractor
         *
         *	- Cache the address of  LTKShapeFeatureExtractor::getLocalDistance() in an instance variable
         *
         * @param none
         *
         * @return 0 on LTKInc::SUCCESS and 1 on LTKInc::FAILURE
         *
         * @exception none
         */
        int initializeFeatureExtractorInstance(const LTKControlInfo& controlInfo);


        int preprocess (const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outPreprocessedTraceGroup);

  

        int mapFeatureExtractor();

        int deleteFeatureExtractorInstance();
        /**
         * This method extracts shape features from given TraceGroup
         *
         * Semantics
         *
         *	- PreProcess tracegroup
         *	- Extract Features
         *
         * @param inTraceGroup	: LTKTraceGroup : Holds TraceGroup of sample
         *
         * @return SUCCESS:  if shapeFeatures is populated successfully
         *         FAILURE:  return ErrorCode
         * @exception none
         */

        int extractFeatVecFromTraceGroup(const LTKTraceGroup& traceGroup, 
                vector<LTKShapeFeaturePtr>& featureVec);

    

        /** This method is used to initialize the PreProcessor
         *
         * Semantics
         *
         * 	- Load the preprocessor DLL using LTKLoadDLL().
         *
         * 	- Get the proc address for creating and deleting the preprocessor instance.
         *
         * 	- Create preprocessor instance.
         *
         * 	- Start the logging for the preprocessor module.
         *	
         * @param  preprocDLLPath : string : Holds the Path of the Preprocessor DLL, 
         * @param  errorStatus    : int    : Holds SUCCESS or Error Values, if occurs
         * @return preprocessor instance
         *
         * @exception	ELOAD_PREPROC_DLL			Could not load preprocessor DLL
         * @exception	EDLL_FUNC_ADDRESS_CREATE	Could not map createPreprocInst
         * @exception	EDLL_FUNC_ADDRESS_DELETE	Could not map destroyPreprocInst
         */
        int initializePreprocessor(const LTKControlInfo& controlInfo,
                LTKPreprocessorInterface** preprocInstance);


        /** This method is used to deletes the PreProcessor instance
         *
         * Semantics
         *
         * 	- Call deleteLTKPreprocInst from the preproc.dll.
         *
         * 	- Unload the preprocessor DLL.
         *
         * @param  ptrPreprocInstance : Holds the pointer to the LTKPreprocessorInterface 
         * @return none
         * @exception none
         */

        int deletePreprocessor();

        /** This method is used to Unloads the preprocessor DLL.
         *
         * Semantics
         *
         * 	- If m_libHandler != NULL, unload the DLL
         *		LTKUnloadDLL(m_libHandler);
         *		m_libHandler = NULL;
         *	
         * @param none
         * @return none
         * @exception none
         */
        int unloadPreprocessorDLL();

};


#endif
