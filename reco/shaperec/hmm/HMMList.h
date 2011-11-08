#ifndef HMMLIST_H_
#define HMMLIST_H_

#include "HMModel.h"
#include<vector>
#include<map>

using namespace std;

//! HMMList Class
/*!
 * HMMList Class responsible for maintaining a list
 * of distinct HMM models and a mapping between HMM Model
 * Name and its corresponding pointer in the array of HMModel Objects
 */
class HMMList
{
public:

	//!m_modelList array of pointers to distinct Hidden Markov Models
	 HMModel** m_modelList;
	
	 //!m_modelMap mapping between HMM model and its pointer into m_modelList
	 map<string,HMModel*> m_modelMap;
	
	 
	//! m_numModels no. of distinct Hidden Markov Models
	 int m_numModels;

	 //!m_vecSize length of single feature Vector
	 int m_vecSize;
	
	//!m_currentModelIndex index where new HMModel can be inserted
	 int m_currentModelIndex;
public:

	//!Default Constructor
	HMMList();

	//!Copy Constructor
	HMMList(const HMMList& hList);

	//!Copy Constructor
	HMMList(const HMMList* hList);
	
	//!Destructor
	~HMMList();
	
	
	//!Function for reading Hidden Markov Definitions from trained HMM definition File
	/*!
	 *\param HmmDefinitionFile name of file containing trained HMM definitions
	 *\param ModelsListFile name of file containing names of distinct HMMs
	 */
	void readHmmDefinition(const string& HmmDefinitionFile,const string& ModelsListFile);

	
	//!Function for calculating Observation probability for all the distinct HMMs
	/*!
	 *\param featureArray float array containing featureVector
	 */
	void preComputeObsProb(float* featureArray);

	
	//!Function for counting no. of distinct Hidden Markov Models
	/*!
	 *\param ModelsListFile name of file containing names of distinct HMMs
	 */
	inline int getNumModels(const string&  ModelsListFile);
	
	//! Function for displaying each HMM in detail present in m_modelList array
	void displayHMMList();
	
	//!Function for returning vecSize of the a single featureVector
	int getVecSize(){return m_vecSize;}

	//!Function for adding new model to List
	/*!
	 *\param hModel pointer to HMModel
	 */
	void addModelToList(HMModel* hModel);

	//!Function for returning pointer to HMModel whose name is given as input
	/*!
	 *\param modelName name of the HMModel whose pointer has to be returned
	 */
	HMModel* getModel(const string& modelName){return m_modelMap[modelName];}

	//!Function for resetting the probabilities of all states of all distinct HMMs
	void resetCurrProb();

	//!Function for resetting the timeStamp of all states of all distinct HMMs
	void resetTimeStamp();
};

#endif /*HMMLIST_H_*/
