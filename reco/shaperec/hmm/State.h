#ifndef STATE_H_
#define STATE_H_



#include "MixtureElement.h"

//! State Class
/*!
 * State Class responsible for representing a particular state in an HMM
 */
class State
{
public:
	//!m_currProb probability for a given feature Vector
	float m_currProb;
	
	//!m_numMixtures no. of gaussian mixtures in the state
	int m_numMixtures;
	
	//!m_mixtureArray array of mixtures present in the state
	MixtureElement** m_mixtureArray;

	//!m_numPreStates no. of predecessor states for the state
	int m_numPreStates;

	//!m_timeFeature keeping track of frame
	int m_timeStamp;
	

	
public:
	
	//! Default Constructor
	State();

	//!Copy Constructor
	State(const State *stateHmm);
	
	//!Copy Constructor
	State(const State &stateHmm);
	
	//!Function for calculating Observation Probability
	/*!
	 *\param featureArray single featureVector
	 */
	float calcObsProb(float* featureArray,int timeStamp);

	//!Destructor
	~State();
};

#endif /*STATE_H_*/
