#ifndef HMModel_H_
#define HMModel_H_

#include"State.h"

#include<string>
#include<vector>
using namespace std;



//! HMModel Class
/*! 
 * HMModel Class responsible for representing Hidden Markov Models
 */
class HMModel
{
public:

	//!m_modelName name of the HMM model
	string m_modelName;
	
	//!m_numStates no. of states in the HMM model
	int m_numStates;

	//!m_stateArray array of StateHmm Objects
	State** m_stateArray;
	
	//!m_transProb transition matrix for Hidden Markov Model
	double ** m_transProb;

	//!m_PreIndices array of preIndices for each state
	int **m_PreIndices;

	

private:
	
	//!Function for displaying featureVector
	/*!
	 *\param featureArray float array conatining featureVector
	 *\param vecSize length of featureVector
	 */
	void displayFloatVector(float* floatArray,int vecSize);


public:
	//!Default Constructor
	HMModel();
	
	//!OverLoaded Constructor
	/*!
	 *\param modelName name of the HMM model
	 */	
	HMModel(const string& modelName);

	//!Copy Constructor
	HMModel(const HMModel* hModel);

	//!Copy Constructor
	HMModel(const HMModel& hModel);
	
	//!Function for displaying HMM in detail
	void displayHmmComponents();

	//!Function for return HMM model name
	string getModelName(){return m_modelName;}

	//!Function for combining HMMs in serial mode
	/*!
	 *\param hModels array of HMModels
	 *\param no. of models
	 */
	void combineModelsSerial(HMModel** hModels,int numModels);

	//!Function for combining HMMs in parallel mode
	/*!
	 *\param hModels array of HMModels
	 *\param no. of models
	 */
	void combineModelsParallel(HMModel** hModels,int numModels);

	//!Destructor
	~HMModel();
};

#endif /*HMModel_H_*/
