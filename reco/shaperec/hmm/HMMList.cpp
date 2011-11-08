#include "HMMList.h"
#include"State.h"
#include"HMModel.h"
#include"MixtureElement.h"
#include"ConstantMacros.h"
#include<fstream>
#include<string>
#include<iostream>
#include<cmath>




using namespace std;



//************************************
// Method:    HMMList
// FullName:  HMMList::HMMList
// Access:    public 
// Returns:   
// Qualifier:
//************************************
HMMList::HMMList(): m_modelList(NULL)
{
}

//************************************
// Method:    HMMList
// FullName:  HMMList::HMMList
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const HMMList * hList
//************************************
HMMList::HMMList(const HMMList* hList)
{
	m_currentModelIndex = hList->m_currentModelIndex;
	m_numModels = hList->m_numModels;
	m_modelList = new HMModel*[m_numModels];
	for(int i=0;i<m_numModels;i++)
	{
		m_modelList[i] = new HMModel(hList->m_modelList[i]);
		m_modelMap[m_modelList[i]->m_modelName] = m_modelList[i];
	}
	//m_modelMap = hList->m_modelMap;
}
//************************************
// Method:    HMMList
// FullName:  HMMList::HMMList
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const HMMList & hList
//************************************
HMMList::HMMList(const HMMList& hList)
{
	m_currentModelIndex = hList.m_currentModelIndex;
	m_numModels = hList.m_numModels;
	m_modelList = new HMModel*[m_numModels];
	for(int i=0;i<m_numModels;i++)
	{
		m_modelList[i] = new HMModel(hList.m_modelList[i]);
		cout<<"Copying "<<m_modelList[i]->m_modelName<<endl;
		m_modelMap[m_modelList[i]->m_modelName] = m_modelList[i];
	}
}
//************************************
// Method:    getNumModels
// FullName:  HMMList::getNumModels
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: const string & ModelsListFile
//************************************
int HMMList::getNumModels(const string& ModelsListFile )
{
		int nModels = 0;
		string temp;
		ifstream readFile(ModelsListFile.c_str());
		if(readFile==NULL)
		{

			cout<<"Error in opening ModelsList file "<<ModelsListFile<<endl;
		//	exit(0);
		}
		while(readFile)
		{
			string eachLine;
			getline(readFile,eachLine,'\n');
			if(!eachLine.empty())
			{
				m_modelMap[eachLine] = NULL;
				nModels++;
			}
		}
		readFile.close();
	    return nModels;
}

//************************************
// Method:    ~HMMList
// FullName:  HMMList::~HMMList
// Access:    public 
// Returns:   
// Qualifier:
//************************************
HMMList::~HMMList()
{
	if(m_modelList!=NULL)
	{
		for(int i=0;i<m_numModels;i++)
		{
			
			delete m_modelList[i];
		}
		delete [] m_modelList;

		m_modelList = NULL;
	}

}

//************************************
// Method:    preComputeObsProb
// FullName:  HMMList::preComputeObsProb
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * featureArray
//************************************
void HMMList::preComputeObsProb(float* featureArray)
{
	int timeStamp=0;
	//cout<<"No. of models is "<<m_numModels<<endl;
	for(int i=0;i<m_numModels;i++)
	{
		for(int j=0;j<m_modelList[i]->m_numStates;j++)
		{
			m_modelList[i]->m_stateArray[j]->calcObsProb(featureArray,timeStamp);
		}
	}
}



//************************************
// Method:    readHmmDefinition
// FullName:  HMMList::readHmmDefinition
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & HmmDefinitionFile
// Parameter: const string & ModelsListFile
//************************************
void HMMList::readHmmDefinition( const string& HmmDefinitionFile,const string& ModelsListFile )
{
	int modelCount = 0,vecSize,curState = 0,curMixture = 0;
	modelCount = getNumModels(ModelsListFile);
	
	m_numModels = modelCount;

	m_currentModelIndex = m_numModels-1;
	int iteratedCount = 0;
	

	 
	m_modelList = new HMModel*[modelCount+MaxHmmModels];
	for(int i=modelCount-1;i>=0;--i)
		m_modelList[i] = new HMModel();
	
	
	ifstream readStream(HmmDefinitionFile.c_str());
	string tempInput,garbage,modelName;
	int nMeans=0,nVariances=0,numStates,sIndex,numMixes=0;
	float mixWt,mean,var;
	MixtureElement* mixElem;
	
	if(readStream==NULL)
	{
		cout<<"Error in Opening HMModel Definition File\n";
		//exit(0);
	}
	
	while(readStream!=NULL)
	{
		
		readStream>>tempInput;
		if(tempInput=="~o")
		{
				continue;
		}
		if(tempInput=="<STREAMINFO>")
		{
				int n1,n2;
				readStream>>n1;
				readStream>>n2;
				continue;
		}
		if(tempInput=="<VECSIZE>")
		{
				readStream>>nMeans;
				vecSize = nMeans;
				m_vecSize = nMeans;
				nVariances=nMeans;
				readStream>>garbage;
				continue;
		}
		if(tempInput=="~h")
		{
				readStream>>modelName;
			//	cout<<modelName<<endl;
			    modelName = modelName.substr(1,modelName.length()-2);
			    m_modelList[iteratedCount]->m_modelName = modelName;
				continue;
		}
		if(tempInput=="<BEGINHMM>")
		{
				continue;
		}
		if(tempInput=="<NUMSTATES>")
		{
			readStream>>numStates;
			m_modelList[iteratedCount]->m_numStates = numStates;
			m_modelList[iteratedCount]->m_transProb = new double*[numStates];
			m_modelList[iteratedCount]->m_stateArray = new State*[numStates];
			for(int i=0;i<numStates;i++)
			{
				m_modelList[iteratedCount]->m_transProb[i] = new double[numStates];
				m_modelList[iteratedCount]->m_stateArray[i] = new State();
			}
			curState = 0;
			continue;
		}
		if(tempInput=="<STATE>")
		{
			readStream>>sIndex;
			sIndex--;
			curState = sIndex;
			curMixture = 0;
			continue;
		}
		if(tempInput=="<NUMMIXES>")
		{
			readStream>>numMixes;
			//m_modelList[iteratedCount]->m_stateArray[curState]->m_stateIndex = curState;
			m_modelList[iteratedCount]->m_stateArray[curState]->m_numMixtures = numMixes;
			m_modelList[iteratedCount]->m_stateArray[curState]->m_mixtureArray = new MixtureElement*[numMixes];
		/*	for(int i=0;i<numMixes;i++)
				m_modelList[iteratedCount]->m_stateArray[curState]->m_mixtureArray[i] = new MixtureElement();
		*/	
			for(int i=0;i<numMixes;i++)
				m_modelList[iteratedCount]->m_stateArray[curState]->m_mixtureArray[i] = NULL;

			continue;
		}
		if(tempInput=="<MIXTURE>")
		{
			mixElem = new MixtureElement();
			readStream>>garbage;
			readStream>>mixWt;
			mixElem->m_mixWeight = mixWt;
			continue;
		}
		if(tempInput=="<MEAN>")
		{
			readStream>>nMeans;
			mixElem->m_numMeans = nMeans;
			mixElem->m_meanArray = new float[nMeans];
			mixElem->m_varArray = new float[nMeans];

			for(int i=0;i<nMeans;i++)
			{
				readStream>>mean;
				mixElem->m_meanArray[i] = mean;
			}
			continue;
		}
		if(tempInput=="<VARIANCE>")
		{
			readStream>>nVariances;
			for(int i=0;i<nVariances;i++)
			{
				readStream>>var;
				mixElem->m_varArray[i] = var;
			}
			continue;
		}
		if(tempInput=="<GCONST>")
		{
				float gConst;
				readStream>>gConst;
				mixElem->m_gConst = gConst;
				m_modelList[iteratedCount]->m_stateArray[curState]->m_mixtureArray[curMixture] = new MixtureElement(mixElem);
			//	mixElem.~MixtureElement();
				delete mixElem;
				curMixture++;
				continue;
		}
		if(tempInput=="<TRANSP>")
		{
			   readStream>>numStates;
			   for(int i=0;i<numStates;i++)
			   {
				 for(int j=0;j<numStates;j++)
				 {
					 float temp;
					 readStream>>temp;
					 if(temp==0.0)
						 m_modelList[iteratedCount]->m_transProb[i][j] = LogZero;
					 else m_modelList[iteratedCount]->m_transProb[i][j] = log(temp);
				 }
				}
			   m_modelList[iteratedCount]->m_PreIndices = new int*[numStates];
			   
			   
			   for(int k=1;k<numStates;k++)
			   {
				   vector<int> preIndices;
				   for(int l=0;l<numStates;l++)
				   {
					   
					   if(m_modelList[iteratedCount]->m_transProb[l][k]>LogThreshold)
						  preIndices.push_back(l);
				   }
				   m_modelList[iteratedCount]->m_PreIndices[k] = new int[preIndices.size()];
				   for(int m=0;m<preIndices.size();m++)
					   m_modelList[iteratedCount]->m_PreIndices[k][m] = preIndices[m];
				   m_modelList[iteratedCount]->m_stateArray[k]->m_numPreStates = preIndices.size();
			   }
			continue;
		}
		if(tempInput=="<ENDHMM>")
		{
				sIndex++;
				string name = m_modelList[iteratedCount]->m_modelName;

				if(m_modelMap[name]==NULL)
					m_modelMap[name] = m_modelList[iteratedCount];
				else
				{

					cout<<"Multiple definitions found for "<<name<<endl;
					exit(0);
				}

			
				iteratedCount++;

				if(iteratedCount == m_numModels)
				{
					break;
				}
		 }

			
		}
		readStream.close();
		//cout<<"Reading of Hmms is finished\n";
}


void HMMList::displayHMMList()
{
	for(int i=0;i<m_numModels;i++)
	{
		//cout<<"Index of model is "<<i<<endl;
		m_modelList[i]->displayHmmComponents();
	}
}





//************************************
// Method:    addModelToList
// FullName:  HMMList::addModelToList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HMModel * hModel
//************************************
void HMMList::addModelToList(HMModel* hModel)
{
	//cout<<"Current Model Index is "<<m_currentModelIndex<<endl;
//	cout<<"Model added is "<<hModel->m_modelName<<endl;

	m_modelList[m_currentModelIndex] = hModel;
	int modelIndex = m_currentModelIndex;

	m_modelMap[hModel->m_modelName] = hModel;
	
	//cout<<"Model added is "<<hModel->m_modelName<<endl;
	//cout<<"Position is "<<modelIndex<<endl;
	m_currentModelIndex++;
//	m_numModels = m_currentModelIndex;
}



void HMMList::resetCurrProb()
{
	for(int i=0;i<m_numModels;i++)
	{
		HMModel *hmm = m_modelList[i];
		int numStates = hmm->m_numStates;
		for(int j=0;j<numStates;j++)
		{
			if(hmm->m_stateArray[j]->m_currProb>LogZero)
				hmm->m_stateArray[j]->m_currProb = LogZero;
			else continue;
		}
	}
}
void HMMList::resetTimeStamp()
{
	for(int i=0;i<m_numModels;i++)
	{
		HMModel *hmm = m_modelList[i];
		int numStates = hmm->m_numStates;
		for(int j=0;j<numStates;j++)
		{
			hmm->m_stateArray[j]->m_timeStamp = -1;
		}
	}

}