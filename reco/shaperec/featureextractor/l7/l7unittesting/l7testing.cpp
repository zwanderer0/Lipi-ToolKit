/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-02-20 10:03:51 +0530 (Wed, 20 Feb 2008) $
 * $Revision: 423 $
 * $Author: sharmnid $
 *
 ************************************************************************/

#include "LTKInc.h"
#include "LTKMacros.h"
#include "LTKTypes.h"
#include "LTKTraceGroup.h"
#include "LTKLoggerUtil.h"
#include "LTKShapeFeatureExtractorFactory.h"
#include "LTKShapeFeatureMacros.h"
#include "LTKShapeFeature.h"
#include "LTKShapeFeatureExtractor.h"
#include "LTKShapeRecoUtil.h"
#include "LTKCaptureDevice.h"
#include "LTKScreenContext.h"
#include "L7ShapeFeatureExtractor.h"
#include "LTKImageWriter.h"

#define OPTION_CFG_FILE_PATH "-cfg"
#define LIPI_ROOT      "-lipiroot"

char **globalArg;
int globalArgCount = 0;
string g_lipiRoot = "";
string g_dataDirectory = "src" + string(SEPARATOR) + "reco" + string(SEPARATOR) + 
                         "shaperec" + string(SEPARATOR) + "featureextractor" + 
                         string(SEPARATOR) + "l7" + string(SEPARATOR) +
                         "l7unittesting" + string(SEPARATOR) + "testData";
                           

LTKLogger::EDebugLevel debLevel = LTKLogger::LTK_LOGLEVEL_DEBUG;

int readCommandLineArguments(LTKControlInfo& controlInfo);
void testEmptyTraceGroupExtraction(LTKShapeFeatureExtractor *);
void testSize1TraceGroupExtraction(LTKShapeFeatureExtractor *);
void printFeatureVector(vector < LTKShapeFeaturePtr >&  featureVector);
void testRender(LTKShapeFeatureExtractor *m_ptrFeatureExtractor);
int readInkToTraceGroup(const string& inkFilePath, LTKTraceGroup& inTraceGroup);

int main(int argc, char* argv[])
{
	globalArg = argv;
	globalArgCount = argc;
    LTKControlInfo controlInfo;
    LTKShapeFeatureExtractor* m_ptrFeatureExtractor = NULL;
    

	cout << "included l7 feature extractor " << endl;

	int errorCode = readCommandLineArguments(controlInfo);

	if (errorCode != SUCCESS)
    {
		return (errorCode);
    }

	LTKShapeFeatureExtractorFactory factory;
    void* m_libHandlerFE = NULL;

    g_lipiRoot = controlInfo.lipiRoot;

    errorCode = factory.createFeatureExtractor(NAME_L7_SHAPE_FEATURE_EXTRACTOR,
                                                   controlInfo.lipiRoot,
                                                   &m_libHandlerFE,
                                                   controlInfo,
                                                   &m_ptrFeatureExtractor);

    if (errorCode != SUCCESS)
    {
        cout << "Could not create feature extractor : " << errorCode<< endl;
        return (errorCode);
    }


	testEmptyTraceGroupExtraction(m_ptrFeatureExtractor);

    testSize1TraceGroupExtraction(m_ptrFeatureExtractor);

	testRender(m_ptrFeatureExtractor);

	return 0;
}


/**************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 14-Feb-2008
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		:
* RETURNS		:
* NOTES			:
* CHANGE HISTROY
* Author			Date
***************************************************************************/
int readCommandLineArguments(LTKControlInfo& controlInfo)
{
	
	if (globalArgCount !=5)
	{
		cout << "Invalid number of arguments " << globalArgCount << endl;
		return FAILURE;
	}
	

	for (int i=0; i<globalArgCount; i++)
	{
        if(LTKSTRCMP(globalArg[i], OPTION_CFG_FILE_PATH) == 0 && i+1 <globalArgCount)
		{
		    string tempStr(globalArg[i+1]);
			if(tempStr.empty())
			{
				cout << "Invalid or no value specified for -project option." << endl;
				return -1;
			}
			else 
            {
                controlInfo.cfgFilePath = tempStr;
            }
		}

        // Read Lipi Root

        if(LTKSTRCMP(globalArg[i], LIPI_ROOT) == 0 && i+1 <globalArgCount)
		{
		    string tempStr(globalArg[i+1]);
			if(tempStr.empty())
			{
				cout << "Invalid or no value specified for -LIPI_ROOT option." << endl;
				return -1;
			}
			else 
            {
                controlInfo.lipiRoot = tempStr;
            }
		}
	}

	return SUCCESS;
}

/**************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 14-Feb-2008
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		:
* RETURNS		:
* NOTES			:
* CHANGE HISTROY
* Author			Date
***************************************************************************/
void testEmptyTraceGroupExtraction(LTKShapeFeatureExtractor* m_ptrFeatureExtractor)
{
    LTKTraceGroup traceGroup;
	vector < LTKShapeFeaturePtr >  featureVector;

    int errorCode = m_ptrFeatureExtractor->extractFeatures(traceGroup, featureVector);
	cout << "feature vector size " << featureVector.size()  << "error " << errorCode << endl;
}

/**************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 14-Feb-2008
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		:
* RETURNS		:
* NOTES			:
* CHANGE HISTROY
* Author			Date
***************************************************************************/
void testSize1TraceGroupExtraction(LTKShapeFeatureExtractor *m_ptrFeatureExtractor)
{

	cout << "**********************************************" << endl;
	cout << endl;
	cout << "testSize1TraceGroupExtraction " << endl;
	
    LTKTraceGroup  traceGroup;

    string dataDirectory = g_lipiRoot + SEPARATOR + g_dataDirectory + SEPARATOR;
	string fileName = "traceGroupWithOnePoint.txt";
    
    string inkFilePath = dataDirectory + fileName;
    
    int errorCode = readInkToTraceGroup(inkFilePath, traceGroup); 

    if (errorCode != SUCCESS)
    {
        cout << " Error : " << errorCode << " : " << 
            getErrorMessage(errorCode) << "reading ink file " << inkFilePath << endl;

        return;
    }


    vector < LTKShapeFeaturePtr >  featureVector;
    
    m_ptrFeatureExtractor->extractFeatures(traceGroup, featureVector);

    cout << "After feature extraction " << endl;
	cout << endl;
    cout << "The output feature vector size " << featureVector.size() << endl;

    printFeatureVector(featureVector);
    
    cout << "**********************************************" << endl;
}

/**************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 14-Feb-2008
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		:
* RETURNS		:
* NOTES			:
* CHANGE HISTROY
* Author			Date
***************************************************************************/
void printFeatureVector(vector < LTKShapeFeaturePtr >&  featureVector)
{
    vector < LTKShapeFeaturePtr >::iterator iter = featureVector.begin();
    vector < LTKShapeFeaturePtr >::iterator iterEnd = featureVector.end();

    for(; iter != iterEnd ; iter++)
    {
        string strFeature;
        (*iter)->toString(strFeature);
        cout << strFeature << endl;
        
    }
}

/**************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 18-Feb-2008
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		:
* RETURNS		:
* NOTES			:
* CHANGE HISTROY
* Author			Date
***************************************************************************/
void testRender(LTKShapeFeatureExtractor *m_ptrFeatureExtractor)
{

	cout << "**********************************************" << endl;
	cout << endl;
	cout << "testRender " << endl;
	
	string dataDirectory = g_lipiRoot + SEPARATOR + g_dataDirectory + SEPARATOR;
	string fileName = "multiStrokeCharacter.unp";
    
    string imgFileNameAfterExtraction =  dataDirectory + "ultiStrokeCharacterAfterExt.bmp";
    string imgFileNameBeforeExtraction = dataDirectory + "ultiStrokeCharacterBeforeExt.bmp";

    string inkFilePath = dataDirectory + fileName;
    
    LTKTraceGroup  traceGroup, outTraceGroup;

    int errorCode = readInkToTraceGroup(inkFilePath, traceGroup); 

    if (errorCode != SUCCESS)
    {
        cout << " Error : " << errorCode << " : " << 
            getErrorMessage(errorCode) << "reading ink file " << inkFilePath << endl;

        return;
    }

    LTKImageWriter iwObj;

    iwObj.drawLTKTraceGroupToImage(traceGroup, imgFileNameBeforeExtraction);    
    
    vector < LTKShapeFeaturePtr >  featureVector;
    
    m_ptrFeatureExtractor->extractFeatures(traceGroup, featureVector);

    m_ptrFeatureExtractor->convertFeatVecToTraceGroup(featureVector,outTraceGroup);

    

    iwObj.drawLTKTraceGroupToImage(outTraceGroup, imgFileNameAfterExtraction);
    
    cout << "After feature extraction " << endl;
	cout << endl;
    cout << "The output feature vector size " << featureVector.size() << endl;

    printFeatureVector(featureVector);
    
    cout << "**********************************************" << endl;
}


/**************************************************************************
* AUTHOR		: Nidhi Sharma
* DATE			: 19-Feb-2008
* NAME			: 
* DESCRIPTION	: 
* ARGUMENTS		:
* RETURNS		:
* NOTES			:
* CHANGE HISTROY
* Author			Date
***************************************************************************/
int readInkToTraceGroup(const string& inkFilePath, LTKTraceGroup& inTraceGroup)
{
    LTKShapeRecoUtil utilObj;
	
    LTKCaptureDevice  captureDevice;
    LTKScreenContext screenContext;
    
    int errorCode = utilObj.readInkFromFile(inkFilePath, 
                                            g_lipiRoot, 
                                            inTraceGroup, 
                                            captureDevice, 
                                            screenContext);


    cout<< "Input TraceGroup size :\t" << inTraceGroup.getNumTraces() << endl;	
	cout << endl;
    
    return errorCode;
}
