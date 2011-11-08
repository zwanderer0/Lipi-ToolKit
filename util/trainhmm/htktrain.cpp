
#include "htktrain.h"

vector<int> classIdsVec;

void createHMMPrototype(string outputFilePath,int numberOfStates,int numberOfGaussians,int vecSize,float selfTransProb,string prototypeName)
{
    ofstream output(outputFilePath.c_str());
    output<<"~o <VecSize> "<<vecSize<<" <USER> <StreamInfo> 1 "<<vecSize<<endl;
    output<<"~h \""<<prototypeName<<"\""<<endl;
    output<<"<BeginHMM>"<<endl;
    output<<"<NumStates> "<<numberOfStates<<endl;
    float eachGaussianProb=1.0/numberOfGaussians;
    for(int s=2;s<numberOfStates;++s)
    {
         output<<"<State> "<<s<<" <NumMixes> "<<numberOfGaussians<<endl;
         output<<"<Stream> 1"<<endl;
         for(int g=1;g<=numberOfGaussians;++g)
         {
          output<<"<Mixture> "<<g<<" "<<eachGaussianProb<<endl;
          output<<"<Mean> "<<vecSize<<endl;
          for(int m=1;m<=vecSize;++m)
          {
           output<<"0.0 ";
          }
          output<<endl;
          output<<"<Variance> "<<vecSize<<endl;
          for(int v=1;v<=vecSize;++v)
          {
           output<<"1.0 ";
          }
          output<<endl;
         }

    }
    output<<"<TransP> "<<numberOfStates<<endl;
    for(int i=1;i<=numberOfStates;++i)
    {
         for(int j=1;j<=numberOfStates;++j)
         {
              if(i==1 && j==2)
              {
                 output<<"1.0 ";
              }
              else if(i==j && i!=numberOfStates && i!=1)
              {
                 output<<selfTransProb<<" ";
              }
              else if(j==(i+1) && i!=numberOfStates)
              {
               output<<(1-selfTransProb)<<" ";
              }
              else
              {
               output<<"0.0 ";
              }
         }
         output<<endl;
    }
    output<<"<EndHMM>"<<endl;

    output.close();
}



int htkTrain(const string& lipiRootPath,const string& tempDir,int numPointsPerState,int numGaussiansPerState,int vecSize,const string& strProfileDir)
{


	string classStatisticsFilePath =  tempDir + string(SEPARATOR)+"class-train-statistics.txt";

	ifstream doesExist(classStatisticsFilePath.c_str());

	if(!doesExist)
	{
		cout<<"Unable to locate statistics file:"<<classStatisticsFilePath<<endl;
		return 1;
	}

	doesExist.close();

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

	string htkTrainedDataDir=tempDir+string(SEPARATOR)+"htktraindata";

	string trainListFilesDir=tempDir+string(SEPARATOR)+"trainlistfiles";


	if(numPointsPerState <= 0)
	{
		cout<<"Invalid value for numPointsPerState:"<<numPointsPerState<<endl;
		return 1;
	}


	if(numGaussiansPerState <= 0)
	{
		cout<<"Invalid value for numGaussiansPerState:"<<numGaussiansPerState<<endl;
		return 1;
	}


	if(vecSize <= 0)
	{
		cout<<"Invalid value for vecSize:"<<vecSize<<endl;
		return 1;
	}

	string prototypeHMMFilePath;

	ifstream statisticsInput(classStatisticsFilePath.c_str());

	if(!statisticsInput)
	{
		cout<<"Class statistics file not found..exiting"<<endl;
		return 1;
	}

	string cmdLine = "mkdir "+trainListFilesDir;
	system(cmdLine.c_str());

	cmdLine = "mkdir "+tempDir +string(SEPARATOR)+"hmms";
	system(cmdLine.c_str());

	cmdLine = "mkdir "+tempDir + string(SEPARATOR)+"hmms"+string(SEPARATOR)+"initialized";
	system(cmdLine.c_str());

	cmdLine = "mkdir "+tempDir + string(SEPARATOR)+"hmms"+string(SEPARATOR)+"trained";
	system(cmdLine.c_str());


		while(statisticsInput)
		{
		  string eachLine="";
		  getline(statisticsInput,eachLine,'\n');
		  if(!eachLine.empty())
		  {
			   if(eachLine[0]!='#')
			   {
					vector<string> eachLineTokens;
					LTKStringUtil::tokenizeString(eachLine," ",eachLineTokens);

					int numSamples=0;
					float avgNumPoints=0;
					int classID=0;

					sscanf(eachLineTokens[0].c_str(),"%d",&classID);
					sscanf(eachLineTokens[1].c_str(),"%f",&avgNumPoints);
					sscanf(eachLineTokens[2].c_str(),"%d",&numSamples);

					classIdsVec.push_back(classID);

					int estimatedNumStates=(int)floor((avgNumPoints/(float)numPointsPerState)+0.5f); //assuming each state can model numPointsPerState points
					if(estimatedNumStates==0)
					{
					 estimatedNumStates=1;
					}

					estimatedNumStates=estimatedNumStates+2; //accounting for the dummy states in HTK

					string trainListFilePath=trainListFilesDir+string(SEPARATOR)+"trainlistclass"+eachLineTokens[0];
					ofstream trainListFileOutput(trainListFilePath.c_str());
					for(int i=0;i<numSamples;++i)
					{
					 char buffer[100];
					 sprintf(buffer,"%03d",i);
					  trainListFileOutput<<htkTrainedDataDir<<SEPARATOR<<eachLineTokens[0]<<SEPARATOR<<"train_class"<<eachLineTokens[0]<<"_"<<buffer<<endl;
					}
					trainListFileOutput.close();

					cout<<"Initializing HMM for class-"<<eachLineTokens[0]<<endl;
					bool isInitialized=true;
					while(true)
					{
						prototypeHMMFilePath = tempDir+string(SEPARATOR)+"hmmprototype.txt";

						createHMMPrototype(prototypeHMMFilePath,estimatedNumStates,numGaussiansPerState,vecSize,0.5,"hmmprototype");

						cmdLine=htkInstallationDir+string(SEPARATOR)+"HInit -T 1 -v 0.0001 -C "+htkInstallationDir+string(SEPARATOR)+"config -A -i 100 -o hmm_char_"+eachLineTokens[0]+" -M "+tempDir+string(SEPARATOR)+"hmms"+string(SEPARATOR)+"initialized "+prototypeHMMFilePath+" -S "+trainListFilePath;
						system(cmdLine.c_str());
						string outputInitHmmFile=tempDir+string(SEPARATOR)+"hmms"+string(SEPARATOR)+"initialized"+string(SEPARATOR)+"hmm_char_"+eachLineTokens[0];
						ifstream doesFileExist(outputInitHmmFile.c_str());
						if(!doesFileExist)
						{
							 estimatedNumStates=estimatedNumStates-1; //reducing the number of states to attempt training next time
							 if(estimatedNumStates==2)
							 {
								 string uninitializedSymFilePath = tempDir+string(SEPARATOR)+"uninitializedsymbols.txt";
								 ofstream notTrainedListFile(uninitializedSymFilePath.c_str(),ios::app);
								 notTrainedListFile<<eachLineTokens[0]<<endl;
								 notTrainedListFile.close();
								 isInitialized=false;
								 break;
							 }
							 else
							 {
								 continue;
							 }
						}
						else
						{
							 //if the hmm is successfully initialized, then logging the number of states used to model the class
							 string numStatesFilePath = tempDir+string(SEPARATOR)+"numstatesperclass.txt";
							 ofstream numStatesOutput(numStatesFilePath.c_str(),ios::app);
							 numStatesOutput<<eachLineTokens[0]<<" "<<avgNumPoints<<" "<<estimatedNumStates<<endl;
							 numStatesOutput.close();
							 doesFileExist.close();
							 break;
						 }
					}
					if(isInitialized)
					{
						cout<<"Training HMM for class-"<<eachLineTokens[0]<<endl;
						cmdLine=htkInstallationDir+string(SEPARATOR)+"HRest -A -T 1 -v 0.0001 -C "+htkInstallationDir+string(SEPARATOR)+"config -i 100 -S "+trainListFilePath+" -M "+tempDir+string(SEPARATOR)+"hmms"+string(SEPARATOR)+"trained "+tempDir+string(SEPARATOR)+"hmms"+string(SEPARATOR)+"initialized"+string(SEPARATOR)+"hmm_char_"+eachLineTokens[0];
						system(cmdLine.c_str());
					}
			   }
		  }
		}
	statisticsInput.close();


	string mdtFilePath = strProfileDir + string(SEPARATOR) + string(HMM) + string(DATFILEEXT);

	#ifdef WIN32
		cmdLine = "copy "+tempDir+string(SEPARATOR)+"hmms"+string(SEPARATOR)+"trained"+string(SEPARATOR)+"*.* "+mdtFilePath+" /B";
	#else
		cmdLine = "cat "+tempDir+string(SEPARATOR)+"hmms"+string(SEPARATOR)+"trained"+string(SEPARATOR)+"* > "+mdtFilePath;
	#endif

	system(cmdLine.c_str());


	string dictionaryFilePath = strProfileDir + string(SEPARATOR) + DICTIONARYFILE;

	string modelsListFilePath = strProfileDir + string(SEPARATOR) + MODELSLISTFILE;

	string networkFilePath = strProfileDir + string(SEPARATOR) + NETWORKFILE;

	string tempGrammarFilePath = strProfileDir + string(SEPARATOR) + "tempgrammar.txt";

	ofstream dictionaryFileOutput(dictionaryFilePath.c_str());

	if(!dictionaryFileOutput)
	{
		cout<<"Unable to create dictionary file at:"<<dictionaryFilePath<<endl;
		return 1;
	}

	ofstream modelsListFileOutput(modelsListFilePath.c_str());

	ofstream tempGrammarFileOutput(tempGrammarFilePath.c_str());

	for(int c=0;c<classIdsVec.size();++c)
	{

		dictionaryFileOutput<<classIdsVec[c]<<" "<<"hmm_char_"<<classIdsVec[c]<<endl;
		modelsListFileOutput<<"hmm_char_"<<classIdsVec[c]<<endl;

		if(c==0)
		{
			tempGrammarFileOutput<<"( "<<endl;
			tempGrammarFileOutput<<classIdsVec[c]<<" | "<<endl;
		}
		else if(c==(classIdsVec.size()-1))
		{
			tempGrammarFileOutput<<classIdsVec[c]<<" "<<endl;
			tempGrammarFileOutput<<")"<<endl;
		}
		else
		{
			tempGrammarFileOutput<<classIdsVec[c]<<" | "<<endl;
		}


	}

	tempGrammarFileOutput.close();

	modelsListFileOutput.close();

	dictionaryFileOutput.close();


	cmdLine = htkInstallationDir+string(SEPARATOR)+"HParse "+tempGrammarFilePath+" "+networkFilePath;

	system(cmdLine.c_str());


	return 0;

}
