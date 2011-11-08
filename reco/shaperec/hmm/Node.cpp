#include "Node.h"
#include<iostream>
#include"ConstantMacros.h"
using namespace std;


//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
//************************************
Node::Node()
{
	m_symbolName = "";
	m_numPre = 0;
	m_numSucc = 0;
	m_nextFreePre = 0;
	m_nextFreeSucc = 0;
	m_nodeType = -1;
	m_model = NULL;
	m_active = false;
	m_stateToken = NULL;
	m_timeStamp = 0;
	m_listIndex = -1;
	m_wL = NULL;
	m_preArray = NULL;
	m_succArray = NULL;
	m_bestScore = LogZero;
	m_exitToken = new token[1];
}

//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const string & symbolName
//************************************
Node::Node(const string& symbolName)
{
	m_symbolName = symbolName;
	m_numPre = 0;
	m_numSucc = 0;
	m_nextFreePre = 0;
	m_nextFreeSucc = 0;
	m_nodeType = -1;
	m_model = NULL;
	m_active = false;
	m_stateToken = new token[1];
	m_timeStamp = 0;
	m_listIndex = -1;
	m_wL = NULL;
	m_preArray = NULL;
	m_succArray = NULL;
	m_bestScore = LogZero;
	m_exitToken = new token[1];
	
}
//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const string & symbolName
// Parameter: int maxPre
// Parameter: int maxSucc
//************************************
Node::Node(const string& symbolName,int maxPre,int maxSucc)
{
	m_preArray = new Node*[maxPre];
	m_succArray = new Node*[maxSucc];
	m_numPre = 0;
	m_numSucc = 0;
	m_symbolName = symbolName;
	m_active = false;
	m_model = NULL;
	m_nextFreePre = 0;
	m_nextFreeSucc = 0;
	m_nodeType = -1;
	m_wL = NULL;
	m_timeStamp = 0;
	m_stateToken = new token[1];
	m_listIndex = -1;
	m_bestScore = LogZero;
	m_exitToken = new token[1];
}
//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const string & symbolName
// Parameter: int nodeType
// Parameter: HMModel * hModel
// Parameter: int maxPre
// Parameter: int maxSucc
//************************************
Node::Node(const string& symbolName,int nodeType,HMModel* hModel,int maxPre,int maxSucc)
{
	m_preArray = new Node*[maxPre];
	m_succArray = new Node*[maxSucc];
	m_numPre = 0;
	m_numSucc = 0;
	m_symbolName = symbolName;
	m_active = false;
	m_nodeType = nodeType;
	m_model = hModel;
	m_nextFreePre = 0;
	m_nextFreeSucc = 0;
	m_stateToken = new token[m_model->m_numStates];
	m_timeStamp = 0;
	m_listIndex = -1;
	m_wL = NULL;
	m_bestScore = LogZero;
	m_exitToken = new token[1];
}
//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const string & symbolName
// Parameter: HMModel * hModel
// Parameter: int nodeType
//************************************
Node::Node(const string& symbolName,HMModel* hModel,int nodeType)
{
	m_numPre = 0;
	m_numSucc = 0;
	m_preArray = NULL;
	m_succArray = NULL;
	m_symbolName = symbolName;
	m_active = false;
	m_nodeType = nodeType;
	m_model = hModel;
	m_stateToken = new token[m_model->m_numStates];
	m_timeStamp = 0;
	m_wL = NULL;
	m_nextFreePre = 0;
	m_nextFreeSucc = 0;
	m_listIndex = -1;
	m_bestScore = LogZero;
	m_exitToken = new token[1];

}
//************************************
// Method:    ~Node
// FullName:  Node::~Node
// Access:    public 
// Returns:   
// Qualifier:
//************************************
Node::~Node()
{
	if(m_stateToken!=NULL)
		delete [] m_stateToken;
	if(m_preArray!=NULL)
		delete [] m_preArray;
	if(m_succArray!=NULL)
		delete [] m_succArray;
	delete [] m_exitToken;
	m_stateToken = NULL;
	m_preArray = NULL;
	m_succArray = NULL;
}

//************************************
// Method:    addPreArray
// FullName:  Node::addPreArray
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: Node * preNode
//************************************
void Node::addPreArray(Node* preNode)
{
	m_preArray[m_numPre] = preNode;
	++m_numPre;
}
void Node::addSuccArray(Node* succNode)
{
	m_succArray[m_numSucc] = succNode;
	++m_numSucc;
}



//************************************
// Method:    addPreArrayNet
// FullName:  Node::addPreArrayNet
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: Node * preNode
//************************************
void Node::addPreArrayNet(Node* preNode)
{
	if(m_nextFreePre==m_numPre)
	{
		int i;
		Node** tempArray;
		tempArray = new Node*[m_numPre+1];
		for(i=0;i<m_numPre;i++)
			tempArray[i] = m_preArray[i];
		delete[] m_preArray;
		m_preArray = tempArray;
		m_preArray[m_nextFreePre] = preNode;
		m_numPre++;
		m_nextFreePre++;
	}
	else
	{
		m_preArray[m_nextFreePre] = preNode;
		m_nextFreePre++;
		
	}
}

//************************************
// Method:    addSuccArrayNet
// FullName:  Node::addSuccArrayNet
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: Node * succNode
//************************************
void Node::addSuccArrayNet(Node* succNode)
{
	if(m_nextFreeSucc==m_numSucc)
	{
		int i;
		Node** tempArray;
		tempArray = new Node*[m_numSucc+1];
		for(i=0;i<m_numSucc;i++)
			tempArray[i] = m_succArray[i];
		delete[] m_succArray;
		m_succArray = tempArray;
		m_succArray[m_nextFreeSucc] = succNode;
		m_numSucc++;
		m_nextFreeSucc++;
	}
	else
	{
		m_succArray[m_nextFreeSucc] = succNode;
		m_nextFreeSucc++;

	}


}
//************************************
// Method:    displayNode
// FullName:  Node::displayNode
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void Node::displayNode()
{
	//cout<<"The name of the Node is "<<m_symbolName<<endl;
	//cout<<"Model Name  of the Node is "<<m_model->m_modelName<<endl;
	//getchar();
}


//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const Node * node
//************************************
Node::Node(const Node* node)
{
	m_nextFreePre = node->m_nextFreePre;
	m_nextFreeSucc = node->m_nextFreeSucc;
	m_numPre = node->m_numPre;
	m_numSucc = node->m_numSucc;
	m_preArray = new Node*[m_numPre];
	m_succArray = new Node*[m_numSucc];
	for(int i=0;i<m_nextFreePre;i++)
		m_preArray[i] = node->m_preArray[i];
	for(int j=0;j<m_nextFreeSucc;j++)
		m_succArray[j] = node->m_succArray[j];
	if(node->m_symbolName=="!NULL")
	{
		m_stateToken = new token();
		m_stateToken[0] = node->m_stateToken[0];
	}
	else
	{
		
		m_stateToken = new token[node->m_model->m_numStates];
		for(int k=0;k<node->m_model->m_numStates;k++)
			m_stateToken[k] = node->m_stateToken[k];
	}

	m_symbolName = node->m_symbolName;
	m_exitToken = node->m_exitToken;
	m_timeStamp = node->m_timeStamp;
}
//************************************
// Method:    Node
// FullName:  Node::Node
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: const Node & node
//************************************
Node::Node(const Node& node)
{
	
	m_nextFreePre = node.m_nextFreePre;
	m_nextFreeSucc = node.m_nextFreeSucc;
	m_numPre = node.m_numPre;
	m_numSucc = node.m_numSucc;
	m_preArray = new Node*[m_numPre];
	m_succArray = new Node*[m_numSucc];
	for(int i=0;i<m_nextFreePre;i++)
		m_preArray[i] = new Node(node.m_preArray[i]);
	for(int j=0;j<m_nextFreeSucc;j++)
		m_succArray[j] = new Node(node.m_succArray[j]);
	m_symbolName = node.m_symbolName;
	m_exitToken = node.m_exitToken;
	m_timeStamp = node.m_timeStamp;
}