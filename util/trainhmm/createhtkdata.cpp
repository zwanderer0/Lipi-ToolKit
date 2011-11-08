
#include "createhtkdata.h"

struct ClassDetails
{
long totalNumPoints;
int numSamples;
};

map<int,ClassDetails> classStatistics;



int createHTKData(const string& lipiRootPath,const string& featureFilePath,const string& tempDir,int& outNumFeatures)
{

	string htkFeatFilesDestDir;
	string strNumFeatures;
	string statisticsFilePath;
	string cmdLine;

	string htkInstallationDir;

	#ifdef WIN32

		htkInstallationDir = lipiRootPath + SEPARATOR + "externaltools" + SEPARATOR + "htk" + SEPARATOR + "windows";
	#else
		htkInstallationDir = lipiRootPath + SEPARATOR + "externaltools" + SEPARATOR + "htk" + SEPARATOR + "linux";
	#endif
	
	
	string htkConfigFilePath = htkInstallationDir+string(SEPARATOR)+"config";

	ifstream checkHTKConfigExists(htkConfigFilePath.c_str());

	if(!checkHTKConfigExists)
	{
		cout<<"Unable to access HTK's config file:"<<htkConfigFilePath<<endl;
		return 1;
	}

	checkHTKConfigExists.close();

	cmdLine = "mkdir "+tempDir;

	system(cmdLine.c_str());

	statisticsFilePath = tempDir + string(SEPARATOR) + "class-train-statistics.txt";

	ofstream doesExist(statisticsFilePath.c_str());

	if(!doesExist)
	{

		cout<<"Unable to create files at:"<<tempDir<<endl;
		return 1;
	}

	doesExist.close();


	htkFeatFilesDestDir = tempDir+string(SEPARATOR)+"htktraindata";

	cmdLine = "mkdir "+htkFeatFilesDestDir;

	system(cmdLine.c_str());

	ifstream input(featureFilePath.c_str());

	if(input)
	{
		string eachLine;
		getline(input,eachLine,'\n'); //simply reading the first line of the feature file

		bool isSecondLine = true;

		int classId;

		int numPoints;

		int lineNumber = 1;

		while(input)
		{
			eachLine = "";

			++lineNumber;

			getline(input,eachLine,'\n');

			if(!eachLine.empty())
			{
				vector<string> tokens;
				LTKStringUtil::tokenizeString(eachLine," ",tokens);

				if(tokens.size()!=2)
				{
					cout<<"Error reading feature file at line number:"<<lineNumber<<endl;
					return 1;
				}

				classId  =atoi(tokens[0].c_str());

				vector<string> strFeaturePointsVec;
				LTKStringUtil::tokenizeString(tokens[1],"|",strFeaturePointsVec);

				numPoints = strFeaturePointsVec.size();
								

				if(numPoints <=0)
				{
					cout<<"Error reading feature file at line number:"<<lineNumber<<endl;
					return 1;
				}

				if(isSecondLine)
				{
					vector<string> tempTokens;
					LTKStringUtil::tokenizeString(strFeaturePointsVec[0],",",tempTokens);
					char buff[50];
					sprintf(buff,"%d",tempTokens.size()-1); //-1 to ignore the pen-up bit feature
					strNumFeatures = string(buff);
					outNumFeatures = atoi(strNumFeatures.c_str());
					isSecondLine = false;
				}

				if(classStatistics.find(classId)!=classStatistics.end())
				{
					  classStatistics[classId].numSamples+=1;
					  classStatistics[classId].totalNumPoints+=numPoints;

				}
				else
				{
					 ClassDetails cd;
					 cd.numSamples=1;
					 cd.totalNumPoints=numPoints;
					 classStatistics[classId]=cd;
					 string classDir=htkFeatFilesDestDir+SEPARATOR+tokens[0];
					 
					 cmdLine = "mkdir "+classDir;
					 system(cmdLine.c_str());
				}
				string tempFilePath = tempDir+string(SEPARATOR)+"tmp.txt";

				ofstream tempOutput(tempFilePath.c_str());

				for(int i=0;i<strFeaturePointsVec.size();++i)
				{
					vector<string> tempTokens;
					LTKStringUtil::tokenizeString(strFeaturePointsVec[i],",",tempTokens);

					for(int ii=0;ii<(tempTokens.size()-1);++ii) //-1 to ignore the pen-up bit feature
					{
						tempOutput<<tempTokens[ii]<<" ";
					}
				}
				tempOutput<<endl;
				tempOutput.close();

				char buffer[100];
				sprintf( buffer,"%03d",(classStatistics[classId].numSamples)-1);
				string strSampleIndex(buffer);

				char buff[100];
				sprintf(buff,"%d",strFeaturePointsVec.size());
				string strNumPoints(buff);

				string htkFeatFilePath=htkFeatFilesDestDir+SEPARATOR+tokens[0]+string(SEPARATOR)+"train_class"+tokens[0]+"_"+strSampleIndex;


				cmdLine=htkInstallationDir+string(SEPARATOR)+"ascii_htk "+tempFilePath+" "+strNumFeatures+" "+strNumPoints+" "+htkFeatFilePath;
				system(cmdLine.c_str());
				remove(tempFilePath.c_str());


			}
		}
	}
	else
	{
		cout<<"Unable to acess feature file:"<<featureFilePath<<endl;
		return 1;
	}

	input.close();

   ofstream output(statisticsFilePath.c_str());
   output<<"#Class_ID AVG_NUM_POINTS NUM_SAMPLES"<<endl;
   for(map<int,ClassDetails>::iterator iter=classStatistics.begin();iter!=classStatistics.end();++iter)
   {
	output<<iter->first<<" ";
	int numOfSamples=(iter->second).numSamples;
	int avgNumPoints=(iter->second).totalNumPoints/numOfSamples;
	output<<avgNumPoints<<" "<<numOfSamples<<endl;
   }
   output.close();
	
   return 0;

}