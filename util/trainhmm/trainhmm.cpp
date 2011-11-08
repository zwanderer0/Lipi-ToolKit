
#include "trainhmm.h"
#include "LTKStringUtil.h"
#include "LTKConfigFileReader.h"
#include "LTKException.h"

using namespace std;

string trainMDTFilePath;
string testMDTFilePath;
string outputDirectory;
string strNumFeatures;
string strNumRecoChoices;
string strNumPointsPerState;
string strNumGaussiansPerState;
string strFeatExtractorName;


char strFeatureFile[MAX_PATH] = "";
char strProjectName[MAX_PROJECT_NAME] = "";
char strProfileName[MAX_PROFILE_NAME] = "";

char strLipiRootPath[MAX_PATH];

bool bVersionRequest = false;


char **globalArg;
int globalArgCount = 0;


void printUsage()
{
	printf("\n\nUsage : trainhmm\n");
	printf("\ntrainhmm\n");
	printf("\n	-project <projectname>\n");
	printf("\n	-featurefile <filename>\n");
	printf("\n	[-lipiroot <root name of the lipitk>]\n");
	printf("\n	[-profile <profilename>]\n\n");
	
}




int validateAndPopulateConfigValues(const string& lipiRootPath)
{

	string strHMMConfigFile = string(lipiRootPath) + PROJECTS_PATH_STRING +
					   string(strProjectName) + PROFILE_PATH_STRING +
					   string(strProfileName) + SEPARATOR +
					   HMM + CONFIGFILEEXT;

	LTKConfigFileReader* configReader;
	
	try
	{

		configReader = new LTKConfigFileReader(strHMMConfigFile);

	}
	catch(...)
	{
		cout<<"Error reading "<<HMM<<" config file:"<<strHMMConfigFile<<endl;
		return 1;
	}

	
	int errorCode;

	errorCode = configReader->getConfigValue("NumPointsPerState",strNumPointsPerState);

	if(errorCode != 0)
	{
		cout<<"Attribute NumPointsPerState is missing in the config file."<<endl;
		return 1;
	}

	if(atoi(strNumPointsPerState.c_str()) <=0 )
	{
		cout<<"Invalid value for NumPointsPerState:"<<strNumPointsPerState<<" in the config file"<<endl;
		return 1;
	}

	errorCode = configReader->getConfigValue("NumGaussiansPerState",strNumGaussiansPerState);

	if(errorCode != 0)
	{
		cout<<"Attribute NumGaussiansPerState is missing in the config file."<<endl;
		return 1;
	}

	if(atoi(strNumGaussiansPerState.c_str()) <=0 )
	{
		cout<<"Invalid value for NumGaussiansPerState:"<<strNumGaussiansPerState<<" in the config file"<<endl;
		return 1;
	}

	errorCode = configReader->getConfigValue(FEATUREEXTRACTOR,strFeatExtractorName);

	if(errorCode != 0)
	{
		cout<<"Attribute "<<FE_NAME<<" is missing in the config file."<<endl;
		return 1;
	}

	ifstream tempInput(strFeatureFile);

	string firstLine;

	if(tempInput)
	{
		getline(tempInput,firstLine,'\n');
	}

	vector<string> tokens;

	LTKStringUtil::trimString(firstLine);

	if(firstLine.size() <=2) //ensuring size before stripping the first and last angle brackets
	{
		cout<<"No header information in the feature file"<<endl;
		return 1;
	}


	firstLine = firstLine.substr(1,firstLine.size()-2); //removing the first and last angle brackets

	LTKStringUtil::tokenizeString(firstLine,"= ",tokens);

	if(tokens.size()!=2 && tokens[0]!=FE_NAME)
	{
		cout<<"Feature extractor name is missing in the feature file."<<endl;
		return 1;
	}

	if(tokens[1] != strFeatExtractorName)
	{
		cout << "Feature extractor name in feature file does not match with config file."<<endl;
		return 1;
	}
	

	tempInput.close();
	
	
	return 0;
}


int processCommandLineArgs()
{
	bool bProfileAssumed = true;

	// Minimum MIN_ARGS arguments must...
	if(globalArgCount >= MIN_ARGS)
	{
		if(LTKSTRCMP(globalArg[1], OPTION_VERSION) == 0)
		{
			bVersionRequest = true;
			return 0;
		}
	}

	for(int i = 1; i < globalArgCount; i++)
	{
		if((LTKSTRCMP(globalArg[i], OPTION_HELP1) == 0) || 
            (LTKSTRCMP(globalArg[i], OPTION_HELP2) == 0))
		{
			return 1;
		}

		if(LTKSTRCMP(globalArg[i], OPTION_VERSION) == 0)
		{
			bVersionRequest = true;
			return 0;
		}

		if(LTKSTRCMP(globalArg[i], OPTION_PROJECT) == 0)
		{
			if(getStringValue(i, strProjectName) != 0)
			{
				cout << "Invalid or no value specified for -project option." << endl;
				return 1;
			}
			else { i++; continue;}
		}

		if(LTKSTRCMP(globalArg[i], OPTION_PROFILE) == 0)
		{
			if(getStringValue(i, strProfileName) != 0)
			{
				cout << "Invalid or no value specified for -profile option." << endl;
				return 1;
			}
			else { i++; bProfileAssumed = false; continue;}
		}

		if(LTKSTRCMP(globalArg[i], OPTION_FEATURE_FILE) == 0)
		{
			if(getStringValue(i, strFeatureFile) != 0)
			{
				cout << "Invalid or no value specified for -featurefile option." << endl;
				return 1;
			}
			else { i++; continue;}
		}

		if(LTKSTRCMP(globalArg[i], LIPI_ROOT) == 0)
		{
			if(getStringValue(i, strLipiRootPath) != 0)
			{
				cout << "Invalid or no value specified for -lipiroot option." << endl;
				return 1;
			}
			else { i++;  continue;}
		}
		
		cout << globalArg[i] << " : Invalid or Unknown option." << endl;
		return 1;
	}

	
	if(LTKSTRCMP(strProjectName, "") == 0)
	{
		cout << "No project name specified using -project." << endl;
		return 1;
	}

	if(LTKSTRCMP(strFeatureFile, "") == 0)
	{
		cout << "-featurefile option is required to run this." << endl;
		return 1; // No option specified for -featurefile
	}

	
	if(LTKSTRCMP(strProfileName, "") == 0)
		strcpy(strProfileName, DEFAULT_PROFILE);

	if(bProfileAssumed)
		cout << "Profile not specified, assuming " 
		     <<  DEFAULT_PROFILE << " profile." << endl;

	return 0;
}

int getStringValue(int index, char* strOption)
{
	if(index > globalArgCount)
		return 1;

	if(index + 1 >= globalArgCount) // set to default...
		return 1;

	if(strlen(globalArg[index+1]) >= MAX_PATH)
		strncpy(strOption, globalArg[index], MAX_PATH);
	else
	{
		if(strlen(globalArg[index+1]) <= 1)
			return 1;

		// Checking the value 
		if(checkForOption(globalArg[index+1]) != 0) 
			return 1;

		strcpy(strOption, globalArg[index+1]);
	}

	return 0;
}

int checkForOption(char* strVal)
{
	if( (LTKSTRCMP(strVal, OPTION_PROJECT) == 0) ||
		(LTKSTRCMP(strVal, OPTION_PROFILE) == 0) ||
		(LTKSTRCMP(strVal, LIPI_ROOT) == 0) ||
		(LTKSTRCMP(strVal, OPTION_FEATURE_FILE) == 0))
		return 1;

	return 0;
}


int main(int argc, char** argv)
{

	char *envstring = NULL;
	globalArg = argv;
	globalArgCount = argc;

	strcpy(strLipiRootPath,"");

	if(processCommandLineArgs() != 0)
	{
		printUsage();
        return 1;
	}


	/* Get the LIPI_ROOT environment variable */
	envstring = strLipiRootPath;

	if(strlen(envstring) == 0)
	{
		envstring = getenv(LIPIROOT_ENV_STRING);

		if(envstring == NULL )
		{
			cout << "Error, Environment variable LIPI_ROOT is not set" << endl;
            return 1;
		}
		strcpy(strLipiRootPath, envstring );

	}


	ifstream inputFeatureFile(strFeatureFile);

	if(inputFeatureFile == NULL)
	{
		cout << "Unable to access feature file: " << strFeatureFile << endl;
		return 1;
	}
	
	inputFeatureFile.close();

	
	string profileCfgPath = string(strLipiRootPath) + PROJECTS_PATH_STRING + 
						string(strProjectName) + PROFILE_PATH_STRING + 
						string(strProfileName)+ SEPARATOR + PROFILE_CFG_STRING;



	LTKConfigFileReader* profileConfigReader = NULL;
	
	try
	{
		profileConfigReader = new LTKConfigFileReader(profileCfgPath);
	}
	catch(LTKException e)
	{
		cout<<"Unable to read profile cfg file:"<<profileCfgPath <<endl;

		return 1;
	}

	string outRecognizerString = "";

	int errorCode = profileConfigReader->getConfigValue(SHAPE_RECOGNIZER_STRING, 
													outRecognizerString);
	
	if(errorCode != SUCCESS)
	{
		if(outRecognizerString == "")
		{
			cout<<"No value for "<<SHAPE_RECOGNIZER_STRING<<" in the profile file:"<<profileCfgPath<<endl;
			return 1;
		}
		else if(outRecognizerString != HMM)
		{
			cout<<"The value for "<<SHAPE_RECOGNIZER_STRING<<" in the profile file:"<<profileCfgPath<<
				" is not "<<HMM<<". Use the runshaperec tool for training other recognizers."<<endl;
		}
	
		delete profileConfigReader;		

		return 1;
	}

	delete profileConfigReader;


	if(validateAndPopulateConfigValues(strLipiRootPath) != 0)
	{
		cout<<"Error while reading config file"<<endl;
		return 1;
	}

	cout<<"Creating feature files in HTK format..."<<endl;

	string tempDir = strLipiRootPath+string(SEPARATOR)+string("bin")+string(SEPARATOR)+string("temp");

	int numFeatures = 0;

	if(createHTKData(strLipiRootPath,strFeatureFile,tempDir,numFeatures) != 0)
	{
		cout<<"Error while creating feature files in HTK format"<<endl;
		return 1;
	}

	cout<<"Feature file creation complete"<<endl;

	string strProfileDir = string(strLipiRootPath) + PROJECTS_PATH_STRING +
					   string(strProjectName) + PROFILE_PATH_STRING +
					   string(strProfileName);

	cout<<"Training HMMs using HTK..."<<endl;

	
	
	if(htkTrain(strLipiRootPath,tempDir,atoi(strNumPointsPerState.c_str()),atoi(strNumGaussiansPerState.c_str()),numFeatures,strProfileDir) != 0)
	{
		cout<<"Error while training HMM using HTK"<<endl;
		return 1;
	}

	cout<<"HMM training DONE!!"<<endl;

	cout<<"Deleting temporary folder..."<<endl;

	string cmdLine="";

	#ifdef WIN32
		cmdLine = "rmdir /s /q "+tempDir;
	#else
		cmdLine = "rm -r "+tempDir;
	#endif

	

	system(cmdLine.c_str());

	cout<<"Deleted."<<endl;

	cout<<"Output HMM MDT file may be found at:"<<strProfileDir<<SEPARATOR<<HMM<<DATFILEEXT<<endl;

	return 0;
}
