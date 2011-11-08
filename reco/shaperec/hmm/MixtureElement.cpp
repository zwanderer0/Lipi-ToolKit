#include "MixtureElement.h"
#include<iostream>

using namespace std;

//************************************
// Method:    MixtureElement
// FullName:  MixtureElement::MixtureElement
// Access:    public 
// Returns:   
// Qualifier:
//************************************
MixtureElement::MixtureElement(): m_meanArray(NULL) , m_varArray(NULL)
{
}

//************************************
// Method:    MixtureElement
// FullName:  MixtureElement::MixtureElement
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: int nMeans
// Parameter: int nVars
//************************************
MixtureElement::MixtureElement(int nMeans,int nVars)
{
	m_meanArray = new float[nMeans];
	m_varArray =  new float[nVars];
	m_numMeans = nMeans;
}

//************************************
// Method:    MixtureElement
// FullName:  MixtureElement::MixtureElement
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const MixtureElement & mixElem
//************************************
MixtureElement::MixtureElement(const MixtureElement& mixElem)
{
	m_numMeans = mixElem.m_numMeans;
	m_gConst = mixElem.m_gConst;
	m_mixWeight = mixElem.m_mixWeight;
	m_meanArray = new float[m_numMeans];
	m_varArray = new float[m_numMeans];
	for(int i=0;i<m_numMeans;i++)
	{
		m_meanArray[i] = mixElem.m_meanArray[i];
		m_varArray[i] = mixElem.m_varArray[i];
	}
}

//************************************
// Method:    MixtureElement
// FullName:  MixtureElement::MixtureElement
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const MixtureElement * mixElem
//************************************
MixtureElement::MixtureElement(const MixtureElement* mixElem)
{
	m_numMeans = mixElem->m_numMeans;
	m_gConst = mixElem->m_gConst;
	m_mixWeight = mixElem->m_mixWeight;
	m_meanArray = new float[m_numMeans];
	m_varArray = new float[m_numMeans];
	for(int i=0;i<m_numMeans;i++)
	{
		m_meanArray[i] = mixElem->m_meanArray[i];
		m_varArray[i] = mixElem->m_varArray[i];
	}
}



//************************************
// Method:    ~MixtureElement
// FullName:  MixtureElement::~MixtureElement
// Access:    public 
// Returns:   
// Qualifier:
//************************************
MixtureElement::~MixtureElement()
{
	if(m_meanArray!=NULL)
		delete [] m_meanArray;
	if(m_varArray!=NULL)
		delete [] m_varArray;
	m_meanArray = NULL;
	m_varArray = NULL;

	
}
