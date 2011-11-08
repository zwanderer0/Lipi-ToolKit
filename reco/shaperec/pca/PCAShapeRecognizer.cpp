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
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Implementation for PCA Shape Recognition module
 *
 * CONTENTS:
 *  loadTrainingData
 *  recognize
 *  learn
 *  train
 *  jacobi
 *  getPointAllocStats
 *  resampleTraceGroup
 *  preprocess
 *
 * AUTHOR:     Balaji R.
 *
 * DATE:       December 23, 2004
 * CHANGE HISTORY:
 * Author       Date            Description of change
 ************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include "LTKTypes.h"

#include "LTKShapeRecognizer.h"

#include "PCAShapeRecognizer.h"

#include "LTKConfigFileReader.h"

#include "LTKStringUtil.h"

#include "LTKInkFileReader.h"

#include "LTKLoggerUtil.h"

#include "LTKException.h"

#include "LTKPreprocDefaults.h"

#include "LTKErrors.h"

#include "LTKErrorsList.h"

#include "LTKPreprocessor.h"

#include "LTKTraceGroup.h"

#include "LTKShapeRecoResult.h"

#include "LTKShapeRecoConfig.h"

#include "LTKShapeRecoEngine.h"

#include "LTKTrace.h"

#include "lipiengine.h"

#include "LTKShapeRecoUtil.h"

#include "LTKShapeFeatureExtractor.h"

#include "LTKShapeFeature.h"

#include "LTKShapeSample.h"

#include "LTKShapeFeatureExtractorFactory.h"

#include "LTKVersionCompatibilityCheck.h"

#include "LTKLoadLibraryFailedException.h"

extern void *m_libHandlerFE;

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : PCAShapeRecognizer
 * DESCRIPTION   : Initialization 
 * ARGUMENTS     : 
 * RETURNS       : 
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 * VIJAYAKUMAR		27-JUL-2005			Added code to Intialization Modile for loading dlls.
 * Thanigai			29-JUL-2005			Modified the function to compose m_pcaCfgFile & 
 *										m_referenceModelFile variables. Also reading the 
 *										project and profile config files
 *
 * Saravanan R		22-Jan-2007			Made the isProjectDynamic and 
 *										initializePreprocessor functions as a generic
 *************************************************************************************/
PCAShapeRecognizer::PCAShapeRecognizer(LTKControlInfo controlInfo):
	m_ltkPreprocPtr(NULL),
	m_reverseFeatureVectors(PREPROC_DEF_REVERSE_FEATURE_VECTORS),
	m_traceDimension(PREPROC_DEF_TRACE_DIMENSION),
	m_numEigenVectors(PREPROC_DEF_NUMEIGEN_VECTORS),
	m_distanceMeasure(PREPROC_DEF_DISTANCEMEASURE),
	m_featureExtractor(PREPROC_DEF_FEATURE_EXTRACTOR),
	m_ptrFeatureExtractor(NULL),
	m_ptrShapeRecUtil(new LTKShapeRecoUtil()),
	m_projectTypeDynamic(false),
	deleteLTKLipiPreProcessor(NULL)
{

	xChannelstr = "X";
	yChannelstr = "Y";

	if( (controlInfo.projectName).empty() )
	{
		throw LTKException(EINVALID_PROJECT_NAME);
	}
	if( (controlInfo.lipiRoot).empty() )
	{
		throw LTKException(ELIPI_ROOT_PATH_NOT_SET);
	}

	if( (controlInfo.profileName).empty() )
	{
		(controlInfo.profileName) = DEFAULT_PROFILE;
	}

    if ( controlInfo.toolkitVersion.empty() )
        throw LTKException(ENO_TOOLKIT_VERSION);
	
	m_lipiRootPath = controlInfo.lipiRoot;
	m_currentVersion = controlInfo.toolkitVersion;
	string tempStr = "";
	string preprocDllPath = "";
	stringStringMap projectCfgAttrs;  //  contains details for configuration of the shape recognizer
	int returnStatus = 0;
	string strNumShapes = "";

  	m_pcaCfgFile = m_lipiRootPath + PROJECTS_PATH_STRING +  
                   (controlInfo.projectName) + PROFILE_PATH_STRING +
                   (controlInfo.profileName) + SEPARATOR + 
                   PCA + CONFIGFILEEXT;


	// construct the pca.mdt path using project and profile name
    m_referenceModelFile = m_lipiRootPath + PROJECTS_PATH_STRING +  
                          (controlInfo.projectName) + PROFILE_PATH_STRING +
                          (controlInfo.profileName) + SEPARATOR + 
                          PCA + DATFILEEXT;

	m_headerInfo[PROJNAME].assign((controlInfo.projectName).begin(),(controlInfo.projectName).end());

	
	// Read number of shapes from project.cfg
	tempStr = m_lipiRootPath + PROJECTS_PATH_STRING + (controlInfo.projectName) + PROFILE_PATH_STRING + PROJECT_CFG_STRING;

	try
	{	//To find whether the project was dynamic or not
		//to read read number of shapes from project.cfg
		m_projectTypeDynamic = m_ptrShapeRecUtil->isProjectDynamic(tempStr, m_numShapes, strNumShapes);

		m_headerInfo[NUMSHAPES] = strNumShapes;

		preprocDllPath = m_lipiRootPath + SEPARATOR + "lib" + SEPARATOR + PREPROC + DLL_EXT;

		//Currently preproc cfg also present in PCA
		controlInfo.cfgFileName = PCA;
		
		// initialize preprocessor this has to change
		m_ltkPreprocPtr = initializePreprocessor(preprocDllPath, controlInfo);

		//Reading pca configuration file
		returnStatus = readClassifierConfig();
		
		m_featureExtractor = m_headerInfo[FE_NAME];

		initializeFeatureExtractorInstance(controlInfo);
	}
	catch(LTKException e)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  e.getExceptionMessage() <<endl;
		throw e;
	}

}



/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : loadModelData
 * DESCRIPTION   : loads the reference model file into memory
 * ARGUMENTS     : 
 * RETURNS       : SUCCESS on successful loading of the reference model file
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 * VIJAYAKUMAR							Changed the function name from LoadTrainingData
 *										to LoadModelData
 *************************************************************************************/
int PCAShapeRecognizer::loadModelData()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::loadTrainingData" <<endl;

	unsigned short numShapes;           // number of shapes in the recognition problem

	unsigned short traceDimension;      // number of points obtained from a shape

	unsigned short numEigenVectors;     // number of eigen vectors used model each shape

	int shapeIndex;                     //  variable to loop over all the shapes

	int eigenIndex;                     //  variable to loop over all the eigenvectors

	int traceIndex;                     //  variable to loop over all the dimensions of the vector

	unsigned short momentDimension;     //  number of moment dimensions (not used anymore)

	unsigned short velocityDimension;   //  number of velcity dimensions (not used anymore)

	float eigenVectorComponent;         //  one of the dimensions of an eigenvector

	float eigenValue;                   //  one eigen value

	string inFilePath;                  //  name of the dat file to be read

	floatVector eigenVector;            //  an eigenvector

	float2DVector modelParamsVec;   //  all the eigenvectors of a particular shape

	floatVector eigenValuesVec;            //  all the eigenvalues of a particular shape

	PCAShapeModel shapeModel;               //  shape-eigenvectors storage type

	string algoVersionReadFromMDT;					//Algorithm version.

	int shapeID;							//ID of the class

	char currentVersion[VERSION_STR_LEN];
	string version;
	int iMajor, iMinor, iBugfix;

	stringStringMap headerSequence;

	if((errorCode = cheSumGen.readMDTHeader(m_referenceModelFile,headerSequence)) != 0)
	{
		return errorCode;
		//throw LTKException("ModelData File has been corrupted" + m_referenceModelFile);
	}
/*
	m_featureExtractor = "";
	m_featureExtractor = headerSequence[FE_NAME];

	string featureExtractor = headerSequence[FE_NAME];
	if(LTKSTRCMP(m_featureExtractor.c_str(), featureExtractor.c_str()) != 0)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<< "Value of FeatureExtractor parameter in config file ("<<m_featureExtractor<<") does not match with the value in MDT file ("<<featureExtractor<<")"<<endl;
		LTKReturnError(ECONFIG_MDT_MISMATCH);
	}

	string feVersion = headerSequence[FE_VER];
	try
	{
		initializeFeatureExtractorInstance();
	}
	catch(LTKException e)
	{
		LTKReturnError(e.getErrorCode());
	}*/


	// Version comparison START
	algoVersionReadFromMDT = headerSequence[RECVERSION].c_str();

	
	LTKVersionCompatibilityCheck verTempObj;
	string supportedMinVersion(SUPPORTED_MIN_VERSION);
	string currentVersionStr(m_currentVersion);

	bool compatibilityResults = verTempObj.checkCompatibility(supportedMinVersion, 
			currentVersionStr, 
			algoVersionReadFromMDT);

	if(compatibilityResults == false)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Incompatible version" <<endl;

		LTKReturnError(EINCOMPATIBLE_VERSION);			
	}
	// Version comparison END


	//  opening the dat file

	ifstream infile(m_referenceModelFile.c_str(),ios::in | ios::binary);

	if(!infile)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Unable to open model data file : " + m_referenceModelFile <<endl;

		LTKReturnError(EMODEL_DATA_FILE_OPEN);
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Reference model file read" <<endl;

	infile.seekg(atoi(headerSequence[HEADERLEN].c_str()),ios::beg);    

	//  reading reference model file header

	infile.read((char*) &numShapes, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "number of shapes read from reference model file = "<< numShapes <<endl;

	if(!m_projectTypeDynamic && m_numShapes != numShapes)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<< "Value of NumShapes parameter in config file ("<<m_numShapes<<") does not match with the value in MDT file ("<<numShapes<<")"<<endl;
		LTKReturnError(ECONFIG_MDT_MISMATCH);
	}

	/*
	if(!m_projectTypeDynamic && m_numShapes != numShapes)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Invalid value for number of shapes" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "NumShapes from cofiguration file ( or default value ) : " << m_numShapes <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "NumShapes from model data file : " << numShapes <<endl;

		LTKReturnError(EINVALID_NUM_OF_SHAPES);
	}
*/
	if(m_projectTypeDynamic)
	{
		m_numShapes = numShapes;
	}

	infile.read((char*) &traceDimension, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "trace dimension read from reference model file = " << traceDimension <<endl;

	if(m_traceDimension !=  traceDimension)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<< "Value of TraceDimension parameter in config file ("<<m_traceDimension<<") does not match with the value in MDT file ("<<traceDimension<<")"<<endl;
		LTKReturnError(ECONFIG_MDT_MISMATCH);
	}


/*	
	if(m_traceDimension != traceDimension)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Invalid value for trace dimension" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "TraceDimension from cofiguration file ( or default value ) : " << m_traceDimension <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "TraceDimension from model data file : " << traceDimension <<endl;

		LTKReturnError(EINVALID_TRACE_DIMENTION);
	}
*/
	infile.read((char*) &momentDimension, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "momentDimension = " << momentDimension <<endl;

	infile.read((char*) &velocityDimension, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "velocityDimension = " << velocityDimension <<endl;

	infile.read((char*) &numEigenVectors, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "number of eigen vectors read from reference model file = " << numEigenVectors <<endl;

	if(m_numEigenVectors != numEigenVectors)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<< "Value of NumEigenVectors parameter in config file ("<<m_numEigenVectors<<") does not match with the value in MDT file ("<<numEigenVectors<<")"<<endl;
		LTKReturnError(ECONFIG_MDT_MISMATCH);
	}


/*

	if(m_numEigenVectors != numEigenVectors)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Invalid value for eigen vector" <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "NumEigenVectors from config file ( or default value ): " << m_numEigenVectors <<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "NumEigenVectors from model data file: " << numEigenVectors <<endl;

		LTKReturnError(EINVALID_NUMEIGENVECTOR);
	}
	*/

	int floatSize = atoi(headerSequence[SIZEOFFLOAT].c_str());

	int intSize = atoi(headerSequence[SIZEOFINT].c_str());

	if(floatSize <= 0 )
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Invalid float size entry in model data File : " << floatSize <<endl;

		LTKReturnError(EINVALID_FLOAT_SIZE);	
	}

	//  reading eigenvectors of each of the shapes
	eigenVector.reserve(m_traceDimension*2);
	modelParamsVec.reserve(m_numEigenVectors);
	//	m_referenceModels.reserve(m_numShapes);

	for( shapeIndex = 0; shapeIndex < m_numShapes ; ++shapeIndex)
	{


		infile.read((char*)&shapeID,intSize);

		shapeModel.setShapeId(shapeID);



		for( eigenIndex = 0;eigenIndex < m_numEigenVectors; ++eigenIndex)
		{
			for( traceIndex = 0; traceIndex < (m_traceDimension*2) + 1;++traceIndex)
			{    
				infile.read((char*) &eigenVectorComponent, floatSize);

				eigenVector.push_back(eigenVectorComponent);

			}

			modelParamsVec.push_back(eigenVector);

			eigenVector.clear();
		}

		eigenValuesVec.clear();

		for( eigenIndex = 0;eigenIndex <= m_numEigenVectors; ++eigenIndex)
		{
			infile.read( (char*)&eigenValue, floatSize);

			eigenValuesVec.push_back(eigenValue);
		}

		shapeModel.setEigenValues(eigenValuesVec);

		shapeModel.setModel(modelParamsVec);

		modelParamsVec.clear();

		m_referenceModels.insert(make_pair(shapeID,shapeModel));
	}

	//  closing the reference model file and returning

	infile.close();

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::loadTrainingData" <<endl;

	return SUCCESS;

}

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : recognize
 * DESCRIPTION   : recognizes the incoming tracegroup
 * ARGUMENTS     : traceGroup - trace group to be recognized
 *                 uiParams - screen context
 *                 deviceParams - device information
 *                 shapeSubSet - A subset of the entire class space which is to be used for recognizing the input shape.
 *                 confThreshold - shapes with confidence below this threshold are rejected
 *                 numChoices - maximum number of choices to be returned
 *                 resultVec - result of recognition
 * RETURNS       : SUCCESS on successful running of the code
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/

int PCAShapeRecognizer::recognize(const LTKTraceGroup& traceGroup, const LTKScreenContext& uiParams, 
		 vector<int>& shapeSubSet, float confThreshold, 
		int  numChoices, vector<LTKShapeRecoResult>& resultVec)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::recognize" <<endl;

	int shapeIndex;                         //  variable to loop over all of the shapes

	int choiceIndex;                        //  variable to loop over all the recognition choices

	int featureIndex;                       //  variable to loop over all features

	float distance;                         //  pca distance

	float weightedDistance;                 //  pca distance weighted by the eigen values

	float dotProduct;                       //  dot product of the input with an eigenvector

	float revDistance;                      //  distance calculated with the reversed feature vector

	float weightedRevDistance;              //  weighted distance calculated with the reversed feature vector

	float revDotProduct;

	int shapeLabel;                         //  label assigned to the shapes

	float norm = 0.0;                       //  norm of the test sample

	float maximumDist = 0.0;                //  maximum distance in the choices list

	unsigned short featureDimension = 0;    //  number of feature dimensions

	intVector labelVec;                     //  list of current top choices                 

	floatVector distanceVec;                //  list of current top pca distances

	float distanceSum = 0;					// summation of distances of the returned choices

	float normalizedWeight = 0;				//	normalized weight for each eigen 

	float lastEigenValue;

	float firstEigenValue;

	LTKShapeRecoResult outResult;           //  recognition result holder

	LTKTraceGroup preprocessedTraceGroup;   //  pre-processed input

	PCAShapeModel shapeModel;               //  shape-eigenvectors storage space

	floatVector featureVector;              //  feature vector of the input

	//  iterators of the various vectors defined earlier

	floatVector::const_iterator featureVecIter;

	floatVector::reverse_iterator revFeatureVecIter;

	floatVector::const_iterator eigVecComponentsIter;

	floatVector::const_iterator featureVecEnd;

	vector<floatVector >::const_iterator eigVecIter;

	floatVector::const_iterator eigValIter;

	map<int,PCAShapeModel>::const_iterator shapeIter;

	vector<int>::iterator shapeSubsetVecIter; 

	//vector<PCAShapeModel>::const_iterator shapeIter;

	m_screenContext = uiParams;

	m_ltkPreprocPtr->setCaptureDevice(m_captureDevice);
	m_ltkPreprocPtr->setScreenContext(m_screenContext);

	if( preprocess(traceGroup, preprocessedTraceGroup) != SUCCESS)
	{
		return errorCode;
	}

	featureVector.clear();
	featureVector.push_back(1.00);

	vector<LTKShapeFeature*>* tempPtr = m_ptrFeatureExtractor->extractFeatures(preprocessedTraceGroup);

	m_ptrShapeRecUtil->shapeFeatureVectorToFloatVector((*tempPtr), featureVector);

	m_ptrFeatureExtractor->clearFeatureVector(tempPtr);


	if(featureVector.size() != (2*m_traceDimension +1) )
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The number of points in the tracegroup is not normalized by the preprocessing module (add resampling to the preprocessing module)"<<endl;

		LTKReturnError(EINVALID_NUM_OF_POINTS);		//return FAILURE;
	}

	//  initializing the top choice labels and pca distances

	for(choiceIndex = 0;choiceIndex <= numChoices; choiceIndex++)
	{
		distanceVec.push_back(-FLT_MAX);

		labelVec.push_back(-1);
	}


	//sorting and finding unique shape IDs. 
	//This helps since we can iterate the class ID - PCAShapeModel model synchronously
	//with the input shapeSubSet vector

	if(shapeSubSet.size()>0)
	{
		sort(shapeSubSet.begin(),shapeSubSet.end());
		unique(shapeSubSet.begin(),shapeSubSet.end());
	}



	//  computing the effective number of feature dimensions possible

	if(featureVector.size() != (2 * m_traceDimension + 1))
	{
		featureDimension = (featureVector.size() < (2 * m_traceDimension+1) ) ? featureVector.size(): (2 * m_traceDimension + 1);
	}
	else
	{
		featureDimension = featureVector.size();
	}

	shapeLabel = -1;


	//  calculating the norm of the test vector 

	for(featureIndex = 0; featureIndex < featureDimension ; featureIndex++)
	{
		norm += (featureVector.at(featureIndex) * featureVector.at(featureIndex));
	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "Square of the norm of the test vector = " << norm <<endl;

	//  calculating the distance of the test vector from each of the possible shapes

	shapeSubsetVecIter=shapeSubSet.begin();

	for(shapeIter = m_referenceModels.begin(); shapeIter != m_referenceModels.end();)
	{

		if(shapeSubSet.size()>0)
		{
			if((shapeIter->second).getShapeId()<(*shapeSubsetVecIter))
			{
				++shapeIter;
				continue;
			}
	
		}
		if(shapeSubSet.size()==0 || (shapeIter->second).getShapeId()==(*shapeSubsetVecIter))
		{
			
				shapeIndex = (shapeIter->second).getShapeId();

				const vector< floatVector >& modelParamsVec = (shapeIter->second).getModel();

				const floatVector& eigenValues = (shapeIter->second).getEigenValues();

				distance = weightedDistance = 0;

				revDistance = weightedRevDistance = 0;

				firstEigenValue = eigenValues.front();

				lastEigenValue = eigenValues.back();

				for( eigVecIter = modelParamsVec.begin(), eigValIter = eigenValues.begin(); eigVecIter < modelParamsVec.end(); ++eigVecIter, ++eigValIter)
				{
					const floatVector& eigenVecComponents = *eigVecIter;

					if(m_distanceMeasure == "normal")
					{
						normalizedWeight = 1;
					}
					else if(m_distanceMeasure == "weighted")
					{
						normalizedWeight = (*eigValIter)/eigenValues[0];
					}
					else
					{
						normalizedWeight = (*eigValIter)*norm/lastEigenValue;
					}

					if(normalizedWeight <EPS)
					{
						normalizedWeight = EPS;
					}

					/*  calculating the projection length of the test vector on the 
						jth eigenvector of shape i  */

					dotProduct = 0;

					revDotProduct = 0;

					featureVecEnd = featureVector.end();

					revFeatureVecIter = featureVector.rbegin();

					for(featureVecIter = featureVector.begin(), eigVecComponentsIter = eigenVecComponents.begin(); featureVecIter < featureVecEnd  ; ++featureVecIter, ++eigVecComponentsIter)
					{ 
						dotProduct += (*featureVecIter) * (*eigVecComponentsIter);

						revDotProduct += (*revFeatureVecIter) * (*eigVecComponentsIter);

						++revFeatureVecIter;
					}

					//distance of 'projection of the test vector on the subspace of shape i' from the 'origin'  

					distance += (dotProduct * dotProduct);

					weightedDistance += (dotProduct * dotProduct)/(normalizedWeight);

					revDistance += (revDotProduct * revDotProduct);

					weightedRevDistance += (revDotProduct * revDotProduct)/(normalizedWeight);

				}

				if(m_distanceMeasure != "normal")
				{
					normalizedWeight = (*eigValIter)/eigenValues[0];

					if(normalizedWeight < EPS)
					{
						normalizedWeight = EPS;
					}

					weightedDistance    += (norm - distance)/(normalizedWeight);

					weightedRevDistance += (norm - revDistance)/(normalizedWeight);

					LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "Square of the norm of the test vector projection on subspace of shape: "<< shapeIndex <<"dist:"<< distance <<endl;

					//  sorting the pca distances in the descending order

					if(weightedRevDistance < weightedDistance) 
					{
						weightedDistance = weightedRevDistance;
					}

					//The value in weighted Distance is a mahanalobis distance
					distance = -weightedDistance; 
				}
				else
				{
					if(distance < revDistance)
					{
						//The value in distance is actually a projection
						distance = revDistance;
					}
				}

				if(distanceVec.at(numChoices - 1) < distance)
				{
					for(choiceIndex = numChoices - 1; choiceIndex >= 0; --choiceIndex)
					{
						if(distanceVec.at(choiceIndex)<distance)
						{
							distanceVec.at(choiceIndex + 1) = distanceVec.at(choiceIndex);

							labelVec.at(choiceIndex + 1) = labelVec.at(choiceIndex);
						}

						else

						{
							distanceVec.at(choiceIndex + 1) = distance;

							labelVec.at(choiceIndex + 1) = shapeIndex;

							break;
						}

					}

					if(distanceVec.at(0) < distance)
					{
						distanceVec.at(0) = distance;

						labelVec.at(0) = shapeIndex;
					}

				}

				distanceSum += 1/(distance*distance);


				++shapeIter;
				++shapeSubsetVecIter; //updating the shapeSubSet iterator to move forward to the next shape ID
				if(shapeSubsetVecIter==shapeSubSet.end())
				{
					break;
				}

		}
		else if((shapeIter->second).getShapeId()>(*shapeSubsetVecIter))
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Invalid class ID in the shape subset vector:"<<(*shapeSubsetVecIter)<<endl;
			LTKReturnError(EINVALID_SHAPEID);

		}
	}

	if(shapeSubsetVecIter!=shapeSubSet.end())
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Invalid class ID in the shape subset vector:"<<(*shapeSubsetVecIter)<<endl;
		LTKReturnError(EINVALID_SHAPEID);
	}

	float normalizedConf;
	for(choiceIndex = 0; choiceIndex < numChoices; choiceIndex++)
	{

		if(m_distanceMeasure == "normal")
		{
			//pass norm
			normalizedConf = normalizeConf(norm, distanceVec.at(choiceIndex));
		}
		else
		{
			//pass distance sum
			normalizedConf = normalizeConf(distanceSum, distanceVec.at(choiceIndex));
		}
		//cout << "normalizedConf = " << normalizedConf <<" confThreshold = " << confThreshold <<endl;  
		if(normalizedConf > confThreshold)
		{
			outResult.setShapeId(labelVec.at(choiceIndex));
			outResult.setConfidence(normalizedConf);
			resultVec.push_back( outResult );
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "Choice index: " << choiceIndex <<" label:"<< labelVec.at(choiceIndex) <<endl;
		}
		else
		{
			break;
		}
	}

	distanceVec.clear();

	labelVec.clear();

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::recognize" <<endl;

	return SUCCESS;

}

/**********************************************************************************
 * AUTHOR		: Deepu V.
 * DATE			: 11-APR-2005
 * NAME			: getShapeRecoConfidences
 * DESCRIPTION	: calls the corresponding getShapeRecoConfidences method of the derived class
 *				  this abstract class is pointing to
 * ARGUMENTS		: traceGroup - trace group to be recognized
 *				  screenContext - screen context
 *				  captureDevice - device information
 *				  shapeIds - A subset of the entire shape space for which shape recongizer 
 *                            confidences need to be computed
 *				  shapeRecoConfidences - the confidence values calculated by the recognizer
 * RETURNS		: SUCCESS on successfully evaluating the confidences
 * NOTES			:
 * CHANGE HISTROY
 * Author			Date				Description of change
 *************************************************************************************/

int PCAShapeRecognizer::getShapeRecoConfidences(const LTKTraceGroup& traceGroup, 
		const LTKScreenContext& screenContext,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences)

{
	LTKTraceGroup preprocessedTraceGroup; //Pre processed trace group
	floatVector featureVector;              //  feature vector of the input
	vector<int>::const_iterator shapeIdIter; //iterator for iterating through the input shape subset

	float distance;                         //  pca distance
	float weightedDistance;                 //  pca distance weighted by the eigen values
	float dotProduct;                       //  dot product of the input with an eigenvector
	float revDistance;                      //  distance calculated with the reversed feature vector
	float weightedRevDistance;              //  weighted distance calculated with the reversed feature vector
	float revDotProduct;
	float norm = 0.0;                       //  norm of the test sample
	int featureIndex;                       //  variable to loop over all features
	unsigned short featureDimension = 0;    //  number of feature dimensions
	float normalizedWeight;					//	Normalized weight
	float lastEigenValue;

	//  iterators of the various vectors defined earlier
	floatVector::const_iterator featureVecIter;
	floatVector::reverse_iterator revFeatureVecIter;
	floatVector::const_iterator eigVecComponentsIter;
	floatVector::const_iterator featureVecEnd;
	vector<floatVector >::const_iterator eigVecIter;
	floatVector::const_iterator eigValIter;

	m_screenContext = screenContext;	
	LTKTraceGroup test(traceGroup);
	if(preprocess(test, preprocessedTraceGroup) != SUCCESS)
	{
		return errorCode;
	}

	featureVector.clear();
	featureVector.push_back(1.00);

	vector<LTKShapeFeature*>* tempPtr = m_ptrFeatureExtractor->extractFeatures(preprocessedTraceGroup);

	m_ptrShapeRecUtil->shapeFeatureVectorToFloatVector((*tempPtr), featureVector);

	m_ptrFeatureExtractor->clearFeatureVector(tempPtr);

	if(featureVector.size() != (2*m_traceDimension +1) )
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << 
			"The number of points in the tracegroup is not normalized by the preprocessing module (add resampling to the preprocessing module)" <<endl;

		LTKReturnError(EINVALID_NUM_OF_POINTS);		//return FAILURE;
	}
	//  computing the effective number of feature dimensions possible
	if(featureVector.size() != (2 * m_traceDimension + 1))
	{
		featureDimension = (featureVector.size() < (2 * m_traceDimension+1) ) ? featureVector.size(): (2 * m_traceDimension + 1);
	}
	else
	{
		featureDimension = featureVector.size();
	}

	//  calculating the norm of the test vector 
	for(featureIndex = 0; featureIndex < featureDimension ; featureIndex++)
	{
		norm += (featureVector.at(featureIndex) * featureVector.at(featureIndex));
	}


	for(shapeIdIter = shapeIds.begin(); shapeIdIter != shapeIds.end(); ++shapeIdIter)
	{

		if(m_referenceModels.find(*shapeIdIter)==m_referenceModels.end())
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "In PCAShapeRecognizer::getShapeRecoConfidences,"<<*shapeIdIter<<" is an invalid shape ID "<<endl;
			LTKReturnError(EINVALID_SHAPEID);

		}

		const vector< floatVector >& modelParamsVec = m_referenceModels[*shapeIdIter].getModel();
		const floatVector& eigenValues = m_referenceModels[*shapeIdIter].getEigenValues();

		distance = weightedDistance = 0;
		revDistance = weightedRevDistance = 0;

		lastEigenValue = eigenValues.back();

		for( eigVecIter = modelParamsVec.begin(), eigValIter = eigenValues.begin(); eigVecIter < modelParamsVec.end();
				++eigVecIter, ++eigValIter)
		{
			const floatVector& eigenVecComponents = *eigVecIter;

			/*  calculating the projection length of the test vector on the 
			    jth eigenvector of shape i  */
			if(m_distanceMeasure == "normal")
			{
				normalizedWeight = 1;
			}
			else if(m_distanceMeasure == "weighted")
			{
				normalizedWeight = (*eigValIter)/eigenValues[0];
			}
			else
			{
				normalizedWeight = (*eigValIter)*norm/lastEigenValue;
			}           

			dotProduct = 0;

			revDotProduct = 0;

			featureVecEnd = featureVector.end();

			revFeatureVecIter = featureVector.rbegin();

			for(featureVecIter = featureVector.begin(), eigVecComponentsIter = eigenVecComponents.begin(); featureVecIter < featureVecEnd  ; ++featureVecIter, ++eigVecComponentsIter)
			{ 
				dotProduct += (*featureVecIter) * (* eigVecComponentsIter);

				revDotProduct += (*revFeatureVecIter) * (* eigVecComponentsIter);

				++revFeatureVecIter;
			}

			//distance of 'projection of the test vector on the subspace of shape i' from the 'origin'  

			distance += (dotProduct * dotProduct);

			weightedDistance += (dotProduct * dotProduct)/normalizedWeight;

			revDistance += (revDotProduct * revDotProduct);

			weightedRevDistance += (revDotProduct * revDotProduct)/normalizedWeight;
		}

		if(m_distanceMeasure == "normal")
		{
			normalizedWeight = 1;
		}
		else if(m_distanceMeasure == "weighted")
		{
			normalizedWeight = (*eigValIter)/eigenValues[0];
		}
		else
		{
			normalizedWeight = (*eigValIter)*norm/lastEigenValue;
		}           

		weightedDistance    += (norm - distance)/normalizedWeight;
		weightedRevDistance += (norm - revDistance)/normalizedWeight;

		if(revDistance < distance)
		{
			distance = revDistance;
		}

		if(weightedRevDistance < weightedDistance)
		{
			weightedDistance = weightedRevDistance;
		}

		if(m_distanceMeasure == "normal")
		{
			distance = (distance/norm);
		}
		else if(m_distanceMeasure == "weighted")
		{
			distance = -weightedDistance*eigenValues[0];
		}
		else
		{
			distance = 1-weightedDistance;
		}           

		//saving the distance into the shape reco confidences
		shapeRecoConfidences.push_back(distance);

	}

	return SUCCESS;
}

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : learn
 * DESCRIPTION   : on the fly adaptation
 * ARGUMENTS     : prototype - prototype to be learned
 *                 shapeId - label of the prototype to be learned
 *                 uiParams - screen context
 *                 deviceParams - device information
 * RETURNS       : SUCCESS on successful learning
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/

int PCAShapeRecognizer::learn(const LTKTraceGroup& prototype, int shapeId, const LTKScreenContext& uiParams, 
		const LTKCaptureDevice& deviceParams)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::learn" <<endl;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::learn" <<endl;

	return SUCCESS;
}

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : train
 * DESCRIPTION   : offline training module
 * ARGUMENTS     : trainingList - name of the file containing the listing of the files to be used for
 *                 training each of the shapes
 * RETURNS       : SUCCESS on successful training
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 * Saravanan .R		17-01-07			Call a getAbsolutePath() function to convert 
 *										the Relative path to absolute path
 *************************************************************************************/

int PCAShapeRecognizer::trainFromListFile(const string& trainingList)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::train" <<endl;

	string path;                            //  file path of a ink file

	string line;							//	a line read from the list file

	vector<string> tokens;					//  line split in to tokens

	int shapeId;							//  shape id of the ink file

	int sampleCount = 0;                    //  count for the no. of samples read for a shape

	int shapeCount = 0;                     //  count of the no. of shape samples read so far

	int nrot = 0;                           //  number of iterations required for jacobi convergence

	int prevShape = -1;                      //  shape id of the previous shape

	int size;                               //  size of the covariance matrix

	int row;                                //  a row of the covariance matrix

	int column;                             //  a column of the covariance matrix

	unsigned short momentDimension = 0;     //  number of moment features (not used any more)

	unsigned short velocityDimension = 0;   //  number of velocity features (not used any more)

	floatVector eigenValueVec;              //  eigen values of the covariance matrix

	vector< floatVector > covarianceMatrix; // covariance matrix of the input samples of a shape

	vector< floatVector > eigenVectorMatrix;//  eigen vectors of the covariance matrix

	floatVector tempVec((2*m_traceDimension)+1, 0.0);// temporary vector to initialize other vectors

	covarianceMatrix.assign((2*m_traceDimension)+1, tempVec); //STL OPT

	eigenVectorMatrix.assign((2*m_traceDimension)+1, tempVec); //STL OPT

	// opening the file containing the training list of each of the shapes 

	ifstream listFileHandle(trainingList.c_str());

	//  throwing error if unable to open the training list file

	if(!listFileHandle)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open train list file : " << trainingList <<endl;

		LTKReturnError(ETRAINLIST_FILE_OPEN);	
	}

	//  opening the output reference model file

	ofstream mdtFileHandle(m_referenceModelFile.c_str(),ios::out|ios::binary);

	// throwing error if unable to open the reference model file

	if(!mdtFileHandle)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Unable to open train list file : "+ m_referenceModelFile <<endl;

		LTKReturnError(ETRAINLIST_FILE_OPEN);	
	}

	// writing the header information into the reference model file

	mdtFileHandle.write((char*) &m_numShapes,sizeof(unsigned short));

	if(!m_projectTypeDynamic)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_numShapes = " << m_numShapes <<endl;
	}

	mdtFileHandle.write((char*) &m_traceDimension,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_traceDimension = " << m_traceDimension <<endl;

	mdtFileHandle.write((char*) &momentDimension,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "momentDimension = " << momentDimension <<endl;

	mdtFileHandle.write((char*) &velocityDimension,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "velocityDimension = " << velocityDimension <<endl;

	mdtFileHandle.write((char*) &m_numEigenVectors,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_numEigenVectors = " << m_numEigenVectors <<endl;

	// reading the ink file paths from the training list file

	bool lastshapeIdFlag = false; // flag to skip reading a newline in the list file, when a new class starts 
	bool eofFlag = false; // flag is set when EOF is reached
	bool initClassFlag = false; // flag to check initial class

	/*try
	{
		initializeFeatureExtractorInstance();
	}
	catch(LTKException e)
	{
		LTKReturnError(e.getErrorCode());
	}*/

	while(!listFileHandle.eof())
	{
		if(lastshapeIdFlag == false) 
		{
			getline(listFileHandle,line,'\n');
			LTKStringUtil::tokenizeString(ine,  " \t\r",  tokens);

			if(listFileHandle.eof())  // check if EOF is reached
			{
				eofFlag = true;
			}

			if(tokens.size() > 0)
			{
				path = tokens[0];
			}
			// skip the remaining part of the loop if it is empty line or commented line
			// after reading the complete line, incase it is comment line, skip the loop
			// only if EOF is not yet reached
			if((tokens.size() == 0 || path[0] == COMMENTCHAR))
			{
				if(eofFlag == false)
				{
					continue;
				}
			}
			// read the shape id only if the line is not a comment line and
			// EOF is not reached
			if(tokens.size() == 2)
			{
				shapeId = atoi(tokens[1].c_str());
				if(shapeId < 0)
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The PCA Shape recognizer requires training file class Ids to be positive integers and listed in the increasing order"<<endl;

					LTKReturnError(EINVALID_SHAPEID); 
				}
				else if(shapeId < prevShape)
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Shape IDs in the train list file should be in the increasing order. Please use scripts/validateListFile.pl to generate list files." <<endl;

					LTKReturnError(EINVALID_ORDER_LISTFILE);
				}

				if(initClassFlag == false)
				{
					initClassFlag = true;
					prevShape=shapeId;

				}

			}
			else
			{
				if(eofFlag == false)
				{
					LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The training list file has an invalid line ... " << line <<endl;


					LTKReturnError(EINVALID_LINE_LISTFILE);
				}
			}

		}
		else // class change, do not read next line during this iteration
		{
			lastshapeIdFlag = false; //flag unset to read next line during the next iteration
		}

		// read the ink file and populate the covariance matrix
		if(shapeId == prevShape && path != "" && path[0] != COMMENTCHAR && tokens.size() > 0)
		{

			LTKTraceGroup inTraceGroup, outTraceGroup;			


			/* To get the Absolute Path from the ink file	*/					

			int error = m_ptrShapeRecUtil->getAbsolutePath(path, m_lipiRootPath);

			/* Print the Path name	*/

			cout << path << endl;

			// reading the ink file
			try
			{
				LTKInkFileReader::readUnipenInkFile(path, inTraceGroup, m_captureDevice, m_screenContext);
			}
			catch(LTKException e)
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  e.getExceptionMessage() <<endl;
				LTKReturnError(EINK_FILE_OPEN);
			}

			m_ltkPreprocPtr->setCaptureDevice(m_captureDevice);
			m_ltkPreprocPtr->setScreenContext(m_screenContext);

			// preprocessing the read trace group

			if( preprocess(inTraceGroup, outTraceGroup) != SUCCESS)
			{
				return errorCode;
			}

			// extracting features from the preprocessed trace group
			floatVector featureVector;

			featureVector.clear();
			featureVector.push_back(1.00);

			vector<LTKShapeFeature*>* tempPtr = m_ptrFeatureExtractor->extractFeatures(outTraceGroup);
			m_ptrShapeRecUtil->shapeFeatureVectorToFloatVector((*tempPtr), featureVector);


			m_ptrFeatureExtractor->clearFeatureVector(tempPtr);

			if(featureVector.size() != (2*m_traceDimension +1) )
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The number of points in the tracegroup is not normalized by the preprocessing module (add resampling to the preprocessing module)" <<endl;

				LTKReturnError(EINVALID_NUM_OF_POINTS);	//return FAILURE;
			}

			floatVector revFeatureVector;

			if(m_reverseFeatureVectors)
			{
				reverseFeatureVector(featureVector, revFeatureVector);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Reversing the feature vectors for training" <<endl;
			}

			++sampleCount;


			// updating the covariance matrix after each example of the shape is read 

			for(size = 0; size <= 2 * m_traceDimension; size++)
			{
				(covarianceMatrix.at(size)).at(size) += (featureVector.at(size)*featureVector.at(size));

				if(m_reverseFeatureVectors)
				{
					(covarianceMatrix.at(size)).at(size) += (revFeatureVector.at(size)*revFeatureVector.at(size));
				}
			}

			for(row =0; row <= 2 * m_traceDimension; row++)
			{
				for(column = row+1; column <= 2 * m_traceDimension; column++)
				{
					(covarianceMatrix.at(row)).at(column) +=  (featureVector.at(row)*featureVector.at(column));

					if(m_reverseFeatureVectors)
					{
						(covarianceMatrix.at(row)).at(column) +=  (revFeatureVector.at(row)*revFeatureVector.at(column));
					}
					(covarianceMatrix.at(column)).at(row) = (covarianceMatrix.at(row)).at(column);
				}
			}

			featureVector.clear();

			if(m_reverseFeatureVectors)
			{
				revFeatureVector.clear();
			}
		}

		// compute eigen vectors after all the samples of a class are read 	
		if(shapeId != prevShape || eofFlag == true)
		{

			if(sampleCount >0 )  // increase shape count only if there are atleast one sample per class
				shapeCount++;
			if(!m_projectTypeDynamic && shapeCount > m_numShapes)
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The training file contains more shapes than mentioned in the config file :"<< m_numShapes <<endl;
				LTKReturnError(EINVALID_NUM_OF_SHAPES); 
			}

			if(shapeCount > 0 && sampleCount >0)
			{  

				//writing class ID because the IDs need not be contiguous
				if(eofFlag==true)
				{
					mdtFileHandle.write((char *)&shapeId,sizeof(int));
				}
				else
				{
					mdtFileHandle.write((char *)&prevShape,sizeof(int));
				}


				//  complete covariance matrix computation by dividing with the number of samples

				for(row = 0; row <= (2 * m_traceDimension); row++)
				{
					for(column=0; column <= (2 * m_traceDimension); column++)
					{
						if(m_reverseFeatureVectors)
						{
							(covarianceMatrix.at(row)).at(column) /= (2 * sampleCount);
						}
						else
						{
							(covarianceMatrix.at(row)).at(column) /= sampleCount;
						}
					}
				}

				// computing the eigenvectors and eigenvalues of the covariance matrix

				computeEigenVectors(covarianceMatrix,(2 * m_traceDimension)+1, eigenValueVec, eigenVectorMatrix, nrot);

				// writing the eigenvectors into reference model file

				for(column = 0; column < m_numEigenVectors; ++column)
				{
					for(row = 0; row < (2 * m_traceDimension)+1; ++row)
					{
						mdtFileHandle.write((char *)&((eigenVectorMatrix.at(row)).at(column)),sizeof(float));
					}
				}   

				//  writing the eigen values of the covariance matrix
				//  one eigen value more !!!
				for(column = 0; column <= m_numEigenVectors; ++column)
				{
					mdtFileHandle.write((char *)&eigenValueVec.at(column),sizeof(float));
				}               

				eigenValueVec.clear();



			}

			// reinitializing covariance matrix for the next shape

			for(row = 0; row < 2 * m_traceDimension; row++)
			{
				for(column = 0; column <= 2 * m_traceDimension; column++)                 
				{
					(covarianceMatrix.at(row)).at(column) = 0;          
				}
			}
			// resetting sampleCount for the next class 
			sampleCount = 0;
			// set the flag so that the already read line of next class in the list file is not lost
			lastshapeIdFlag = true;
			// seting the next class shapeId
			prevShape=shapeId;
		}

	}


	//  closing the training list file and returning
	listFileHandle.close();

	if(!m_projectTypeDynamic && shapeCount != m_numShapes)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The training file contains less shapes than mentioned in the config file:" << m_numShapes<<endl;
		LTKReturnError(EINVALID_NUM_OF_SHAPES); 
	}

	if(m_projectTypeDynamic)
	{
		m_numShapes = shapeCount;
		mdtFileHandle.seekp(0,ios::beg);    
		mdtFileHandle.write((char*) &m_numShapes,sizeof(unsigned short));
	}
	mdtFileHandle.close();

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::train" <<endl;

	return SUCCESS;
}

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : computeEigenVectors
 * DESCRIPTION   : finds the eigenvalues and eigenvectors of the covariance matrices
 * ARGUMENTS     : covarianceMatrix - input covariance matrix whose eigen vectors/values are required
 *                 n - rank of the covariance matrix
 *                 eigenValueVec - computed eigen values held in this reference
 *                 eigenVectorMatrix - computed eigen vectors held in this reference
 *                 nrot - number of iterations required by the algorithm to converge
 * RETURNS       : SUCCESS on successful computation of eigen vectors/values
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/

int PCAShapeRecognizer::computeEigenVectors(vector< vector<float> > &covarianceMatrix, int n,
		vector<float> &eigenValueVec, vector< vector<float> > &eigenVectorMatrix, int& nrot)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::computeEigenVectors" <<endl;

	int MAX_ITER = 1000;

	int ip,count,p,q,r;

	double theta, tau, t,sm,s,h,g,c;

	double app,aqq,apq,vpp,vqq,vpq;

	vector< floatVector >::iterator  itrRow,itrRowEnd;

	floatVector::iterator itrCol,itrColEnd;

	itrRowEnd = eigenVectorMatrix.end();

	for(ip = 0,itrRow = eigenVectorMatrix.begin(); itrRow <itrRowEnd;++ip, ++itrRow)
	{
		itrColEnd = (*itrRow).end();

		for(itrCol = (*itrRow).begin(); itrCol < itrColEnd; ++itrCol)
		{
			*itrCol = 0;
		}

		*((*itrRow).begin() + ip) = 1;

		eigenValueVec.push_back(0.0);
	}

	nrot = 0;

	for(count = 0; count <MAX_ITER; count++)
	{
		nrot++;

		sm = 0.0;

		itrRowEnd = covarianceMatrix.end();

		for(p = 0,itrRow = covarianceMatrix.begin(); itrRow <itrRowEnd;++p, ++itrRow)
		{
			itrColEnd = (*itrRow).end();

			for(itrCol = (*itrRow).begin()+p+1; itrCol < itrColEnd; ++itrCol)
			{
				sm += fabs(*itrCol);
			}
		}

		if(sm < EPS)
		{
			for(r=0;r<n;r++)
			{
				eigenValueVec[r] = covarianceMatrix[r][r];
			}

			float temp;

			for(p = 0; p<n-1; p++)
			{
				for(q = p+1; q<n; q++)
				{
					if(fabs(eigenValueVec[p]) < fabs(eigenValueVec[q]))
					{
						for(r = 0; r<n; r++)
						{
							temp = eigenVectorMatrix[r][p];

							eigenVectorMatrix[r][p] = eigenVectorMatrix[r][q];

							eigenVectorMatrix[r][q] = temp;
						}

						temp = eigenValueVec[p];

						eigenValueVec[p] = eigenValueVec[q];

						eigenValueVec[q]= temp;
					}
				}

			}

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::computeEigenVectors" <<endl;

			return SUCCESS;
		}

		for(p=0; p<(n-1) ; p++)
		{
			for(q=(p+1); q<n; q++)
			{
				if(fabs(covarianceMatrix[p][q])>EPS1)
				{
					theta = (covarianceMatrix[q][q] - covarianceMatrix[p][p])/
						(2*covarianceMatrix[p][q]);

					t = sqrt(1+theta * theta) - theta;

					c = 1/(sqrt(1+t*t));

					s = t*c;

					tau = s/(1+c);

					apq = covarianceMatrix[p][q];

					app = covarianceMatrix[p][p];

					aqq = covarianceMatrix[q][q];

					vpp = eigenVectorMatrix[p][p];

					vpq = eigenVectorMatrix[p][q];

					vqq = eigenVectorMatrix[q][q];

					for(r=0;r<p;r++)
					{
						h = covarianceMatrix[r][p];

						g = covarianceMatrix[r][q];

						covarianceMatrix[r][p] = (float)(c*h - s*g);

						covarianceMatrix[r][q] = (float)(c*g + s*h);
					}
					covarianceMatrix[p][p] -= (float)(t*apq);

					covarianceMatrix[p][q] = (float)(0.0);

					for(r=p+1; r<q; r++)
					{
						h = covarianceMatrix[p][r];

						g = covarianceMatrix[r][q];

						covarianceMatrix[p][r] = (float)(c*h - s*g);

						covarianceMatrix[r][q] = (float)(c*g + s*h);
					}

					covarianceMatrix[q][q] += (float)(t*apq);

					for(r = q+1; r<n; r++)
					{
						h = covarianceMatrix[p][r];

						g = covarianceMatrix[q][r];

						covarianceMatrix[p][r] = (float)(c*h - s*g);

						covarianceMatrix[q][r] = (float)(c*g + s*h);
					}

					for(r = 0; r<n ; r++)
					{
						h = eigenVectorMatrix[r][p];

						g = eigenVectorMatrix[r][q];

						eigenVectorMatrix[r][p] = (float)(c*h - s*g);

						eigenVectorMatrix[r][q] = (float)(c*g + s*h);
					}
				}
				else
				{
					covarianceMatrix[p][q] = 0;
				}
			}
		}

	}

	LOG( LTKLogger::LTK_LOGLEVEL_INFO)<<  "Too Many Iterations for Jacobi Convergence" <<endl;

	for(r=0;r<n;r++)
	{
		eigenValueVec[r] = covarianceMatrix[r][r];
	}

	float temp;

	for(p = 0; p<n-1; p++)
	{
		for(q = p+1; q<n; q++)
		{
			if(fabs(eigenValueVec[p]) < fabs(eigenValueVec[q]))
			{
				for(r = 0; r<n; r++)
				{
					temp = eigenVectorMatrix[r][p];

					eigenVectorMatrix[r][p]  = eigenVectorMatrix[r][q];

					eigenVectorMatrix[r][q]  = temp;

				}

				temp = eigenValueVec[p];

				eigenValueVec[p] = eigenValueVec[q];

				eigenValueVec[q] = temp;

			}
		}
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::computeEigenVectors" <<endl;

	return SUCCESS;
}


/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : preprocess
 * DESCRIPTION   : calls the required pre-processing functions from the LTKPreprocessor library
 * ARGUMENTS     : inTraceGroup - reference to the input trace group
 *                 outTraceGroup - pre-processed inTraceGroup
 * RETURNS       : SUCCESS on successful pre-processing operation
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 * VIJAYAKUMAR							,....
 *************************************************************************************/

int PCAShapeRecognizer::preprocess(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::preprocess" <<endl;

	int indx = 0;

	string module, funName;

	LTKTraceGroup local_inTraceGroup;

	local_inTraceGroup = inTraceGroup;

	if(mapSequence.size() != 0)
	{
		while(indx < mapSequence.size())
		{
			module = mapSequence.at(indx).moduleName;
			funName =  mapSequence.at(indx).funcName;

			if(!module.compare("PCA"))
			{
				outTraceGroup.emptyAllTraces();

				if((errorCode = ((this->*(PreprocMap[funName]))(local_inTraceGroup,outTraceGroup))) != SUCCESS)
				{
					return errorCode;
				}

				local_inTraceGroup = outTraceGroup;
			}
			else if(!module.compare("CommonPreProc"))
			{
				FN_PTR_PREPROCESSOR pPreprocFunc = NULL;
				pPreprocFunc = m_ltkPreprocPtr->getPreprocptr(funName);
				if(pPreprocFunc!= NULL)
				{
					outTraceGroup.emptyAllTraces();
					if((errorCode = (m_ltkPreprocPtr->*(pPreprocFunc))(local_inTraceGroup,outTraceGroup)) != SUCCESS)
					{
						return errorCode;
					}

					local_inTraceGroup = outTraceGroup;

				}
			}
			indx++;
		}
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::preprocess" <<endl;

	return SUCCESS;

}

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : reverseFeatureVector
 * DESCRIPTION   : reverses the given feature vector to make it look as if the features are extracted from a 
 *                 from the same trace group written in the reverse direction
 * ARGUMENTS     : featureVector - reference to the feature vector to be reversed
 *                 revFeatureVector - reversed feature vector
 * RETURNS       : SUCCESS on successful reversing operation
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/

int PCAShapeRecognizer::reverseFeatureVector(floatVector& featureVector, floatVector& revFeatureVector)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::reverseFeatureVector" <<endl;

	int featureIndex;   //  variable to loop over the features of the feature vector

	//  copying the first feature

	revFeatureVector.push_back(featureVector[0]);

	//  reversing the x-coordinate features

	for(featureIndex = 1; featureIndex <= m_traceDimension; ++featureIndex)
	{
		revFeatureVector.push_back(featureVector[m_traceDimension - featureIndex + 1]);
	}

	//  reversing the y-coordinate features

	for(featureIndex = 1; featureIndex <= m_traceDimension; ++featureIndex)
	{
		revFeatureVector.push_back(featureVector[(2 * m_traceDimension) - featureIndex + 1]);
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer::reverseFeatureVector" <<endl;

	return SUCCESS;
}

/**********************************************************************************
 * AUTHOR        : Balaji R.
 * DATE          : 23-DEC-2004
 * NAME          : ~PCAShapeRecognizer
 * DESCRIPTION   : destructor
 * ARGUMENTS     : 
 * RETURNS       : 
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/

PCAShapeRecognizer::~PCAShapeRecognizer()
{
	deletePreprocessor(m_ltkPreprocPtr);

	delete m_ptrShapeRecUtil;

	unloadFeatureExtractorInstance();

}

/**********************************************************************************
 * AUTHOR		: 
 * DATE			: 
 * NAME			: unloadModelData
 * DESCRIPTION	: New Function - Not yet added
 * ARGUMENTS		: 
 * RETURNS		: 
 * NOTES			:
 * CHANGE HISTROY
 * Author			Date				Description of change
 *************************************************************************************/

int PCAShapeRecognizer::unloadModelData()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer : unloadModelData" <<endl;

	m_referenceModels.clear();
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer : unloadModelData" <<endl;

	return SUCCESS ;
}

/**********************************************************************************
 * AUTHOR		: 
 * DATE			: 
 * NAME			: setDeviceContext
 * DESCRIPTION	: New Function - Not yet added
 * ARGUMENTS		: 
 * RETURNS		: 
 * NOTES			:
 * CHANGE HISTROY
 * Author			Date				Description of change
 *************************************************************************************/

int PCAShapeRecognizer::setDeviceContext(LTKCaptureDevice& deviceinfo)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Entered PCAShapeRecognizer : setDeviceContext" <<endl;

	m_captureDevice = deviceinfo;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer : setDeviceContext" <<endl;

	return SUCCESS;
}

/**********************************************************************************
 * AUTHOR        : Vijayakumara M 
 * DATE          : 01-Aug-2005
 * NAME          : getLastError
 * DESCRIPTION   : returns last occured error description.
 * ARGUMENTS     : - NO -
 * RETURNS       : pointer to an error description 
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/
int PCAShapeRecognizer::getLastError()
{
	return errorCode;
}

/**********************************************************************************
 * AUTHOR        : Vijayakumara M 
 * DATE          : 20-Sept-2005
 * NAME          : mapPreprocFunctions
 * DESCRIPTION   : Maps the module name and its function names in the preprocessing 
 sequence.
 * ARGUMENTS     : - NO -
 * RETURNS       : SUCCESS on successful, errorNumbers on Failure.
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 *************************************************************************************/
int PCAShapeRecognizer::mapPreprocFunctions()
{
	stringStringMap preProcSequence;

	MapModFunc tmpMap;

	vector<string> moduleFuncNames;
	vector<string> modFuncs;
	vector<string> funcNameTokens;	

	string module="", funName="", sequence="";
	string::size_type indx;

	LTKTraceGroup local_inTraceGroup;

	LTKStringUtil::tokenizeString(m_preProcSeq,  DELEMITER_SEQUENCE,  funcNameTokens);

	if(funcNameTokens.size() == 0)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Wrong preprocessor sequence in cfg file : " + m_preProcSeq <<endl; 
		LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
	}

	for (indx = 0; indx < funcNameTokens.size(); indx++)
	{
		moduleFuncNames.push_back(funcNameTokens.at(indx));
	}

	for(indx=0; indx < moduleFuncNames.size(); indx++)
	{
		sequence = moduleFuncNames.at(indx);

		LTKStringUtil::tokenizeString(sequence,  DELEMITER_FUNC,  modFuncs);

		if(modFuncs.size() >= 2)
		{
			module = modFuncs.at(0);

			funName =  modFuncs.at(1);

			if(!module.compare("PCA"))
			{
				if(PreprocMap[funName] != NULL)
				{
					tmpMap.moduleName = module;
					tmpMap.funcName = funName;
					mapSequence.push_back(tmpMap);				
				}
				else
				{
					LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Wrong preprocessor sequence entry in cfg file " + funName <<endl; 

					LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
				}
			}
			else
			{
				if(!module.compare("CommonPreProc"))
				{
					FN_PTR_PREPROCESSOR pPreprocFunc = NULL;
					pPreprocFunc = m_ltkPreprocPtr->getPreprocptr(funName);
					if(pPreprocFunc!= NULL)
					{
						tmpMap.moduleName = module;
						tmpMap.funcName = funName;
						mapSequence.push_back(tmpMap);
					}
					else
					{
						LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Wrong preprocessor sequence entry in cfg file : " + funName <<endl; 

						LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
					}
				}
				else
				{
					LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Wrong preprocessor sequence entry in cfg file : " + module <<endl; 

					LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
				}
			}
		}
		else
		{
			LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Wrong preprocessor sequence entry in cfg file : " + module <<endl; 

			LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
		}
	}


	return SUCCESS;
}


/**********************************************************************************
 * AUTHOR        : Deepu V. 
 * DATE          : 24-Nov-2005
 * NAME          : normalizeConf
 * DESCRIPTION   : Remap the confidences to [0 1]
 * ARGUMENTS     : norm - used for normalizing when the distance measure is normal
 *               :      - used for normalizing when the distance measure is weighted
 * RETURNS       : SUCCESS on successful, errorNumbers on Failure.
 * NOTES         :
 * CHANGE HISTROY
 * Author            Date                Description of change
 Dinesh            3-Aug-2006          change in the function prototype 
 *************************************************************************************/
float PCAShapeRecognizer::normalizeConf(float normFact, float distance)
{
	float normConf;

	if(m_distanceMeasure == "normal")
	{
		normConf =  distance / normFact ;
	}
	else
	{
		normConf = ( (1.0/(distance*distance)) / normFact );
	}
	return normConf;
}

/**********************************************************************************
 * AUTHOR		: Saravanan R.
 * DATE			: 23-Jan-2007
 * NAME			: readClassifierConfig
 * DESCRIPTION	: To read the config files and to assign the values to datamembers
 * ARGUMENTS		: string contains the path of the config file
 * RETURNS		: 
 * NOTES			:
 * CHANGE HISTROY
 * Author			Date				Description
 *************************************************************************************/

int PCAShapeRecognizer::readClassifierConfig()
{
	string tempStringVar = "";
	int tempIntegerVar = 0;
	float tempFloatVar = 0;
	LTKConfigFileReader* shapeRecognizerProperties = NULL;
	int errorCode;

	try
	{
		shapeRecognizerProperties = new LTKConfigFileReader(m_pcaCfgFile);
	}
	catch(...)
	{
		cout<< "Config file not found, using default values of the parameters"<<endl;
		LOG(LTKLogger::LTK_LOGLEVEL_INFO)<< "Config file not found, using default values of the parameters"<<endl;
		if((errorCode = mapPreprocFunctions()) != SUCCESS)
		{
			deletePreprocessor(m_ltkPreprocPtr);
			return errorCode;
		}

		delete shapeRecognizerProperties;
		// We use the default values in this case
		return SUCCESS;
	}

	// Read preprocessing sequence
	m_preProcSeq = shapeRecognizerProperties->getConfigValue(PREPROCSEQUENCE);

	if(m_preProcSeq == "")
	{
		m_preProcSeq = PREPROC_DEF_SEQ_PCA;
	}

	if((errorCode = mapPreprocFunctions()) != SUCCESS)
	{
		deletePreprocessor(m_ltkPreprocPtr);
		return errorCode;
	}


	//  getting the preprocessing steps required
	try
	{

		tempStringVar = shapeRecognizerProperties->getConfigValue(REVERSEFEATUREVECTORS);
		if(tempStringVar == "true")
		{
			m_reverseFeatureVectors = true;

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Reverse feature vectors turned on" <<endl;
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(TRACEDIMENSION);
		if(tempStringVar != "")
		{
			tempIntegerVar = atoi((tempStringVar).c_str());

			if(tempIntegerVar >=10 && tempIntegerVar <= 500)
			{
				m_traceDimension = tempIntegerVar;

				LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "Trace dimension = " << m_traceDimension <<endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  " TRACEDIMENSION is out of permitted range" <<endl;

				throw LTKException(ECONFIG_FILE_RANGE);	
			}

		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(NUMEIGENVECTORS);
		if(tempStringVar != "")
		{
			tempIntegerVar = atoi((tempStringVar).c_str());
			if(tempIntegerVar > 0 )
			{
				m_numEigenVectors = tempIntegerVar;

				LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Number of eigenvectors = " << m_numEigenVectors <<endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  " TRACEDIMENSION is out of permitted range" <<endl;

				throw LTKException(ECONFIG_FILE_RANGE);	
			}

		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(DISTANCEMEASURE);
		if(tempStringVar != "")
		{

			m_distanceMeasure = tempStringVar;

			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "Distance Measure = " << tempStringVar <<endl;

		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(FEATUREEXTRACTOR);
		if(tempStringVar != "")
		{
			m_featureExtractor = tempStringVar;

			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Feature Extractor Method = " << tempStringVar <<endl;			
		}
		else
		{
			LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Feature Extractor was empty in config file" <<endl;

			throw LTKException(ENO_FTR_EXTR_IN_CFG);
		}
		
	}
	catch(LTKException e)
	{
		delete shapeRecognizerProperties;
		throw e;
	}
	delete shapeRecognizerProperties;
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::InitializePCAShapeRecognizer" <<endl;
	return SUCCESS;

}


/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 28-02-2007
 * NAME			: initializeFeatureExtractorInstance
 * DESCRIPTION	: This method get the Instance of the Feature Extractor 
 *				  from LTKShapeFeatureExtractorFactory
 * ARGUMENTS		: 
 * RETURNS		: none
 * NOTES			: 
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
void PCAShapeRecognizer::initializeFeatureExtractorInstance(LTKControlInfo controlInfo)
{
	int featureExtractor_Code = mapFeatureExtractor();

	LTKShapeFeatureExtractorFactory factory;
	try
	{		
		m_ptrFeatureExtractor = factory.createFeatureExtractor(featureExtractor_Code,m_lipiRootPath, &m_libHandlerFE,controlInfo);
		LTKShapeRecoUtil::m_deleteShapeFeatureFunc = (FN_PTR_DELETE_SHAPE_FTR_PTR) LTKGetDLLFunc(m_libHandlerFE, DELTE_SHAPE_FTR_POINTER_FUNC);

		if(m_ptrFeatureExtractor == NULL)
		{
			throw (LTKException(EFTR_EXTR_NOT_EXIST));
		}
	}
	catch(LTKLoadLibraryFailedException le)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << le.getExceptionMessage() <<endl;
		throw le;
	}
	catch(LTKException e)
	{
		throw e;
	}
}

/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 28-03-2007
 * NAME			: mapFeatureExtractor
 * DESCRIPTION	: 
 * ARGUMENTS		: 
 * RETURNS		: 
 * NOTES			: 
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
int PCAShapeRecognizer::mapFeatureExtractor()
{
	int returnCode = FAILURE;
	if(LTKSTRCMP(m_featureExtractor.c_str(), NAME_POINT_FLOAT_SHAPE_FEATURE_EXTRACTOR) == 0)
	{		
		returnCode = POINT_FLOAT_SHAPE_FEATURE_EXTRACTOR;
	}

	return returnCode;
}

/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 26-03-2007
 * NAME			: unloadFeatureExtractorInstance
 * DESCRIPTION	: This method unloads the Feature extractor instance
 * ARGUMENTS		: none
 * RETURNS		: none
 * NOTES			: 
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
void PCAShapeRecognizer::unloadFeatureExtractorInstance()
{
	if ( m_ptrFeatureExtractor != NULL )
	{
		typedef int (*FN_PTR_DELETE_SHAPE_FEATURE_EXTRACTOR)(LTKShapeFeatureExtractor *obj);
		FN_PTR_DELETE_SHAPE_FEATURE_EXTRACTOR deleteFeatureExtractor;

		deleteFeatureExtractor = (FN_PTR_DELETE_SHAPE_FEATURE_EXTRACTOR) LTKGetDLLFunc(m_libHandlerFE, DELETE_SHAPE_FEATURE_EXTRACTOR);

		if(!deleteFeatureExtractor)
		{
			throw(LTKException(EDLL_FUNC_ADDRESS_DELETE_FEATEXT));
		}

		deleteFeatureExtractor(m_ptrFeatureExtractor);

		m_ptrFeatureExtractor = NULL;
	}
}

/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 29-03-2007
 * NAME			: trainFromFeatureFile
 * DESCRIPTION	: 
 * ARGUMENTS		: none
 * RETURNS		: none
 * NOTES			: 
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
int PCAShapeRecognizer::trainFromFeatureFile(const string& featureFilePath)
{
	string line;							//	a line read from the list file

	int shapeId;							//  shape id of the ink file

	int sampleCount = 0;                    //  count for the no. of samples read for a shape

	int shapeCount = 0;                     //  count of the no. of shape samples read so far

	int nrot = 0;                           //  number of iterations required for jacobi convergence

	int prevShape = -1;                      //  shape id of the previous shape

	int size;                               //  size of the covariance matrix

	int row;                                //  a row of the covariance matrix

	int column;                             //  a column of the covariance matrix

	unsigned short momentDimension = 0;     //  number of moment features (not used any more)

	unsigned short velocityDimension = 0;   //  number of velocity features (not used any more)

	floatVector eigenValueVec;              //  eigen values of the covariance matrix

	vector< floatVector > covarianceMatrix; // covariance matrix of the input samples of a shape

	vector< floatVector > eigenVectorMatrix;//  eigen vectors of the covariance matrix

	floatVector tempVec((2*m_traceDimension)+1, 0.0);// temporary vector to initialize other vectors

	covarianceMatrix.assign((2*m_traceDimension)+1, tempVec); //STL OPT

	eigenVectorMatrix.assign((2*m_traceDimension)+1, tempVec); //STL OPT

	// opening the file containing the training list of each of the shapes     
	ifstream featFileHandle(featureFilePath.c_str());

	//  throwing error if unable to open the Feature file
	if(!featFileHandle)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Unable to open feature list file : " << featureFilePath <<endl;

		LTKReturnError(ETRAINLIST_FILE_OPEN);
	}

	getline(featFileHandle, line, NEW_LINE_DELIMITER);
	stringStringMap headerSequence;
	m_ptrShapeRecUtil->readFeatureFileHeader(line, headerSequence);

/*	m_featureExtractor = "";
	m_featureExtractor = headerSequence[FE_NAME];

	try
	{
		initializeFeatureExtractorInstance();
	}
	catch(LTKException e)
	{
		LTKReturnError(e.getErrorCode());
	}*/

	//  opening the output reference model file
	ofstream mdtFileHandle(m_referenceModelFile.c_str(),ios::out|ios::binary);

	// throwing error if unable to open the reference model file
	if(!mdtFileHandle)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR)<<  "Unable to open model data file : "+ m_referenceModelFile <<endl;

		LTKReturnError(ETRAINLIST_FILE_OPEN);	
	}

	// writing the header information into the reference model file
	mdtFileHandle.write((char*) &m_numShapes,sizeof(unsigned short));

	if(!m_projectTypeDynamic)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_numShapes = " << m_numShapes <<endl;
	}

	mdtFileHandle.write((char*) &m_traceDimension,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_traceDimension = " << m_traceDimension  <<endl;

	mdtFileHandle.write((char*) &momentDimension,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "momentDimension = " << momentDimension <<endl;

	mdtFileHandle.write((char*) &velocityDimension,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "velocityDimension = " << velocityDimension <<endl;

	mdtFileHandle.write((char*) &m_numEigenVectors,sizeof(unsigned short));

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<  "m_numEigenVectors = " << m_numEigenVectors  <<endl;

	// reading the ink file paths from the training list file

	LTKShapeSample shapeSampleFeatures;

	bool lastshapeIdFlag = false; // flag to skip reading a newline in the list file, when a new class starts 
	bool eofFlag = false; // flag is set when EOF is reached
	bool initClassFlag = false; // flag to check initial class

	while(!featFileHandle.eof())
	{
		if(lastshapeIdFlag == false) 
		{
			getline(featFileHandle,line,NEW_LINE_DELIMITER);

			if(featFileHandle.eof())  // check if EOF is reached
			{
				eofFlag = true;
			}

			if((getShapeSampleFromLine(line, shapeSampleFeatures) != SUCCESS) && (eofFlag == false) )
				continue;

			shapeId = shapeSampleFeatures.getClassID();

			if(shapeId < 0)
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The PCA Shape recognizer requires training file class Ids to be positive integers and listed in the increasing order" <<endl;

				LTKReturnError(EINVALID_SHAPEID); 
			}
			else if(shapeId < prevShape)
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Shape IDs in the train list file should be in the increasing order. Please use scripts/validateListFile.pl to generate list files." <<endl;

				LTKReturnError(EINVALID_ORDER_LISTFILE);
			}

			if( initClassFlag == false )
			{
				initClassFlag = true;
				prevShape=shapeId;
			}
		}
		else // class change, do not read next line during this iteration
		{
			lastshapeIdFlag = false; //flag unset to read next line during the next iteration
		}

		// read the ink file and populate the covariance matrix
		if(shapeId == prevShape)
		{
			// extracting features from the preprocessed trace group
			floatVector featureVector;

			featureVector.clear();
			featureVector.push_back(1.00);
			m_ptrShapeRecUtil->shapeFeatureVectorToFloatVector(shapeSampleFeatures.getFeatureVector(), featureVector);
			LTKShapeRecoUtil::clearMemory(&shapeSampleFeatures.getFeatureVector());
			shapeSampleFeatures.setClassID(-1);

			if(featureVector.size() != (2*m_traceDimension+1) )
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The number of points in the tracegroup is not normalized by the preprocessing module (add resampling to the preprocessing module)" <<endl;

				LTKReturnError(EINVALID_NUM_OF_POINTS);	//return FAILURE;
			}

			floatVector revFeatureVector;

			if(m_reverseFeatureVectors)
			{
				reverseFeatureVector(featureVector, revFeatureVector);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Reversing the feature vectors for training" <<endl;
			}

			++sampleCount;


			// updating the covariance matrix after each example of the shape is read 

			for(size = 0; size <= 2 * m_traceDimension; size++)
			{
				(covarianceMatrix.at(size)).at(size) += (featureVector.at(size)*featureVector.at(size));

				if(m_reverseFeatureVectors)
				{
					(covarianceMatrix.at(size)).at(size) += (revFeatureVector.at(size)*revFeatureVector.at(size));
				}
			}

			for(row =0; row <= 2 * m_traceDimension; row++)
			{
				for(column = row+1; column <= 2 * m_traceDimension; column++)
				{
					(covarianceMatrix.at(row)).at(column) +=  (featureVector.at(row)*featureVector.at(column));

					if(m_reverseFeatureVectors)
					{
						(covarianceMatrix.at(row)).at(column) +=  (revFeatureVector.at(row)*revFeatureVector.at(column));
					}
					(covarianceMatrix.at(column)).at(row) = (covarianceMatrix.at(row)).at(column);
				}
			}

			featureVector.clear();

			if(m_reverseFeatureVectors)
			{
				revFeatureVector.clear();
			}
		}

		// compute eigen vectors after all the samples of a class are read 	
		if(shapeId != prevShape || eofFlag == true)
		{
			cout << "Training for class : " << prevShape << endl;

			if(sampleCount >0 )  // increase shape count only if there are atleast one sample per class
				shapeCount++;
			if(!m_projectTypeDynamic && shapeCount > m_numShapes)
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The training file contains more shapes than mentioned in the config file:"<< m_numShapes <<endl;
				LTKReturnError(EINVALID_NUM_OF_SHAPES); 
			}

			if(shapeCount > 0 && sampleCount >0)
			{  
				//writing class ID because the IDs need not be contiguous
				if(eofFlag==true)
				{
					mdtFileHandle.write((char *)&shapeId,sizeof(int));
				}
				else
				{
					mdtFileHandle.write((char *)&prevShape,sizeof(int));
				}

				//  complete covariance matrix computation by dividing with the number of samples

				for(row = 0; row <= (2 * m_traceDimension); row++)
				{
					for(column=0; column <= (2 * m_traceDimension); column++)
					{
						if(m_reverseFeatureVectors)
						{
							(covarianceMatrix.at(row)).at(column) /= (2 * sampleCount);
						}
						else
						{
							(covarianceMatrix.at(row)).at(column) /= sampleCount;
						}
					}
				}

				// computing the eigenvectors and eigenvalues of the covariance matrix

				computeEigenVectors(covarianceMatrix,(2 * m_traceDimension)+1, eigenValueVec, eigenVectorMatrix, nrot);

				// writing the eigenvectors into reference model file

				for(column = 0; column < m_numEigenVectors; ++column)
				{
					for(row = 0; row < (2 * m_traceDimension)+1; ++row)
					{
						mdtFileHandle.write((char *)&((eigenVectorMatrix.at(row)).at(column)),sizeof(float));
					}
				}   

				//  writing the eigen values of the covariance matrix
				//  one eigen value more !!!
				for(column = 0; column <= m_numEigenVectors; ++column)
				{
					mdtFileHandle.write((char *)&eigenValueVec.at(column),sizeof(float));
				}               
				eigenValueVec.clear();



			}

			// reinitializing covariance matrix for the next shape
			for(row = 0; row < 2 * m_traceDimension; row++)
			{
				for(column = 0; column <= 2 * m_traceDimension; column++)                 
				{
					(covarianceMatrix.at(row)).at(column) = 0;          
				}
			}
			// resetting sampleCount for the next class 
			sampleCount = 0;
			// set the flag so that the already read line of next class in the list file is not lost
			lastshapeIdFlag = true;
			// seting the next class shapeId
			prevShape=shapeId;
		}
	}

	//  closing the training list or feature file and returning
	featFileHandle.close();

	if(!m_projectTypeDynamic && shapeCount != m_numShapes)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The training file contains less shapes than mentioned in the config file:"<< m_numShapes <<endl;
		LTKReturnError(EINVALID_NUM_OF_SHAPES); 
	}

	if(m_projectTypeDynamic)
	{
		m_numShapes = shapeCount;
		mdtFileHandle.seekp(0,ios::beg);    
		mdtFileHandle.write((char*) &m_numShapes,sizeof(unsigned short));
	}
	mdtFileHandle.close(); 
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG)<<   "Exiting PCAShapeRecognizer::train" <<endl;

	return SUCCESS;
}

/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 23-03-2007
 * NAME			: getShapeSampleFromLine
 * DESCRIPTION	: This method get the Shape Sample Feature from a given line 
 * ARGUMENTS		: 
 * RETURNS		: none
 * NOTES			: w
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
int PCAShapeRecognizer::getShapeSampleFromLine(string& line, LTKShapeSample& shapeSampleFeatures)
{
	//Line is split into tokens
	stringVector tokens;

	//Class Id
	int classId = -1;

	//Feature Vector string
	string strFeatureVector = "";

	//Tokenize the string 
	LTKStringUtil::tokenizeString(ine,  " ",  tokens);

	//Tokens must be of size 2, one is classId and other is Feature Vector	
	if( tokens.size() != 2)
		return FAILURE;				

	//Tokens[0] indicates the path name
	classId = atoi(tokens[0].c_str());

	//Tokens[1] indicates the shapeId
	strFeatureVector = tokens[1];

	LTKStringUtil::tokenizeString(strFeatureVector,  FEATURE_EXTRACTOR_DELIMITER,  tokens);

	vector<LTKShapeFeature*> shapeFeatureVector;
	LTKShapeFeature* shapeFeature;

	for(int i = 0; i < tokens.size(); ++i)
	{
		shapeFeature = m_ptrFeatureExtractor->getShapeFeatureInstance();
		if(shapeFeature->initialize(tokens[i]) != SUCCESS)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_ERR)<<"Number of features extracted from a trace is not correct"<<endl;
			LTKReturnError(EINVALID_INPUT_FORMAT);
		}
		shapeFeatureVector.push_back(shapeFeature);
	}

	//Set the feature vector and class id to the shape sample features
	shapeSampleFeatures.setFeatureVector(shapeFeatureVector);
	shapeSampleFeatures.setClassID(classId);

	//Clearing the vectors
	LTKShapeRecoUtil::clearMemory(&shapeFeatureVector);

	return SUCCESS;
}

/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 29-03-2007
 * NAME			: train
 * DESCRIPTION	: 
 * ARGUMENTS		: 
 * RETURNS		: 
 * NOTES			: 
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
int PCAShapeRecognizer::train(const string& trainingList, string& strModelDataHeaderInfoFile, string &comment, string &dataset, const string &inFileType)
{
	int returnVal = SUCCESS;

	//Time calculation requirements
	time_t startTime;
	time_t endTime;

	//Time at the beginning of Train
	time(&startTime);

	if(LTKSTRCMP(inFileType.c_str(), INK_FILE) == 0)
	{
		returnVal = trainFromListFile(trainingList);
	}
	else if(LTKSTRCMP(inFileType.c_str(), FEATURE_FILE) == 0)
	{
		returnVal = trainFromFeatureFile(trainingList);
	}

	if(returnVal != SUCCESS)
	{
		LTKReturnError(returnVal);
	}
	m_headerInfo[COMMENT]=comment;
	m_headerInfo[DATASET]=dataset;
	m_headerInfo[FE_NAME]=m_featureExtractor;
	m_headerInfo[FE_VER] = "1.1.0";


	//Updating the Header Information
	updateHeaderWithAlgoInfo();

	if(errorCode = cheSumGen.addHeaderInfo(strModelDataHeaderInfoFile, m_referenceModelFile, m_headerInfo))
	{
		return errorCode;
	}

	//Time at the end of Train Clustering
	time(&endTime);
	cout << "Time Taken = " << difftime(endTime, startTime) << endl;

	return SUCCESS;
}

/******************************************************************************
 * AUTHOR		: Saravanan
 * DATE			: 29-03-2007
 * NAME			: updateHeaderWithAlgoInfo
 * DESCRIPTION	: 
 * ARGUMENTS		: 
 * RETURNS		: 
 * NOTES			: 
 * CHANGE HISTROY
 * Author			Date				Description
 ******************************************************************************/
void PCAShapeRecognizer::updateHeaderWithAlgoInfo()
{
	char versionInfo[VERSION_STR_LEN];

	int iMajor, iMinor;
	int iBugFix;

	m_headerInfo[RECVERSION] = m_currentVersion;
	string algoName = PCA;
	m_headerInfo[RECNAME] = algoName;
}

/***********************************************************************************
* AUTHOR		: Saravanan. R
* DATE			: 19-01-2007
* NAME			: initializePreprocessor
* DESCRIPTION	: This method is used to initialize the PreProcessor
* ARGUMENTS		: preprocDLLPath : string : Holds the Path of the Preprocessor DLL, 
*				  returnStatus    : int    : Holds SUCCESS or Error Values, if occurs
* RETURNS		: preprocessor instance				  
* NOTES			: 
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
LTKPreprocessorInterface* PCAShapeRecognizer::initializePreprocessor(const string preprocDLLPath, LTKControlInfo controlInfo)
{	
	LTKPreprocessorInterface *ptrPreprocInstance;
	FN_PTR_CREATELTKLIPIPREPROCESSOR createLTKLipiPreProcessor = NULL;

	// Load the DLL with path=preprocDLLPath
	m_libHandler = LTKLoadDLL(preprocDLLPath.c_str());

	// Could not load the preproc DLL
	if(!m_libHandler)
	{
		throw(LTKException(ELOAD_PREPROC_DLL));
	}

	// Map createpreprocessor and deletePreprocessor functions
	createLTKLipiPreProcessor = (FN_PTR_CREATELTKLIPIPREPROCESSOR)LTKGetDLLFunc(m_libHandler, CREATEPREPROCINST);

	deleteLTKLipiPreProcessor = (FN_PTR_DELETELTKLIPIPREPROCESSOR)LTKGetDLLFunc(m_libHandler, DESTROYPREPROCINST);

	// Could not map the createLipiPreprocessor function from the DLL
	if(!createLTKLipiPreProcessor)
	{
		//Unload the dll
		unloadPreprocessorDLL();
		throw(LTKException(EDLL_FUNC_ADDRESS_CREATE));
	}

	// Could not map the deleteLipiPreprocessor function from the DLL
	if(!deleteLTKLipiPreProcessor)
	{
		//Unload the dll
		unloadPreprocessorDLL();
		throw(LTKException(EDLL_FUNC_ADDRESS_DELETE));
	}

	// Create preprocessor instance
	ptrPreprocInstance = createLTKLipiPreProcessor(controlInfo);

	// Could not create a LTKLipiPreProcessor
	if(ptrPreprocInstance == NULL)
	{		
		// Unload the DLL
		unloadPreprocessorDLL();
		throw(LTKException(ECREATE_PREPROC));
	}
	
	return ptrPreprocInstance;

}

/**********************************************************************************
* AUTHOR		: Saravanan. R
* DATE			: 25-01-2007
* NAME			: deletePreprocessor
* DESCRIPTION	: This method is used to deletes the PreProcessor instance
* ARGUMENTS		: ptrPreprocInstance : Holds the pointer to the LTKPreprocessorInterface
* RETURNS		: none				  
* NOTES			: 
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
void PCAShapeRecognizer::deletePreprocessor(LTKPreprocessorInterface *ptrPreprocInstance)
{
	//deleting the preprocessor instance
	if(ptrPreprocInstance != NULL)
	{
		deleteLTKLipiPreProcessor(ptrPreprocInstance);
		ptrPreprocInstance = NULL;
	}

	//Unload the dll
	unloadPreprocessorDLL();
}

/**********************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 29-01-2007
* NAME			: unloadPreprocessorDLL
* DESCRIPTION	: This method is used to Unloads the preprocessor DLL.
* ARGUMENTS		: none
* RETURNS		: none				  
* NOTES			: 
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
void PCAShapeRecognizer::unloadPreprocessorDLL()
{
	//Check the preprocessor DLL was loaded already
	if(m_libHandler != NULL)
	{
		//Unload the DLL
		LTKUnloadDLL(m_libHandler);
		m_libHandler = NULL;
	}
}
