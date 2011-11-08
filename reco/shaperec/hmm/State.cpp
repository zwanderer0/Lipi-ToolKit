#include "State.h"
#include"MixtureElement.h"
#include"NumericUtility.h"




//************************************
// Method:    State
// FullName:  State::State
// Access:    public 
// Returns:   
// Qualifier:
//************************************
State::State()
{
	m_currProb = 0.0;
	m_mixtureArray = NULL;
	m_numMixtures = 0;
	m_numPreStates = 0;
	m_timeStamp = -1;
}


//************************************
// Method:    calcObsProb
// FullName:  State::calcObsProb
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * featureArray
//************************************
float State::calcObsProb(float* featureArray,int timeStamp)
{
	if(m_timeStamp==timeStamp)
		return m_currProb;
	else
	{
		float sum,prob,temp;
		prob = LogZero;
		MixtureElement *mixElem;
		for(int i=0;i<m_numMixtures;i++)
		{
			sum = 0.0;
			mixElem = m_mixtureArray[i];

			if(mixElem != NULL)
			{
				sum = mixElem->m_gConst;
				for(int j=0;j<mixElem->m_numMeans;j++)
				{
					
					temp = featureArray[j] - mixElem->m_meanArray[j];
					sum = sum+(temp*temp)/(mixElem->m_varArray[j]);
					
				}
				sum = sum*(-0.5);
				prob = addLogValues(prob,log(mixElem->m_mixWeight)+sum);

			}
		}
		m_currProb = prob;
		m_timeStamp = timeStamp;
		return m_currProb;
	}
}

//************************************
// Method:    State
// FullName:  State::State
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const State * stateHmm
//************************************
State::State(const State *stateHmm)
{
	m_numMixtures = stateHmm->m_numMixtures;
	m_mixtureArray = new MixtureElement*[m_numMixtures];
	for(int i=0;i<m_numMixtures;i++)
		m_mixtureArray[i] = new MixtureElement((stateHmm->m_mixtureArray[i]));
}


//************************************
// Method:    State
// FullName:  State::State
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const State & stateHmm
//************************************
State::State(const State &stateHmm)
{
	m_numMixtures = stateHmm.m_numMixtures;
	m_mixtureArray = new MixtureElement*[m_numMixtures];
	for(int i=0;i<m_numMixtures;i++)
		m_mixtureArray[i] = new MixtureElement((stateHmm.m_mixtureArray[i]));
}


//************************************
// Method:    ~State
// FullName:  State::~State
// Access:    public 
// Returns:   
// Qualifier:
//************************************
State::~State()
{
	m_currProb = 0.0;
	if(m_mixtureArray!=NULL)
	{
		for(int i=0;i<m_numMixtures;i++)
		{
			if(m_mixtureArray[i] != NULL)
			{
				delete m_mixtureArray[i];	
			}
		}
		delete [] m_mixtureArray;
		m_mixtureArray = NULL;
	}	
}
