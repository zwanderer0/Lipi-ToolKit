#ifndef MIXTUREELEMENT_H_
#define MIXTUREELEMENT_H_

//! MixtureElement Class
/*!
 * MixtureElement Class responsible for represnting single gaussian Mixture
 */
class MixtureElement
{
public:

	//!m_meanArray float array for keeping the vector of means	
	float *m_meanArray;

	//!m_varArray float array for keeping the vector of variances	
	float *m_varArray;
	
	//!m_gConst gconst for the particular mixture
	float m_gConst;
	
	//!m_mixWeight weight for the particular mixture
	float m_mixWeight;

	//!m_numMeans no. of means in the mixture
	int m_numMeans;
	
	
public:

	//!Default Constructor
	MixtureElement();
	
	//! OverLoaded Constructor
	/*!
	 * \param nMeans no. of means in the mixture
	 * \param nVars no. of variances in the mixture
	 */
	MixtureElement(int nMeans,int nVars);
	
	//!Copy Constructor
	MixtureElement(const MixtureElement& mixElem);
	
	//!Copy Constructor
	MixtureElement(const MixtureElement* mixElem);
	
	//!Destructor
	~MixtureElement();
};

#endif /*MIXTUREELEMENT_H_*/
