/****************************************************************************
* Copyright (c) HP Labs India. All rights reserved.
* This source code is a property of HP Labs India. and is copyright protected.
* UNATHORISED ACCESS, COPYING, REPRODUCTION OR DISTRIBUTION of this program
* is strictly PROHIBITED.
****************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/

/************************************************************************
 * FILE DESCR: Implementation for EigDef Shape Recognition module
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
 * Author       Date            Description
 ************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include "LTKTypes.h"

#include "LTKShapeRecognizer.h"

#include "EigDefShapeRecognizer.h"

#include "LTKConfigFileReader.h"

#include "LTKStringUtil.h"

#include "LTKInkFileReader.h"

#include "LTKLoggerUtil.h"

#include "EigDefFeatureExtractor.h"

#include "LTKException.h"

#include "DynamicTimeWarping.h"

//#include "LTKHierarchicalClusteringS.h"

#include "LTKHierarchicalClustering.h"


#include "LTKPreprocDefaults.h"

#include "LTKTrace.h"

#include "lipiengine.h"

#include "LTKShapeRecoUtil.h"

extern void *m_libHandler;

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : EigDefShapeRecognizer
* DESCRIPTION   : Default Constructor
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

EigDefShapeRecognizer::EigDefShapeRecognizer()
{
	xChannelstr = "X";
	yChannelstr = "Y";

}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : EigDefShapeRecognizer
* DESCRIPTION   : Initialization Constructor
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

EigDefShapeRecognizer::EigDefShapeRecognizer(LTKShapeRecoConfig& config) : LTKShapeRecognizer(EigDef)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered default constructor of EigDefShapeRecognizer" << endl;

    //  initializing number of shapes in the shape set

    m_numShapes = config.getNumShapes();

    //  composing the EigDef configuration file name

    this->m_EigDefCfgFile = config.getLipiRoot() + SEPARATOR + config.getShapeSet() + SEPARATOR + 
             config.getProfile() + SEPARATOR + m_shapeRecognizerName + CONFIGFILEEXT;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "EigDef Configuration File is: " << m_EigDefCfgFile <, endl;


    //  composing the name of the reference model file

    this->m_referenceModelFile = config.getLipiRoot() + SEPARATOR + config.getShapeSet() + 
                        SEPARATOR + config.getProfile() + SEPARATOR + m_shapeRecognizerName + DATFILEEXT;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Reference Model File is: " << m_referenceModelFile << endl;

    
	
	xChannelstr = "X";
	yChannelstr = "Y";

	
	
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting default constructor of EigDefShapeRecognizer" << endl;
}

/**********************************************************************************
* AUTHOR        : Dinesh M.
* DATE          : 23-DEC-2005
* NAME          : EigDefShapeRecognizer
* DESCRIPTION   : Initialization 
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description

*************************************************************************************/
int EigDefShapeRecognizer::initialize(string& strProjectName, string &strProfileName)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<  
        "Entered EigDefShapeRecognizer::Initialize"  << endl;

	string LipiRootPath = string(getLipiPath());
	string tempStr = "";
	string preprocDllPath = "";
    headerInfo[PROJNAME] = strProjectName;
	int returnStatus=0;
	string strNumShapes = "";

	ltkShapeRecPtr = NULL;
	
	//path for eigdef.cfg
	m_EigDefCfgFile = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR + EigDef + CONFIGFILEEXT;

	//path for eigdef.mdt
	m_referenceModelFile = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + strProfileName + SEPARATOR + EigDef + DATFILEEXT;

	// Read number of shapes from project.cfg
	tempStr = LipiRootPath + PROJECTS_PATH_STRING + strProjectName + PROFILE_PATH_STRING + PROJECT_CFG_STRING;
	
	try
	{
		//To find whether the project was dynamic or not
		//to read read number of shapes from project.cfg
		m_projectTypeDynamic = shaperecUtilInst->isProjectDynamic(tempStr, m_numShapes, returnStatus, strNumShapes);
		
		headerInfo[NUMSHAPES] = strNumShapes;
		
		preprocDllPath = LipiRootPath + SEPARATOR + "lib" + SEPARATOR + PREPROC + DLL_EXT;

		// initialize preprocessor
		returnStatus = 0;
		ltkShapeRecPtr = shaperecUtilInst->initializePreprocessor(preprocDllPath,
						returnStatus);

	}
	
	catch(LTKException e)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << e.getExceptionMessage()  << endl;
		LTKReturnError(returnStatus);
	}
	
	setPreprocMap(); // To add function name and the function address into the maping variable.

	//Assigning Default Values
	assignDefaultValues();

	//Reading pca configuration file
	returnStatus = readClassifierConfig();

	return returnStatus;
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
* Author            Date                Description
*************************************************************************************/
int EigDefShapeRecognizer::loadModelData()
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::loadTrainingData"  << endl;

    unsigned short numShapes;           // number of shapes in the recognition problem
    
    unsigned short traceDimension;      // number of points obtained from a shape

    unsigned short numEigenVectors;     // number of eigen vectors used model each shape

    int shapeIndex;                     //  variable to loop over all the shapes

    int eigenIndex;                     //  variable to loop over all the eigenvectors

    int traceIndex;                     //  variable to loop over all the dimensions of the vector

    unsigned short momentDimension;     //  number of moment dimensions (not used anymore)

    unsigned short velocityDimension;   //  number of velcity dimensions (not used anymore)

    float eigenVectorComponent;         //  one of the dimensions of an eigenvector

    float eigenValue;					//  one eigen value
	
	float meanValue;					//  mean value

	bool eofFlag;                       // set to true when the end of file is reached.

    string inFilePath;                  //  name of the dat file to be read

    floatVector eigenVector;            //  an eigenvector

    floatVector meanVector;				//  meanVector this is the prototype

	floatVector deformMeanVector;        //mean of the deformations
	
	float2DVector modelParamsVec;   //  all the eigenvectors of a particular shape
    
    floatVector eigenValuesVec;            //  all the eigenvalues of a particular shape

    EigDefShapeModel shapeModel;               //  shape-eigenvectors storage type

	string algoVersion;					//Algorithm version.

	char versionInfo[VERSION_STR_LEN];
	string version;
	int iMajor, iMinor, iBugfix;

	stringStringMap headerSequence;

    if((errorCode = cheSumGen.readMDTHeader(m_referenceModelFile,headerSequence)) != 0)
	{
		return errorCode;
		//throw LTKException("ModelData File has been corrupted" + m_referenceModelFile);
	}

	algoVersion = headerSequence[RECVERSION].c_str();

	getCurrentVersion(&iMajor, &iMinor, &iBugfix);

	sprintf(versionInfo, "%c", algoVersion[0]);

	int ver = atoi(versionInfo);
	if(ver != iMajor)

	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Incompatible algorithm version : " << version  << endl;

		LTKReturnError(EINCOMPATIBLE_VERSION);
	}

    //  opening the dat file

    ifstream infile(m_referenceModelFile.c_str(),ios::in | ios::binary);

    if(!infile)
    {
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Unable to open model data file : " << m_referenceModelFile  << endl;

		LTKReturnError(EMODEL_DATA_FILE_OPEN);
	}

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Reference model file read"  << endl;

    infile.seekg(atoi(headerSequence[HEADERLEN].c_str()),ios::beg);    

	//  reading reference model file header

    infile.read((char*) &numShapes, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "number of shapes read from reference model file = " << m_numShapes );

    if(m_numShapes != numShapes)
    {
        LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Invalid value for number of shapes"  << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "NumShapes from cofiguration file ( or default value ) : " << m_numShapes << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "NumShapes from model data file : " << numShapes << endl;

		LTKReturnError(EINVALID_NUM_OF_SHAPES);
    }

    infile.read((char*) &traceDimension, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "trace dimension read from reference model file = " << traceDimension << endl;

    if(m_traceDimension != traceDimension)
    {
        LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Invalid value for trace dimension"  << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "TraceDimension from cofiguration file ( or default value ) : " << m_traceDimension << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "TraceDimension from model data file : " << traceDimension << endl;

		LTKReturnError(EINVALID_TRACE_DIMENTION);
    }

    infile.read((char*) &momentDimension, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "momentDimension = " << momentDimension << endl;

    infile.read((char*) &velocityDimension, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "velocityDimension = " << velocityDimension << endl;

    infile.read((char*) &numEigenVectors, atoi(headerSequence[SIZEOFSHORTINT].c_str()));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "number of eigen vectors read from reference model file = " << numEigenVectors << endl;

    if(m_numEigenVectors != numEigenVectors)
    {
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Invalid value for eigen vector"  << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "NumEigenVectors from config file ( or default value ): " << m_numEigenVectors << endl;
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "NumEigenVectors from model data file: " << numEigenVectors << endl;

		LTKReturnError(EINVALID_NUMEIGENVECTOR);
    }

    
    int floatSize = atoi(headerSequence[SIZEOFFLOAT].c_str());

	if(floatSize <= 0 )
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Invalid float size entry in model data File : " << floatSize << endl;

		LTKReturnError(EINVALID_FLOAT_SIZE);	
	}
	
	//  reading eigenvectors of each of the shapes
	eigenVector.reserve(m_traceDimension*2);
	modelParamsVec.reserve(m_numEigenVectors);
	//m_referenceModels.reserve(m_numShapes);

    int dim=0;
	int count=0;
	cout << "The following shape models have been loaded" << endl ; 
	while(!infile.eof())			// added this line
    {
		eofFlag = false;

        count++;
		for( eigenIndex = 0;eigenIndex < m_numEigenVectors; ++eigenIndex)
        {
            for( traceIndex = 0; traceIndex < (m_traceDimension*2) + 1;++traceIndex)
            {    
				  if(infile.eof())
				  {
					  --count;
					  eofFlag = true;
					  break;
				  }
                  infile.read((char*) &eigenVectorComponent, sizeof(float));

                  eigenVector.push_back(eigenVectorComponent);
            }

			if(eofFlag)
			{
				break;
			}
            
            modelParamsVec.push_back(eigenVector);

            eigenVector.clear();
        }
		if(eofFlag)
		{
			break;
		}

        eigenValuesVec.clear();

        for( eigenIndex = 0;eigenIndex <= m_numEigenVectors; ++eigenIndex)
        {
            infile.read( (char*)&eigenValue, sizeof(float));
        
            eigenValuesVec.push_back(eigenValue);
        }

		meanVector.clear();
		for( dim = 0;dim <= m_traceDimension*2; ++dim)
        {
            infile.read( (char*)&meanValue, sizeof(float));
        
            meanVector.push_back(meanValue);
            
		}
        
		deformMeanVector.clear();
		for(  dim = 0;dim <= m_traceDimension*2; ++dim)
        {
            infile.read( (char*)&meanValue, sizeof(float));
        
            deformMeanVector.push_back(meanValue);
            
		}

		infile.read((char*)&shapeIndex, sizeof(int));	//added this line
		
		cout << shapeIndex <<' ';
		shapeModel.setEigenValues(eigenValuesVec);

        shapeModel.setShapeId(shapeIndex);

        shapeModel.setModel(modelParamsVec);

        shapeModel.setMean(meanVector);

		shapeModel.setDeformMean(deformMeanVector);

		modelParamsVec.clear();

        m_referenceModels.push_back(shapeModel);
    }
	cout << endl ;
	cout << " The total number of shape models loaded is " << count << endl;
    //  closing the reference model file and returning

    infile.close();

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer::loadTrainingData"  << endl;

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
*                 shapeSubSet - possible output space of the recognition result
*                 confThreshold - shapes with confidence below this threshold are rejected
*                 numChoices - maximum number of choices to be returned
*                 resultVec - result of recognition
* RETURNS       : SUCCESS on successful running of the code
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

int EigDefShapeRecognizer::recognize(const LTKTraceGroup& traceGroup, const LTKScreenContext& uiParams, 
    const vector<bool>& shapeSubSet, float confThreshold, 
    int  numChoices, vector<LTKShapeRecoResult>& resultVec)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::recognize"  << endl;

    int shapeIndex;                         //  variable to loop over all of the shapes

    int choiceIndex;                        //  variable to loop over all the recognition choices

    int featureIndex;                       //  variable to loop over all features

    float distance;                         //  EigDef distance

    float weightedDistance;                 //  EigDef distance weighted by the eigen values

    float dotProduct;                       //  dot product of the input with an eigenvector

    float revDistance;                      //  distance calculated with the reversed feature vector

    float weightedRevDistance;              //  weighted distance calculated with the reversed feature vector

    float revDotProduct;

    int shapeLabel;                         //  label assigned to the shapes

    float norm = 0.0;                       //  norm of the test sample

    float maximumDist = 0.0;                //  maximum distance in the choices list

    unsigned short featureDimension = 0;    //  number of feature dimensions

    intVector labelVec;                     //  list of current top choices                 

    float distanceSum = 0;					// summation of distances of the returned choices

	float normalizedWeight = 0;				//	normalized weight for each eigen 

	float lastEigenValue;

	float firstEigenValue;
	
	float bestSoFar = FLT_MAX;				//Best so far limits

    LTKShapeRecoResult outResult;           //  recognition result holder

    LTKTraceGroup preprocessedTraceGroup;   //  pre-processed input
    
    EigDefShapeModel shapeModel;               //  shape-eigenvectors storage space

    EigDefFeatureExtractor featureExtract;     //  feature extracting object

    floatVector featureVector;              //  Mean subtracted feature vector of the input

	float2DVector feature2DVector;

	floatVector xyVector;

	float2DVector mean2DVector;

	floatVector nonZeroMeanfeatureVector;              //  feature vector of the input
    
	//  iterators of the various vectors defined earlier
	floatVector::const_iterator featureVecIter;

    floatVector::reverse_iterator revFeatureVecIter;

    floatVector::const_iterator eigVecComponentsIter;

    floatVector::const_iterator featureVecEnd;

    vector<floatVector >::const_iterator eigVecIter;

	floatVector::const_iterator eigValIter;

    vector<EigDefShapeModel>::const_iterator shapeIter;

	m_screenContext = uiParams;
	
	vector <struct myPair> distIndexPairVector(m_referenceModels.size());
	int count=0;
    
	//  pre-processing and extracting features from the input

    if( preprocess(traceGroup, preprocessedTraceGroup) != SUCCESS)
	{
			return errorCode;
	}
        
    featureExtract.extractFeatures(preprocessedTraceGroup, nonZeroMeanfeatureVector); 
    
    floatVector tempVec(2, 0.0);                    // temporary vector to initialize other vectors
	
    //  initializing the various vectors

	feature2DVector.assign(m_traceDimension, tempVec);
	mean2DVector.assign(m_traceDimension,tempVec);
	
	//  computing the effective number of feature dimensions possible

    if(nonZeroMeanfeatureVector.size() != (2 * m_traceDimension + 1))
    {
        featureDimension = (nonZeroMeanfeatureVector.size() < (2 * m_traceDimension+1) ) ? nonZeroMeanfeatureVector.size(): (2 * m_traceDimension + 1);
    }
    else
    {
        featureDimension = nonZeroMeanfeatureVector.size();
    }

    shapeLabel = -1;

	for(count = 0; count <= (2 * m_traceDimension); ++count)
    {
		featureVector.push_back(0.0);
	} 
    
    //  calculating the distance of the test vector from each of the possible shapes

    int j;
	for(shapeIter = m_referenceModels.begin(),j = 0; shapeIter < m_referenceModels.end(); ++shapeIter,++j)
    {
		if(!shapeSubSet.empty())
        {
            if(shapeSubSet[shapeIndex] == false)
            {
                continue;
            }
        }

        const vector< floatVector >& modelParamsVec = (*shapeIter).getModel();

		const floatVector& eigenValues = (*shapeIter).getEigenValues();

        const floatVector& meanVector = (*shapeIter).getMean();

		const floatVector& deformMeanVector = (*shapeIter).getDeformMean();
		
		shapeIndex=(*shapeIter).getShapeId();
		
		for(count = 1; count <=( m_traceDimension); ++count)
		{
			feature2DVector.at(count-1).at(0)=nonZeroMeanfeatureVector.at(count);
			mean2DVector.at(count-1).at(0)=meanVector.at(count);
			feature2DVector.at(count-1).at(1)=nonZeroMeanfeatureVector.at(count+60);
			mean2DVector.at(count-1).at(1)=meanVector.at(count+60);
		
		}
		
		// getting the deformation in to the feature vector
		float distanceDTW = 0.0;
		ComputeDTWDeformationVector(mean2DVector,feature2DVector,60,60,featureVector,distanceDTW);
		featureVector.at(0)=0;
		
		distance=computeDeformDistance(featureVector, deformMeanVector, modelParamsVec, eigenValues);
		
		distance=.98*distance +.02*distanceDTW;// it has to be alpha combination here
		
        distIndexPairVector[j].distance = distance;
		distIndexPairVector[j].classId = shapeIndex;
			
		}

		sort(distIndexPairVector.begin(),distIndexPairVector.end(),mycmp);

		outResult.setShapeId(distIndexPairVector[0].classId);
		outResult.setConfidence(distIndexPairVector[0].distance);
		cout << distIndexPairVector[0].classId<< '\t' <<distIndexPairVector[0].distance <<endl;
		resultVec.push_back(outResult);
		vector<int> topClassVec;
		vector<int>::iterator topClassVecIter;
		int numTopClasses = 1;
		topClassVec.push_back(distIndexPairVector[0].classId);
		for(int i = 1; i < distIndexPairVector.size(); ++i)
		{
			topClassVecIter = find(topClassVec.begin(), topClassVec.end(), distIndexPairVector[i].classId);
			if(topClassVecIter == topClassVec.end())
			{
				outResult.setShapeId(distIndexPairVector[i].classId);
				outResult.setConfidence(distIndexPairVector[i].distance);
				cout << distIndexPairVector[i].classId<< '\t' <<distIndexPairVector[i].distance <<endl;
				resultVec.push_back(outResult);
				numTopClasses++;
				topClassVec.push_back(distIndexPairVector[i].classId);
			}
			if(numTopClasses == numChoices)
				break;
		}
   
    labelVec.clear();

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer::recognize"  << endl;

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
* Author			Date				Description
*************************************************************************************/

int EigDefShapeRecognizer::getShapeRecoConfidences(const LTKTraceGroup& traceGroup, 
		const LTKScreenContext& screenContext,
		const vector<int>& shapeIds, vector<float>& shapeRecoConfidences)
{
	LTKTraceGroup preprocessedTraceGroup; //Pre processed trace group
	floatVector featureVector;              //  feature vector of the input
	vector<int>::const_iterator shapeIdIter; //iterator for iterating through the input shape subset
   
	float distance;                         //  EigDef distance
    float weightedDistance;                 //  EigDef distance weighted by the eigen values
    float dotProduct;                       //  dot product of the input with an eigenvector
    float revDistance;                      //  distance calculated with the reversed feature vector
    float weightedRevDistance;              //  weighted distance calculated with the reversed feature vector
    float revDotProduct;
    float norm = 0.0;                       //  norm of the test sample
    int featureIndex;                       //  variable to loop over all features
    unsigned short featureDimension = 0;    //  number of feature dimensions
	float normalizedWeight;					//	Normalized weight
	float lastEigenValue;

    EigDefFeatureExtractor featureExtract;     //  feature extracting object

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

    featureExtract.extractFeatures(preprocessedTraceGroup, featureVector); 
	if(featureVector.size() != (2*m_traceDimension +1) )
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "The number of points in the tracegroup is not normalized by the preprocessing module (add resampling to the preprocessing module)" << endl;

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

        const vector< floatVector >& modelParamsVec = (m_referenceModels.at(*shapeIdIter)).getModel();;
		const floatVector& eigenValues = (m_referenceModels.at(*shapeIdIter)).getEigenValues();

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
* Author            Date                Description
*************************************************************************************/

int EigDefShapeRecognizer::learn(const LTKTraceGroup& prototype, int shapeId, const LTKScreenContext& uiParams, 
                         const LTKCaptureDevice& deviceParams)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered EigDefShapeRecognizer::learn"  << endl;

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting EigDefShapeRecognizer::learn"  << endl;

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Dinesh M
* DATE          : 23-DEC-2004
* NAME          : train
* DESCRIPTION   : offline training module
* ARGUMENTS     : trainingList - name of the file containing the listing of the files to be used for
*                 training each of the shapes
* RETURNS       : SUCCESS on successful training
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/


int EigDefShapeRecognizer::train(const string& trainingList, string& strModelDataHeaderInfoFile, string &comment, string &dataset, const string &inFileType)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered EigDefShapeRecognizer::train"  << endl;

    string path;                            //  file path of a ink file
	
	string line;		// a line read from the list file

	vector<string> tokens;	// line split in to tokens
	
	int  shapeId;                         //  shape id of the ink file

    int shapeCount = 0;                     //  count of the no. of shape samples read so far

    int sampleCount = 0; //  count for the no. of samples read for a shape

    int nrot = 0;                           //  number of iterations required for jacobi convergence

    int size;                               //  size of the covariance matrix
    
    int row;                                //  a row of the covariance matrix

    int column;                             //  a column of the covariance matrix

	int classId = 0;						// classId for checking if the shapeIds read from the list file start from 0 and are contigious
	
	int numClusters;    //number of clusters per class
	
	float distanceDTW;
	  
    unsigned short momentDimension = 0;     //  number of moment features (not used any more)
    
    unsigned short velocityDimension = 0;   //  number of velocity features (not used any more)

    floatVector eigenValueVec;              //  eigen values of the covariance matrix

    vector< floatVector > covarianceMatrix; // covariance matrix of the input samples of a shape

    vector< floatVector > eigenVectorMatrix;//  eigen vectors of the covariance matrix

	vector< floatVector > meanMatrix;       // This is mean * mean' matrix.
	
	vector<floatVector> feature2DVector;
	
	vector<floatVector> mean2DVector;

	Features tempFeature(2);				//temp feature tuple

	floatVector deformVector((2*m_traceDimension)+1, 0.0);	
	
	floatVector deformMean((2*m_traceDimension)+1, 0.0);	

	floatVector mean((2*m_traceDimension)+1, 0.0);						//  Mean for each class. 

	floatVector tempVec((2*m_traceDimension)+1, 0.0);                    // temporary vector to initialize other vectors
	
	int iMajor, iMinor, iBugFix;			// Version identifiers

	char versionInfo[VERSION_STR_LEN];					// Holds the version information.

    string version, eigdef="EigDef";
	
	LTKCaptureDevice captureDevice;

    LTKScreenContext screenContext;

	Character charac;

    // initializing the various vectors

	covarianceMatrix.assign((2*m_traceDimension)+1, tempVec); // here it is the covariance matrix of the deformations
    
	eigenVectorMatrix.assign((2*m_traceDimension)+1, tempVec);
	
	meanMatrix.assign((2*m_traceDimension)+1, tempVec); //we first select the coorelation matrix and then subtract mean matrix from it
														// to get the covariance matrix
    
	floatVector tempVec2(2, 0.0);                    // temporary vector to initialize other vectors
	feature2DVector.assign(m_traceDimension, tempVec2);
	mean2DVector.assign(m_traceDimension,tempVec2);
	
	EigDefFeatureExtractor EigDefFeatureExtract;  //  extracts features required by EigDef

    //  opening the file containing the training list of each of the shapes 
    
    ifstream in(trainingList.c_str());


    //  throwing error if unable to open the training list file

    if(!in)
    {
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Unable to open train list file " << trainingList  << endl;

		LTKReturnError(ETRAINLIST_FILE_OPEN);    
	}

    //  opening the output reference model file

    ofstream out(m_referenceModelFile.c_str(),ios::out|ios::binary);

    // throwing error if unable to open the reference model file

    if(!out)
    {
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
            "Unable to open train list file : " << m_referenceModelFile  << endl;

		LTKReturnError(ETRAINLIST_FILE_OPEN);	
    }

    ifstream inFilePtr;                     //  file pointer to read ink files

    //  writing the header information into the reference model file

    out.write((char*) &m_numShapes,sizeof(unsigned short));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_numShapes = " << m_numShapes << endl;

    out.write((char*) &m_traceDimension,sizeof(unsigned short));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_traceDimension = " << m_traceDimension << endl;

    out.write((char*) &momentDimension,sizeof(unsigned short));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "momentDimension = " << momentDimension << endl;

    out.write((char*) &velocityDimension,sizeof(unsigned short));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "velocityDimension = " << velocityDimension << endl;

    out.write((char*) &m_numEigenVectors,sizeof(unsigned short));

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "m_numEigenVectors = " << m_numEigenVectors << endl;

    //  reading the ink file paths from the training list file
    cout <<"Started training - Eigen Deformations Classifier "<<endl;

	vector<Character> featureVec;
	int2DVector outputVector;        
    vector<Character> resultVec;

	bool lastshapeIdFlag = false; //flag to skip reading a newline in the list file, when a new class starts 
	bool eofFlag = false; //flag is set when EOF is reached
	bool initClassFlag = false; // flag to check initial class
    while(!in.eof())
	{
		if(lastshapeIdFlag == false) 
		{
			getline(in,line,'\n');
			LTKStringUtil::tokenizeString(ine,  " \t\r",  tokens);

			if(in.eof())  //check if EOF is reached
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
			if(path[0] != COMMENTCHAR)
			{
				if(tokens.size() == 2)
				{
					shapeId = atoi(tokens[1].c_str());
					if(initClassFlag == false)
					{
						initClassFlag = true;
						if(shapeId != 0)
						{
							LOG( LTKLogger::LTK_LOGLEVEL_ERR) <<
                                "The DTW Shape recognizer requires training file class Ids to be 0 ... " <<
                                m_numShapes << endl;
                            
							LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
                                " and the training file should list them in the order" << endl;

							LTKReturnError(EINITSHAPE_NONZERO); 
						}
					}

				}
				else
				{
					if(eofFlag == false)
					{
						LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
                            "The training list file has an invalid line ... " << line  << endl;
						LTKReturnError(EINVALID_LINE_LISTFILE);
					}
				}
			}
		}
		else 
		{
			if(shapeId != classId)
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
                    "The DTW Shape recognizer requires training file class Ids to be 0 ... " <<\
                    m_numShapes << endl;
                
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << 
                    " and the training file should list them in the order" << endl;
				LTKReturnError(EINVALID_ORDER_LISTFILE);
			}
			lastshapeIdFlag = false; //flag unset to read next line during the next iteration
		}
		
		if(shapeId == classId && path != "" && path[0] != COMMENTCHAR && tokens.size() > 0)
		{
			cout << path <<endl;
			// if not new class come to this part of the code
			LTKTraceGroup inTraceGroup, outTraceGroup;
			//  reading the ink file
			LTKInkFileReader::readUnipenInkFile(path, inTraceGroup, captureDevice, screenContext);

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Ink file " << path <<
                "of shape " << shapeId << endl;

			++sampleCount;
			//  preprocessing the read trace group
			if( preprocess(inTraceGroup, outTraceGroup) != SUCCESS)
			{
				return errorCode;
			}

			//  extracting features from the preprocessed trace group
			//Convert trace group to character and populate the feature vector
			convertTraceGroupToCharacter(outTraceGroup,charac);

			//featureVec contains a vector of all the feature vectors(character) of a class
			featureVec.push_back(charac);
		
			charac.clear();
			
		}

		
		// execute this loop , if it is the end of samples of a particular class
		if(shapeId != classId || eofFlag == true)
        {
            if(sampleCount >0 )  // increase shape count only if there are atleast one sample per class
				shapeCount++;
			if(shapeCount > m_numShapes)
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) <<
                    "The training file contains more shapes than mentioned in the config file......" <<\
                    m_numShapes << endl;
                
				LTKReturnError(EINVALID_NUM_OF_SHAPES); 
			}
			
			if(shapeCount > 0 && sampleCount >0)
            {  
				cout << "Started Clustering .. " << endl;
				vector<vector<double> > distanceMatrix;	//to get the distance map of the samples
				if(m_numClusters == 0)
				{					
					LTKHierarchicalClustering<Character> hc(featureVec,computeDTWDistance,AVERAGE_LINKAGE);
					hc.cluster();
					//Cluster results are populated in an outputVector
					hc.getClusterResult(outputVector);
					distanceMatrix = hc.getdistanceMap();
				}
				else
				{
					LTKHierarchicalClustering<Character> hc(featureVec,computeDTWDistance,AVERAGE_LINKAGE,m_numClusters);
					hc.cluster();
					//Cluster results are populated in an outputVector
					hc.getClusterResult(outputVector);
					distanceMatrix = hc.getdistanceMap();
				}
		
				cout << "Clustering done ... calculating median" << endl; 

				for (int clusNum = 0; clusNum < outputVector.size(); clusNum++)
				{
					double minDist = FLT_MAX;
					int medianIndex;
					for (int clusMem = 0; clusMem < outputVector[clusNum].size(); clusMem++)// for each element of the cluster
					{
						double dist = 0;
						for(int otherClusMem = 0; otherClusMem < outputVector[clusNum].size(); otherClusMem++)
						{
							if(outputVector[clusNum][clusMem] != outputVector[clusNum][otherClusMem])
							{
								if(outputVector[clusNum][otherClusMem] > outputVector[clusNum][clusMem])
								{
									int tempi = outputVector[clusNum][clusMem];
									int tempj = outputVector[clusNum][otherClusMem];
									dist += distanceMatrix[tempi][tempj-tempi-1];
								}
								else
								{
									int tempi = outputVector[clusNum][otherClusMem];
									int tempj = outputVector[clusNum][clusMem];
									dist += distanceMatrix[tempi][tempj-tempi-1];
								}
							}
						}
						if(dist < minDist)
						{
							minDist = dist;
							medianIndex = outputVector[clusNum][clusMem];
						}
					}
					
					//cout << " The index of the median in the cluster is" << medianIndex << endl;
					resultVec.push_back(featureVec[medianIndex]);
				}

				for (int i=0; i < outputVector.size()  ; i++) // for each cluster
				{
					if(outputVector[i].size() > (m_numEigenVectors*2)) // find eigen deformations only if the size of the cluster is greater than five
					{
						for (int j=0; j < outputVector[i].size()  ; j++)	// for each element of a cluster
						{
							mean.at(0)=1;
							for(int count = 0; count <( m_traceDimension); ++count)
							{
								feature2DVector.at(count).at(0)=featureVec.at(outputVector[i][j]).at(count).at(0);
								mean2DVector.at(count).at(0)=resultVec.at(i).at(count).at(0);
								mean.at(count+1)=resultVec.at(i).at(count).at(0);
								feature2DVector.at(count).at(1)=featureVec.at(outputVector[i][j]).at(count).at(1);
								mean2DVector.at(count).at(1)=resultVec.at(i).at(count).at(1);
								mean.at(count+61)=resultVec.at(i).at(count).at(1);
							}
							
							deformVector.at(0)=0;
							
							ComputeDTWDeformationVector(mean2DVector,feature2DVector,m_traceDimension,m_traceDimension,deformVector,distanceDTW);
							
							for(size = 0; size <= 2 * m_traceDimension; size++)
							{	
								deformMean.at(size)+=(deformVector.at(size)/(outputVector[i].size()));
							}
							for(size = 0; size <=2 * m_traceDimension; ++size)
							{
								(covarianceMatrix.at(size)).at(size) += (deformVector.at(size)*deformVector.at(size));
							}
							for(row =0; row <=2 * m_traceDimension; ++row)
							{
								for(column = row+1; column <= 2 * m_traceDimension; ++column)
								{
									(covarianceMatrix.at(row)).at(column) +=  (deformVector.at(row)*deformVector.at(column));
									(covarianceMatrix.at(column)).at(row) = (covarianceMatrix.at(row)).at(column);
								}
							}
						}// end of for loop for each element of the cluster		
						for(size = 0; size <= 2 * m_traceDimension; ++size)
						{
							(meanMatrix.at(size)).at(size) = (deformMean.at(size)*deformMean.at(size));
						}
						for(row =0; row <= 2 * m_traceDimension; ++row)
						{
							for(column = row+1; column <= 2 * m_traceDimension; ++column)
							{
								(meanMatrix.at(row)).at(column) =  (deformMean.at(row)*deformMean.at(column));
								(meanMatrix.at(column)).at(row) = (meanMatrix.at(row)).at(column);
							}
						}
						for(row = 0; row <= (2 * m_traceDimension); ++row)
						{
							for(column=0; column <=(2 * m_traceDimension); ++column)
							{
								(covarianceMatrix.at(row)).at(column) /= (outputVector[i].size());
								(covarianceMatrix.at(row)).at(column) -= meanMatrix.at(row).at(column);
							}
						}
						meanMatrix.assign((2*m_traceDimension)+1, tempVec);
						computeEigenVectors(covarianceMatrix,(2 * m_traceDimension)+1, eigenValueVec, eigenVectorMatrix, nrot);
   						for(column = 0; column < m_numEigenVectors; ++column)
						{
							for(row = 0; row <= (2 * m_traceDimension); ++row)
							{
								out.write((char *)&((eigenVectorMatrix.at(row)).at(column)),sizeof(float));
							}
						}   
						for(column = 0; column <= m_numEigenVectors; ++column)
						{
							out.write((char *)&eigenValueVec.at(column),sizeof(float));
							if(eigenValueVec.at(column) < 0)
							{
								cout<<"ERROR The eigen value at "<<column<<" is "<<eigenValueVec.at(column)<<endl;
								cout<<"NUMER of samples "<<(outputVector[i].size())<<endl;
							}
						}               
						eigenValueVec.clear();
					
						for(column = 0; column <= (2 * m_traceDimension); ++column)
						{
							out.write((char *)&mean.at(column),sizeof(float));
							
						} 
						mean.assign((2*m_traceDimension)+1, 0.0);
					
						for(column = 0; column <= (2 * m_traceDimension); ++column)
						{
							out.write((char *)&deformMean.at(column),sizeof(float));
						
						}	
						deformMean.assign((2*m_traceDimension)+1, 0.0);
						out.write((char *)&classId,sizeof(int));
								
						for(row = 0; row <= 2 * m_traceDimension; row++)
						{	
							for(column = 0; column <= 2 * m_traceDimension; column++)                 
							{
								(covarianceMatrix.at(row)).at(column) = 0;          
							}
						}
                
					}// end of if loop if cluster size is greater than > 2*m_numEigenVectors
				
				}// end of for loop for each cluster of a class
				cout<<"Calculation of eigen values over"<<endl;
				// resetting sampleCount for the next class 
	            sampleCount = 0;
		        // set the flag so that the already read line of next class in the list file is not lost
				lastshapeIdFlag = true;
				// seting the next class shapeId
				// It must be classId = shapeId but now we support class ids in sequence only
				++classId;
				outputVector.clear();
				featureVec.clear();
				resultVec.clear();
			}// end of inner if numchars for that class > 0
			
		}//end of outer if new class 

		       
	}// end of while loop end of file

    //  closing the training list file and returning
	in.close();
	out.close(); 
	
	if(shapeCount != m_numShapes)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) <<
            "The training file contains less shapes than mentioned in the config file......" <<
            m_numShapes << endl;
		LTKReturnError(EINVALID_NUM_OF_SHAPES); 
	}

	
	getCurrentVersion(&iMajor,&iMinor,&iBugFix);

	sprintf(versionInfo, "%d.%d.%d",iMajor, iMinor, iBugFix);

	version = versionInfo;

	headerInfo[RECVERSION] = version;
	string algoName = "eigdef";
	headerInfo[RECNAME] = algoName;
	headerInfo[COMMENT]=comment;
	headerInfo[DATASET]=dataset;
	if(errorCode = cheSumGen.addHeaderInfo(strModelDataHeaderInfoFile, m_referenceModelFile, headerInfo))
	{
		return errorCode;
	}

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting EigDefShapeRecognizer::train"  << endl;

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
* Author            Date                Description
*************************************************************************************/

int EigDefShapeRecognizer::computeEigenVectors(vector< vector<float> > &covarianceMatrix, int n,
    vector<float> &eigenValueVec, vector< vector<float> > &eigenVectorMatrix, int& nrot)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::computeEigenVectors"  << endl;

    int MAX_ITER = 10000;

    int iq,ip,count,p,q,r;

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

            LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
                "Exiting EigDefShapeRecognizer::computeEigenVectors"  << endl;

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
    
    LOG( LTKLogger::LTK_LOGLEVEL_INFO) << 
        "Too Many Iterations for Jacobi Convergence"  << endl;

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

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer::computeEigenVectors"  << endl;
            
    return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 01-06-2005
* NAME			: resampleTraceGroup
* DESCRIPTION	: allocates points to each of the traces of a multi-stroke trace group 
*				  and calls the resampleTrace function on each of the trace - based on 
				  stroke length
* ARGUMENTS		: inTraceGroup - trace group to be resampled
				  outTraceGroup - resampled inTraceGroup
* RETURNS		: SUCCESS on successful resampling operation
* NOTES			: This function resamples based on stroke length (Distance between the points)
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeRecognizer::resampleTraceGroup(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::resampleTraceGroup1"  << endl;
	
	float totalLength = 0;

	int totalAllocPoints = 0;

	int traceIndex;

	vector<LTKTrace> newTracesVector;

	const vector<LTKTrace>& tracesVector = inTraceGroup.getAllTraces();
	
	int numTraces = tracesVector.size();

	floatVector lengthsVec(numTraces, 0.0f);

	intVector resamplePointsVec(numTraces);

	int maxIndex = 0;

	float maxLength = 0;

	for(traceIndex = 0; traceIndex < numTraces; ++traceIndex)
	{
		const LTKTrace& trace = tracesVector[traceIndex];

		lengthsVec[traceIndex] = ltkShapeRecPtr->computeTraceLength(trace, 0, trace.getNumberOfPoints() - 1);

		totalLength += lengthsVec[traceIndex];

		if(lengthsVec[traceIndex] > maxLength)
		{
			maxLength = lengthsVec[traceIndex];

			maxIndex = traceIndex;
		}
	}

	for(traceIndex = 0; traceIndex < numTraces; ++traceIndex)
	{
		if(traceIndex != maxIndex)
		{
			resamplePointsVec[traceIndex] =  m_quantizationStep * int(floor((m_traceDimension * lengthsVec[traceIndex] / (m_quantizationStep * totalLength)) + 0.5f));

			if(resamplePointsVec[traceIndex] <= 0)
			{
				resamplePointsVec[traceIndex] = 1;
			}

			totalAllocPoints += resamplePointsVec[traceIndex];
		}
	}

	resamplePointsVec[maxIndex] = m_traceDimension - totalAllocPoints;

	for(traceIndex = 0; traceIndex < numTraces; ++traceIndex)
    {
		LTKTrace newTrace;

        const LTKTrace &trace = tracesVector.at(traceIndex);

		
		
		if(resampleTrace(trace,resamplePointsVec[traceIndex], newTrace) != SUCCESS)
		{
			return errorCode;
		}
            
 		outTraceGroup.m_traceVector.push_back(newTrace);
	    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) <<
            "Number of points after resampling trace " <<
            traceIndex << resamplePointsVec[traceIndex] << endl;
    }

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting DTWShapeRecognizer::resampleTraceGroup1"  << endl;

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
* Author            Date                Description
*************************************************************************************/

int EigDefShapeRecognizer::preprocess(const LTKTraceGroup& inTraceGroup, LTKTraceGroup& outTraceGroup)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::preprocess"  << endl;

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

			if(!module.compare("EigDef"))
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
				pPreprocFunc = ltkShapeRecPtr->getPreprocptr(funName);
				if(pPreprocFunc!= NULL)
				{
					outTraceGroup.emptyAllTraces();
					if((errorCode = (ltkShapeRecPtr->*(pPreprocFunc))(local_inTraceGroup,outTraceGroup)) != SUCCESS)
					{
						return errorCode;
					}
						
					local_inTraceGroup = outTraceGroup;
					
				}
			}
			indx++;
		}
	}

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer::preprocess"  << endl;

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
* Author            Date                Description
*************************************************************************************/

int EigDefShapeRecognizer::reverseFeatureVector(floatVector& featureVector, floatVector& revFeatureVector)
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::reverseFeatureVector"  << endl;

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

    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::reverseFeatureVector"  << endl;

    return SUCCESS;
}

/**********************************************************************************
* AUTHOR        : Balaji R.
* DATE          : 23-DEC-2004
* NAME          : ~EigDefShapeRecognizer
* DESCRIPTION   : destructor
* ARGUMENTS     : 
* RETURNS       : 
* NOTES         :
* CHANGE HISTROY
* Author            Date                Description
*************************************************************************************/

EigDefShapeRecognizer::~EigDefShapeRecognizer()
{
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered destructor of EigDefShapeRecognizer"  << endl;
		if(ltkShapeRecPtr != NULL)
	{
		deleteLTKLipiPreProcessor(ltkShapeRecPtr);
		ltkShapeRecPtr = NULL;
	}

	if(m_libHandler != NULL)
	{
		LTKUnloadDLL(m_libHandler);
		m_libHandler = NULL;
	}


    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting destructor of EigDefShapeRecognizer"  << endl;
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
* Author			Date				Description
*************************************************************************************/

int EigDefShapeRecognizer::unloadModelData()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer : unloadModelData"  << endl;

	m_referenceModels.clear();
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer : unloadModelData"  << endl;

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
* Author			Date				Description
*************************************************************************************/

int EigDefShapeRecognizer::setDeviceContext(LTKCaptureDevice& deviceinfo)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer : setDeviceContext"  << endl;

	m_captureDevice = deviceinfo;


	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer : setDeviceContext"  << endl;

	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Vijayakumara M
* DATE			: 11 July 2005
* NAME			: setPreprocMap
* DESCRIPTION	: This function addes the function adderss to the preprecssor maping 
				  variable.
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

void EigDefShapeRecognizer::setPreprocMap()
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer : setPreprocMap"  << endl;

	string resample(RESAMPLE_TRACE_GROUP_FUNC);
	string preprocess(PREPROCESS_FUNC);

	PreprocMap[resample] = &EigDefShapeRecognizer::resampleTraceGroup;

	PreprocMap[preprocess] = &EigDefShapeRecognizer::preprocess;

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting EigDefShapeRecognizer : setPreprocMap"  << endl;
}

/**********************************************************************************
* AUTHOR		: Balaji R.
* DATE			: 23-DEC-2004
* NAME			: resampleTrace
* DESCRIPTION	: spatially resamples the given trace to a given number of points
* ARGUMENTS		: inTrace - trace to be resampled
*				  resamplePoints - number of points required after resampling
*				  outTrace - resampled trace
* RETURNS		: SUCCESS on successful resample operation
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeRecognizer::resampleTrace(const LTKTrace& inTrace,int resamplePoints, LTKTrace& outTrace)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::resampleTrace"  << endl;


    int pointIndex;							//	a variable to loop over the points of a trace				

	int currentPointIndex =0;				//	index of the current point

	float xSum = 0.0f;								//	sum of the x coordinate values 
	
	float ySum = 0.0f;								//	sum of the y coordinate values

	int numTracePoints;						//	number of points in a trace

	int ptIndex = 0;						//	index of point in a trace

	float x;								//	an x coord value
	
	float y;								//	an y coord value
	
	float xDiff;							//	differenc in x direction
	
	float yDiff;							//	diference in y direction
	
	float xTemp;							//	temp x storage 
	
	float yTemp;							//	temp y storage

	float unitLength = 0;					//	estimated length of each segment after resampling
	
	float pointDistance;					
	
	float balanceDistance = 0;				//	distance between the last resampled point and 
											//	the next raw sample point
	
	float measuredDistance;					
	
	float m1,m2;

	floatVector xVec;
	
	floatVector resampledXVec;

	floatVector yVec;
	
	floatVector resampledYVec;

	floatVector distanceVec;
	
	numTracePoints = inTrace.getNumberOfPoints();

	if(numTracePoints == 0)
	{
		LOG( LTKLogger::LTK_LOGLEVEL_ERR) << string(getError(EEMPTY_TRACE)) << endl;
		LTKReturnError(EEMPTY_TRACE);		
	}

	xVec = inTrace.getChannelValues(xChannelstr);
	
	yVec = inTrace.getChannelValues(yChannelstr);

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resamplePoints = " << resamplePoints << endl;


	

	if(resamplePoints < 1)
	{
		resamplePoints = 1;
	}

	if(resamplePoints == 1)
	{
    	xSum=accumulate(xVec.begin(),xVec.end(),0.0f); 
		ySum=accumulate(yVec.begin(),yVec.end(),0.0f); 

		x = xSum/ numTracePoints; //As only 1 point is required, this ratio is computed.

		y = ySum/ numTracePoints;

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "x mean = " << x << endl;

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "y mean = " << y << endl;

		resampledXVec.push_back(x);

		resampledYVec.push_back(y);

		outTrace.addChannel(resampledXVec, xChannelstr);

		outTrace.addChannel(resampledYVec, yChannelstr);
	}
	else if(numTracePoints <= 1)
	{
		x = xVec.at(0);

		y = yVec.at(0);

		for(pointIndex = 0; pointIndex < resamplePoints; ++pointIndex)
		{
			resampledXVec.push_back(x);

			resampledYVec.push_back(y);

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resampled point :" << x << y << endl;
		}

		outTrace.addChannel(resampledXVec, xChannelstr);

		outTrace.addChannel(resampledYVec, yChannelstr);
	}
	else
	{
		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Point distances"  << endl;

		for( pointIndex = 0; pointIndex < (numTracePoints-1); ++pointIndex)
		{
			xDiff = xVec.at(pointIndex) - xVec.at(pointIndex+1);

			yDiff = yVec.at(pointIndex) - yVec.at(pointIndex+1);

			pointDistance = (float) (sqrt(xDiff*xDiff + yDiff*yDiff)); //distance between 2 points.

			unitLength += pointDistance; // finding the length of trace.

			distanceVec.push_back(pointDistance); //storing distances between every 2 consecutive points.

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "point distance: " << pointDistance << endl;
		}

		unitLength /= (resamplePoints -1);

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "unitLength = " << unitLength << endl;

		x = xVec.at(0); // adding x of first point;

		y = yVec.at(0); // adding y of first point;

		resampledXVec.push_back(x);

		resampledYVec.push_back(y);

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resampled point (" << x << y << endl;

		for(pointIndex = 1; pointIndex < (resamplePoints-1); ++pointIndex)
		{
			measuredDistance = balanceDistance;

			while(measuredDistance < unitLength)
			{
				measuredDistance += distanceVec.at(ptIndex++);

				if(ptIndex == 1)
				{
					currentPointIndex = 1;
				}
				else
				{
					currentPointIndex++;
				}
			}

			if(ptIndex < 1) ptIndex = 1;

			m2 = measuredDistance - unitLength;

			m1 = distanceVec.at(ptIndex -1) - m2;

			if( fabs(m1+m2) > EPS)
			{
				xTemp =  (m1* xVec.at(currentPointIndex) + m2* xVec.at(currentPointIndex -1))/(m1+m2);

				yTemp =  (m1* yVec.at(currentPointIndex) + m2* yVec.at(currentPointIndex -1))/(m1+m2);
			}
			else
			{
				xTemp = xVec.at(currentPointIndex);

				yTemp = yVec.at(currentPointIndex);
			}

			resampledXVec.push_back(xTemp);

			resampledYVec.push_back(yTemp);

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resampled point " << xTemp << yTemp << endl;

			balanceDistance = m2;
		}

		x = xVec.at(xVec.size() - 1); // adding x of last point;

		y = yVec.at(yVec.size() - 1); // adding y of last point;

		resampledXVec.push_back(x);

		resampledYVec.push_back(y);

		LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "resampled point " << x << y << endl;

		outTrace.addChannel(resampledXVec, xChannelstr);

		outTrace.addChannel(resampledYVec, yChannelstr);
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting EigDefShapeRecognizer::resampleTrace"  << endl;
	
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
* Author            Date                Description
*************************************************************************************/
int EigDefShapeRecognizer::getLastError()
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
* Author            Date                Description
*************************************************************************************/
int EigDefShapeRecognizer::mapPreprocFunctions()
{
	stringStringMap preProcSequence;

	MapModFunc tmpMap;

	string preProcSeq;
	vector<string> moduleFuncNames;
	vector<string> modFuncs;
	vector<string> funcNameTokens;	
	
	string module, funName, sequence;
	string::size_type indx;

	LTKTraceGroup local_inTraceGroup;

	preProcSeq = shapeRecognizerProperties[PREPROCSEQUENCE];

	if(preProcSeq == "")
	{
		preProcSeq = PREPROC_DEF_SEQ_EigDef;
	}

	LTKStringUtil::tokenizeString(preProcSeq,  DELEMITER_SEQUENCE,  funcNameTokens);

	if(funcNameTokens.size() == 0)
	{
		preProcSeq = PREPROC_DEF_SEQ_EigDef;
		LTKStringUtil::tokenizeString(preProcSeq,  DELEMITER_SEQUENCE,  funcNameTokens);
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
			
			if(!module.compare("EigDef"))
			{
				if(PreprocMap[funName] != NULL)
				{
					tmpMap.moduleName = module;
					tmpMap.funcName = funName;
					mapSequence.push_back(tmpMap);				
				}
				else
				{
					LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Wrong preprocessor sequence entry in cfg file : " + funName  << endl; 
					
					LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
				}
			}
			else
			{
				if(!module.compare("CommonPreProc"))
				{
					FN_PTR_PREPROCESSOR pPreprocFunc = NULL;
					pPreprocFunc = ltkShapeRecPtr->getPreprocptr(funName);
					if(pPreprocFunc!= NULL)
					{
						tmpMap.moduleName = module;
						tmpMap.funcName = funName;
						mapSequence.push_back(tmpMap);
					}
					else
					{
						LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Wrong preprocessor sequence entry in cfg file : " + funName  << endl; 
						
						LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
					}
				}
				else
				{
					LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Wrong preprocessor sequence entry in cfg file : " + module  << endl; 
					
					LTKReturnError(EINVALID_PREPROC_SEQUENCE);	
				}
			}
		}
		else
		{
			LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "Wrong preprocessor sequence entry in cfg file : " + module  << endl; 
					
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
* Author            Date                Description
*************************************************************************************/
int EigDefShapeRecognizer::normalizeConf(float norm, float normFact, vector<LTKShapeRecoResult>& results)
{
	vector<LTKShapeRecoResult>::iterator resultIter;
	float conf;

	if(m_distanceMeasure == "normal")
	{
		for( resultIter = results.begin(); resultIter != results.end(); ++resultIter )
		{
			conf = (*resultIter).getConfidence();
			(*resultIter).setConfidence( conf / norm );
		}
	}
	else
	{
		for( resultIter = results.begin(); resultIter != results.end(); ++resultIter )
		{
			conf = (*resultIter).getConfidence();
			(*resultIter).setConfidence( (1.0/(conf*conf)) / normFact );
		}
	}
	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Ramnath 
* DATE			: 10-5-2005
* NAME			: reduceByHalf
* DESCRIPTION	: reduces the dimension of a given character by averaging adj points
* ARGUMENTS		: const Character& character, Character& outChar
* RETURNS		: SUCCESS
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description

*************************************************************************************/	

int EigDefShapeRecognizer::reduceByHalf(const Character& character, Character& outChar)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered reduceByHalf of EigDefClassifier"  << endl;

	int i,j = 0;

	for(i = 0; i < character.size(); i = i+2)
	{
		outChar[j][0] = (character[i][0] + character[i+1][0]) / 2;
		outChar[j][1] = (character[i][1] + character[i+1][1]) / 2;
		j++;
	}

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exited reduceByHalf of EigDefClassifier"  << endl;

	return SUCCESS;
}



/**********************************************************************************
* AUTHOR		: Ramnath.K.
* DATE			: 23-01-2005
* NAME			: computeEuclideanDistance
* DESCRIPTION	: computes euclidean distance between two points
* ARGUMENTS		: Features - 2
* RETURNS		: euclidean distance 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/

float EigDefShapeRecognizer:: computeEuclideanDistance(const Features& f1,const Features& f2)
{
	float xDiff, yDiff;
	xDiff = (f1[0]-f2[0]); 
	yDiff = (f1[1]-f2[1]);
	return ((xDiff*xDiff)+(yDiff*yDiff));
}


int EigDefShapeRecognizer::ComputeDTWDeformationVector(float2DVector trainCord,  float2DVector testCord, int trainSize, int testSize, floatVector& DeformVec, float& distanceDTW)
{
	//cout << "come here" << endl;
	int i,j,l, banded = 40, trunkI, trunkJ, src_ind,DIM=60;
	float **cum;
	int **src,count,Temp,local;
	float col_min1, temp1, src1[3], src_min, colThresh = 1.0,Temp1,Temp2;

	trunkI=banded;
	trunkJ=0;
	
	cum=(float **)malloc(trainSize*sizeof(float *));
	for(l=0;l< trainSize;l++)
		cum[l]=(float *)malloc(testSize*sizeof(float));
	
	src=(int **)malloc(trainSize*sizeof(int *));
	for(l=0;l< trainSize;l++)
		src[l]=(int *)malloc(testSize*sizeof(int));
			
	for(i=0;i< trainSize;i++)
		for(j=0;j< testSize;j++)
		{
			src[i][j]=0;
			cum[i][j]=100000.0;
		}
	
		
	cum[0][0]= euclidDist(trainCord[0][0],trainCord[0][1],testCord[0][0],testCord[0][1]);
	src[0][0]=4;

	for(i=1;i< testSize;i++)
	{
		cum[0][i]=cum[0][i-1]+euclidDist(trainCord[0][0],trainCord[0][1],testCord[i][0],testCord[i][1]);
		src[0][i]=3;
	}
	for(j=1;j< trainSize;j++)
	{
		cum[j][0]=cum[j-1][0]+euclidDist(trainCord[j][0],trainCord[j][1],testCord[0][0],testCord[0][1]); 
		src[j][0]=2;
	}
	
	col_min1 = 10000.0;	
	for(i=1;i< trainSize;i++)
	{
		for(j=1+trunkJ;j< testSize-trunkI;j++)
		{
			temp1= euclidDist(trainCord[i][0],trainCord[i][1],testCord[j][0],testCord[j][1]);
			src1[0]=cum[i-1][j-1]+temp1;
			src1[1]=cum[i-1][j]+temp1;
			src1[2]=cum[i][j-1]+temp1;
			src_min=10000.0;
			for(l=1;l<4;l++)
			{
				if(src_min>=src1[l-1])
				{
					src_min= src1[l-1];
					src_ind=l;
				}		
			}
			cum[i][j]= src_min;
			src[i][j]=src_ind;
			if(col_min1> src_min)
				col_min1=src_min;
		}
		if(trunkI > 0)
			trunkI--;
		if(banded+i+1> trainSize)
			trunkJ++;			
	}
	
	distanceDTW = src_min;
	
	i=0;j=0;
	count=1;
	DeformVec[DIM-1+1]=trainCord[DIM-1][0]-testCord[DIM-1][0];
	DeformVec[2*DIM-1+1]=trainCord[DIM-1][1]-testCord[DIM-1][1];
	
	Temp=src[DIM-1][DIM-1];
	
	// to find the deformation from the mapping
	while(Temp!=4)
	{
		if(Temp==1)
		{
			i+=1;
			j+=1;
			DeformVec[DIM-1-count+1]=trainCord[DIM-1-i][0]-testCord[DIM-1-j][0];
			DeformVec[2*DIM-1-count+1]=trainCord[DIM-1-i][1]-testCord[DIM-1-j][1];
			count++;
		}
		if(Temp==3)
		{
			j+=1;
		}
		if(Temp==2)
		{
			i+=1;
			DeformVec[DIM-1-count+1]=trainCord[DIM-1-i][0]-testCord[DIM-1-j][0];
			DeformVec[2*DIM-1-count+1]=trainCord[DIM-1-i][1]-testCord[DIM-1-j][1];
			count++;
		}
		Temp=src[DIM-1-i][DIM-1-j];
	}
	
	for(l=0;l< trainSize;l++)
		free(cum[l]);
	free(cum);
	for(l=0;l< trainSize;l++)
		free(src[l]);
	free(src);
	return(0);
	
}

float EigDefShapeRecognizer::euclidDist(float x1,float x2,float y1,float y2)
{
	float dist;
	dist= (x1 - y1)*(x1 - y1) + (x2 - y2)*(x2 - y2);
	return dist;
}


/**********************************************************************************
* AUTHOR		: Ramnath. K
* DATE			: 19-05-2005
* NAME			: computerDTWDistanceClusteringWrapper
* DESCRIPTION	: Wrapper function to the function whichcomputes DTW distance between 
				  two characters
* ARGUMENTS		: train character, test character
* RETURNS		: DTWDistance
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description
*************************************************************************************/
double EigDefShapeRecognizer::computeDTWDistance(Character& train,Character& test)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered EigDefShapeRecognizer::ComputeDTWDistanceClusteringWrapper"  << endl;

	float distanceDTW;

	DynamicTimeWarping<vector<float>,float,distPointer,reduceFunctionPointer> fd(FLT_MAX,computeEuclideanDistance,reduceByHalf);

	fd.initializeDTW(train.size(),0);

	#ifdef FASTDTW
		fd.computeFastDTW(train,test,RADIUS,distanceDTW);
	#else
		fd.computeDTW(train,test,FLT_MAX,distanceDTW);
	#endif

	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
	"Exited EigDefShapeRecognizer::ComputeDTWDistanceClusteringWrapper"  << endl;

	return distanceDTW;
}

/**********************************************************************************
* AUTHOR		: Ramnath & Mehul.
* DATE			: 2-2-2005
* NAME			: convTg2Char
* DESCRIPTION	: convert function - converts tracegroup to feature vector
* ARGUMENTS		: const LTKTraceGroup&,Character&
* RETURNS		: SUCCESS
* NOTES			: 
* CHANGE HISTORY
* Author			Date				Description
*************************************************************************************/

int EigDefShapeRecognizer::convertTraceGroupToCharacter(const LTKTraceGroup& traceGroupObj,Character& outChar)
{
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Entered convTg2Char of Eigdef Classifier"  << endl;

	Features featurePoint(2,0);			// obj having the features
	int numPoints;						// number of pts
	int pointIndex;
	float channelval;

	const LTKTraceVector& allTraces = traceGroupObj.getAllTraces();

	LTKTraceVector::const_iterator traceIter;
	LTKTraceVector::const_iterator traceEnd = allTraces.end();

	for (traceIter = allTraces.begin(); traceIter != traceEnd ; ++traceIter)
	{
		const floatVector& xVec = (*traceIter).getChannelValues("X");
		const floatVector& yVec = (*traceIter).getChannelValues("Y");
		numPoints = xVec.size();
		for (pointIndex =0; pointIndex <numPoints; ++pointIndex)
		{
			featurePoint[0] = xVec[pointIndex];
			featurePoint[1] = yVec[pointIndex];
			outChar.push_back(featurePoint);
		}
	}
	
	LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << 
        "Exiting convTg2Char of DTWClassifier"  << endl;

	return SUCCESS;

}


float EigDefShapeRecognizer::computeDeformDistance(floatVector DeformVec, floatVector Mean, float2DVector EigenVec, floatVector EigenValue)
{
	int k,j;
	float Distance=0.0,Temp;
	int numEigenValues = EigenValue.size();
	int traceDimension = 60;
	for(j=0; j < numEigenValues-1; j++)
	{
		Temp=0.0;
		for(k=0;k<=(2*traceDimension);k++)
		{
			Temp+=(DeformVec[k]-Mean[k])*EigenVec[j][k];
		}
		
		Distance+=(1.0/EigenValue[j]-1.0/EigenValue[numEigenValues-1])*Temp*Temp;
	}
	Temp=0.0;
	for(k=0;k<=(2*traceDimension);k++)
		Temp+=(DeformVec[k]-Mean[k])*(DeformVec[k]-Mean[k]);
	Distance+=Temp/EigenValue[numEigenValues-1];
	return(Distance/(float)(traceDimension*traceDimension));
}

bool mycmp(struct myPair x, struct myPair y)
{
	return (x.distance) < (y.distance);
}



void EigDefShapeRecognizer::assignDefaultValues()
{
	//Assigning all factory defaults...
	m_reverseFeatureVectors = PREPROC_DEF_REVERSE_FEATURE_VECTORS;
	m_traceDimension = PREPROC_DEF_TRACE_DIMENSION;
	m_numEigenVectors = PREPROC_DEF_NUMEIGEN_VECTORS;
	ltkShapeRecPtr->setFilterLength(PREPROC_DEF_SMOOTHFILTER_LENGTH);
    m_quantizationStep = PREPROC_DEF_QUANTIZATIONSTEP; 
    m_distanceMeasure = PREPROC_DEF_DISTANCEMEASURE;
	m_numClusters = 0;
}

int EigDefShapeRecognizer::readClassifierConfig()
{
	int tempIntegerVar = 0;
	float tempFloatVar = 0;
	string tempStringVar = "";

	LTKConfigFileReader* shapeRecognizerProperties = NULL;
    try
	{
		shapeRecognizerProperties = new LTKConfigFileReader(m_EigDefCfgFile);
	}
	catch(LTKException e)
	{
		delete shapeRecognizerProperties;
		// We use the default values in this case
		return SUCCESS;
	}

	if((errorCode = mapPreprocFunctions()) != SUCCESS)
	{
		shaperecUtilInst->deletePreprocessor(ltkShapeRecPtr);
		return errorCode;
	}

		//  reading eigdef configuration file

    m_reverseFeatureVectors = false;

    //  getting the preprocessing steps required

	try
	{

		tempStringVar = shapeRecognizerProperties->getConfigValue(REVERSEFEATUREVECTORS);
		if(tempStringVar == "true")
		{
			m_reverseFeatureVectors = true;

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Reverse feature vectors turned on"  << endl;
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(TRACEDIMENSION);
		if(tempStringVar != "")

		{
        		tempIntegerVar = atoi(tempStringVar.c_str());

				if(tempIntegerVar >=10 && tempIntegerVar <= 500)
				{
					m_traceDimension = tempIntegerVar;

					LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Trace dimension = " << m_traceDimension << endl;
				}
				else
				{
					LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " TRACEDIMENSION is out of permitted range"  << endl;

					throw LTKException(ECONFIG_FILE_RANGE);	
				}
		
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(NUMEIGENVECTORS);
		if(tempStringVar != "")
		{
			tempIntegerVar = atoi(tempStringVar.c_str());
			if(tempIntegerVar > 0 )
			{
				m_numEigenVectors = tempIntegerVar;

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Number of eigenvectors = " << m_numEigenVectors << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " NUMEIGENVECTORS is out of permitted range"  << endl;
				
				throw LTKException(ECONFIG_FILE_RANGE);	
			}

		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(NUMCLUSTERS);
		if(tempStringVar != "")
		{
			tempIntegerVar = atoi(tempStringVar.c_str());
			if(tempIntegerVar > 0 )
			{
				m_numClusters = tempIntegerVar;

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Number of clusters = " << m_numClusters << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " Number of clusters is out of permitted range"  << endl;
				
				throw LTKException(ECONFIG_FILE_RANGE);	
			}

		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(NORMALIZEDSIZE);
		if(tempStringVar !="")
		{
			tempFloatVar=atof(tempStringVar.c_str());

			if(tempFloatVar >=1  && tempFloatVar <=10)
			{
				ltkShapeRecPtr->setNormalizedSize(tempFloatVar);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "NormalizedSize = " + tempStringVar << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " NORMALIZEDSIZE is out of permitted range"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);	
			}
		}
		
		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(SIZETHRESHOLD);
		if(tempStringVar != "")
		{
			tempFloatVar=atof(tempStringVar.c_str());

			if(tempFloatVar  >= 0 && tempFloatVar <= 1 )
			{
				ltkShapeRecPtr->setSizeThreshold(tempFloatVar);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Size threshold = " + tempStringVar << endl;
			}
			else 
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " SIZETHRESHOLD should be atleast less than the 1/100'th of  NORMALIZEDSIZE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			}
		}
    
		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(ASPECTRATIOTHRESHOLD);
		if(tempStringVar != "")
		{
			tempFloatVar=atof(tempStringVar.c_str());
			
			if(tempFloatVar >= 0)
			{
				ltkShapeRecPtr->setAspectRatioThreshold(tempFloatVar);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Aspect ration threshold = " + tempStringVar << endl;
			}
			else
			{
					LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " ASPECTRATIOTHRESHOLD should be positive"  << endl;

					throw LTKException(ECONFIG_FILE_RANGE);
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(DOTTHRESHOLD);
		if(tempStringVar != "")
		{
			tempFloatVar=atof(tempStringVar.c_str());
			
			if(tempFloatVar >= 0 && tempFloatVar <= 1)
			{
				ltkShapeRecPtr->setDotThreshold(tempFloatVar);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Dot threshold = " + tempStringVar << endl;
			}
			else 
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " DOTTHRESHOLD should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			}
			
		}
    
		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(LOOPTHRESHOLD);
		if(tempStringVar != "")
		{
			tempFloatVar = atof(tempStringVar.c_str());

			if(tempFloatVar >= 0 && tempFloatVar <= 1)
			{
				ltkShapeRecPtr->setLoopThreshold(tempFloatVar);

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Loop threshold = " + tempStringVar << endl;
			}
			else
			{
				
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " LOOPTHRESHOLD should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(HOOKLENGTHTHRESHOLD1);
		if(tempStringVar != "")
		{
			tempFloatVar = atof(tempStringVar.c_str());
			if(tempFloatVar >= 0 && tempFloatVar <= 1)
			{
				ltkShapeRecPtr->setHookLengthThreshold1(tempFloatVar);
				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Hook length threshold 1 = " + tempStringVar << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " HOOKLENGTHTHRESHOLD1 should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(HOOKLENGTHTHRESHOLD2);
		if(tempStringVar != "")
		{
			tempFloatVar = atof(tempStringVar.c_str());
			if(tempFloatVar >= 0 && tempFloatVar <= 1)
			{
				ltkShapeRecPtr->setHookLengthThreshold2(tempFloatVar);
				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Hook length threshold 2 = " + tempStringVar << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " HOOKLENGTHTHRESHOLD2 should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(HOOKANGLETHRESHOLD);
		if(tempStringVar != "")
		{
			tempFloatVar = atof(tempStringVar.c_str());
			if(tempFloatVar >0)
			{
				ltkShapeRecPtr->setHookAngleThreshold(tempFloatVar);
				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Hook angle threshold = " + tempStringVar << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << " HOOKANGLETHRESHOLD should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(SMOOTHFILTERLENGTH);
		if(tempStringVar != "")
		{
			tempIntegerVar = atoi(tempStringVar.c_str());
			if(tempIntegerVar >= 1)
			{

				ltkShapeRecPtr->setFilterLength(tempIntegerVar);
				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Length of smoothening filter = " + tempStringVar << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "SMOOTHFILTERLENGTH  should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(QUANTIZATIONSTEP);
		if(tempStringVar != "")
		{
			tempIntegerVar = atoi(tempStringVar.c_str());
			if(tempIntegerVar >= 1)
			{
				m_quantizationStep = tempIntegerVar;

				LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Quantization step for resampling = " + tempStringVar << endl;
			}
			else
			{
				LOG( LTKLogger::LTK_LOGLEVEL_ERR) << "QUANTIZATIONSTEP  should be POSITIVE"  << endl;

				throw LTKException(ECONFIG_FILE_RANGE);
			}
		}

		tempStringVar = "";
		tempStringVar = shapeRecognizerProperties->getConfigValue(DISTANCEMEASURE);
		if(tempStringVar != "")
		{
       
			m_distanceMeasure = tempStringVar;

			LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Distance Measure = " + tempStringVar << endl;
			
		}
	}
	catch(LTKException e)
	{
		delete shapeRecognizerProperties;
		LTKReturnError(ECONFIG_FILE_RANGE);
	}

    
    LOG( LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting EigDefShapeRecognizer::InitializeEigDefShapeRecognizer"  << endl;
	delete shapeRecognizerProperties;
    return SUCCESS;

}
