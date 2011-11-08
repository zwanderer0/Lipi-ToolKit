#include "HMModel.h"
#include "State.h"
#include "MixtureElement.h"
#include "ConstantMacros.h"
#include <iostream>
#include <cmath>


using namespace std;



//************************************
// Method:    HMModel
// FullName:  HMModel::HMModel
// Access:    public 
// Returns:   
// Qualifier:
//************************************
HMModel::HMModel():m_modelName(""),m_transProb(NULL),m_PreIndices(NULL),m_stateArray(NULL)
{
}


//************************************
// Method:    ~HMModel
// FullName:  HMModel::~HMModel
// Access:    public 
// Returns:   
// Qualifier:
//************************************
HMModel::~HMModel()
{
	int i;
	if(m_transProb!=NULL)
	{
		for(i=0;i<m_numStates;i++)
			delete m_transProb[i];
		delete []m_transProb;
		m_transProb = NULL;
	}
	


	if(m_PreIndices!=NULL)
	{
		for(i=1;i<m_numStates;i++)
		{
			delete m_PreIndices[i];
		}
		delete [] m_PreIndices;
		m_PreIndices = NULL;
	}

	if(m_stateArray!=NULL)
	{
		for(i=0;i<m_numStates;i++)
			delete m_stateArray[i];
		delete [] m_stateArray;
		m_stateArray = NULL;
	}
}

//************************************
// Method:    HMModel
// FullName:  HMModel::HMModel
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const string & modelName
//************************************
HMModel::HMModel(const string& modelName): m_modelName(modelName),m_transProb(NULL),m_PreIndices(NULL),m_stateArray(NULL)
{
}




//************************************
// Method:    displayFloatVector
// FullName:  HMModel::displayFloatVector
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: float * floatArray
// Parameter: int vecSize
//************************************
void HMModel::displayFloatVector(float* floatArray,int vecSize)
{
	for(int i=0;i<vecSize;i++)
		cout<<floatArray[i]<<" ";
	cout<<endl;
}

//************************************
// Method:    displayHmmComponents
// FullName:  HMModel::displayHmmComponents
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void HMModel::displayHmmComponents()
{
	cout<<"------------------------------------\n";
	cout<<"The HMModel Model is "<<m_modelName<<endl;
	cout<<"No. of states in HMModel is "<<m_numStates<<endl;
	
	for(int i=0;i<m_numStates;i++)
	{
		cout<<"The State Index is "<<i<<endl;
		if(m_stateArray[i]->m_numMixtures==0)
			continue;
		cout<<"No. of mixtures in state is "<<m_stateArray[i]->m_numMixtures<<endl;
		for(int j=0;j<m_stateArray[i]->m_numMixtures;j++)
		{
			cout<<"Mixture Weight of current mixture "<<j<<" is "<<m_stateArray[i]->m_mixtureArray[j]->m_mixWeight<<endl;
			cout<<"Mean Vector of current mixture "<<j<<" is as follows:\n";
			displayFloatVector(m_stateArray[i]->m_mixtureArray[j]->m_meanArray,m_stateArray[i]->m_mixtureArray[j]->m_numMeans);
			cout<<"Variance Vector of current mixture "<<j<<" is as follows:\n";
			displayFloatVector(m_stateArray[i]->m_mixtureArray[j]->m_varArray,m_stateArray[i]->m_mixtureArray[j]->m_numMeans);
			cout<<"Gconst of current mixture "<<j<<" is "<<m_stateArray[i]->m_mixtureArray[j]->m_gConst<<endl;
		}
	}
	cout<<"Displaying Transition Matrix of HMModel: \n";
	for(int m=0;m<m_numStates;m++)
	{
		for(int n=0;n<m_numStates;n++)
		{
			cout<<m_transProb[m][n]<<" ";
		}
		cout<<endl;
	}
	cout<<"------------------------------------\n";
	
}


//************************************
// Method:    combineModelsSerial
// FullName:  HMModel::combineModelsSerial
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HMModel * * hModels
// Parameter: int numModels
//************************************
void HMModel::combineModelsSerial(HMModel** hModels,int numModels)
{
	cout<<"In Combining Models \n";
	int i,j,k,m,n;
	m_numStates = 0;
	for(i=0;i<numModels;i++)
	{
		m_numStates+=(hModels[i]->m_numStates-2);
		m_modelName+=("AND"+(hModels[i]->m_modelName));
	}
	m_numStates+=2;

	m_transProb = new double*[m_numStates];


	for(m=0;m<m_numStates;m++)
	{
		m_transProb[m] = new double[m_numStates];
		for(n=0;n<m_numStates;n++)
			m_transProb[m][n] = LogZero;
	}
	m_stateArray = new State*[m_numStates];

	m_stateArray[0] = new State();

	int index = 1;
	for(j=0;j<numModels;j++)
	{
		int numStates = hModels[j]->m_numStates;

		for(k=1;k<numStates-1;k++)
		{
			m_stateArray[index] = new State(hModels[j]->m_stateArray[k]);
			index++;
		}
	}
	m_stateArray[m_numStates-1] = new State();



	for(i=0;i<hModels[0]->m_numStates-1;i++)
	{
		for(j=0;j<hModels[0]->m_numStates;j++)
		{
			m_transProb[i][j] = hModels[0]->m_transProb[i][j];
		}
	}

	int prevNumStates = (hModels[0]->m_numStates)-2;
	for(i=1;i<numModels;i++)
	{
		int numStates = hModels[i]->m_numStates;
		for(j=1;j<numStates-1;j++)
		{
			for(k=1;k<numStates;k++)
			{
				m_transProb[j+prevNumStates][k+prevNumStates] = hModels[i]->m_transProb[j][k];
			}
		}

		prevNumStates+=(numStates-2);
	}
	m_PreIndices = new int*[m_numStates];
	for(i=1;i<m_numStates;i++)
	{
		vector<int> preIndices;
		for(j=0;j<m_numStates;j++)
		{
			if(m_transProb[j][i]>LogThreshold)
			{
				preIndices.push_back(j);
			}
		}
		m_PreIndices[i] = new int[preIndices.size()];
		for(k=0;k<preIndices.size();k++)
			m_PreIndices[i][k] = preIndices[k];
		m_stateArray[i]->m_numPreStates = preIndices.size();
	}

}

//************************************
// Method:    combineModelsParallel
// FullName:  HMModel::combineModelsParallel
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HMModel * * hModels
// Parameter: int numModels
//************************************
void HMModel::combineModelsParallel(HMModel** hModels,int numModels)
{
	int i,j,k,m,n;
	m_numStates = 0;
	for(i=0;i<numModels;i++)
	{
		m_numStates+=(hModels[i]->m_numStates-2);
		m_modelName+=("OR"+(hModels[i]->m_modelName));
	}
	m_numStates+=2;
	
	m_transProb = new double*[m_numStates];


	for(m=0;m<m_numStates;m++)
	{
		m_transProb[m] = new double[m_numStates];
		for(n=0;n<m_numStates;n++)
			m_transProb[m][n] = LogZero;
	}

	m_stateArray = new State*[m_numStates];

	m_stateArray[0] = new State(0);

	int index = 1;
	for(j=0;j<numModels;j++)
	{
		int numStates = hModels[j]->m_numStates;

		for(k=1;k<numStates-1;k++)
		{
			m_stateArray[index] = new State(hModels[j]->m_stateArray[k]);
			index++;
		}
	}
	m_stateArray[m_numStates-1] = new State();

	int prevNumStates = 0;

	for(i=0;i<numModels;i++)
	{
		int numStates = hModels[i]->m_numStates;
		for(j=1;j<numStates-1;j++)
		{
			for(k=1;k<numStates-1;k++)
			{
				m_transProb[j+prevNumStates][k+prevNumStates] = hModels[i]->m_transProb[j][k];
			}
		}
		//m_transProb[0][prevNumStates+1] = 0;
		//m_transProb[prevNumStates+numStates-2][m_numStates-1] = hModels[i]->m_transProb[numStates-2][numStates-1];

		for(k=1;k<numStates-1;k++)
		{
			m_transProb[0][k+prevNumStates] = hModels[i]->m_transProb[0][k];
			m_transProb[m_numStates-2][k+prevNumStates] = hModels[i]->m_transProb[numStates-2][k];
		}
		prevNumStates+=(numStates-2);
	}

	m_PreIndices = new int*[m_numStates];
	for(i=1;i<m_numStates;i++)
	{
		vector<int> preIndices;
		for(j=0;j<m_numStates;j++)
		{
			if(m_transProb[j][i]>LogThreshold)
			{
				preIndices.push_back(j);
			}
		}
		m_PreIndices[i] = new int[preIndices.size()];
		for(k=0;k<preIndices.size();k++)
			m_PreIndices[i][k] = preIndices[k];
		m_stateArray[i]->m_numPreStates = preIndices.size();
	}
}

//************************************
// Method:    HMModel
// FullName:  HMModel::HMModel
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const HMModel & hModel
//************************************
HMModel::HMModel(const HMModel& hModel)
{
	m_modelName = hModel.m_modelName;
	m_numStates = hModel.m_numStates;
	m_transProb = new double*[m_numStates];
	for(int i=0;i<m_numStates;i++)
		m_transProb[i] = new double[m_numStates];
	for(int j=0;j<m_numStates;j++)
		for(int k=0;k<m_numStates;k++)
			m_transProb[j][k] = hModel.m_transProb[j][k];
	m_stateArray = new State*[m_numStates];
	for(int m=0;m<m_numStates;m++)
		m_stateArray[m] = new State(hModel.m_stateArray[m]);
	m_PreIndices = hModel.m_PreIndices;

}

//************************************
// Method:    HMModel
// FullName:  HMModel::HMModel
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const HMModel * hModel
//************************************
HMModel::HMModel(const HMModel* hModel)
{
	m_modelName = hModel->m_modelName;
	m_numStates = hModel->m_numStates;
	m_transProb = new double*[m_numStates];
	for(int i=0;i<m_numStates;i++)
		m_transProb[i] = new double[m_numStates];
	for(int j=0;j<m_numStates;j++)
		for(int k=0;k<m_numStates;k++)
			m_transProb[j][k] = hModel->m_transProb[j][k];
		m_stateArray = new State*[m_numStates];
		for(int m=0;m<m_numStates;m++)
			m_stateArray[m] = new State(hModel->m_stateArray[m]);
		m_PreIndices = hModel->m_PreIndices;
		
}







