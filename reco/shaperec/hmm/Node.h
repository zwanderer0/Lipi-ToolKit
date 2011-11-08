#ifndef NODE_H_
#define NODE_H_


#include<string>
#include<vector>
#include "HMModel.h"
using namespace std;


typedef struct _token token;
typedef struct _wordLink wordLink;



//! Node Class 
/*!
 * Node Class represents single Node object of the Network
 */
class Node
{
public:
	
	//!m_listIndex index in the FSN list array
	int m_listIndex;

	//!m_numPre number of predecessors of Node
	int m_numPre;
	
	//!m_numSucc number of successors of Node
	int m_numSucc;
	
	//!m_nextFreePre next free slot where predecessors can be added
	int m_nextFreePre;

	//!m_nextFreeSucc next free slot where successors can be added
	int m_nextFreeSucc;
	
	//!m_symbolName name of the node
	string m_symbolName;
	
	//!m_bestScore best Score attained by the node
	float m_bestScore;
	
	//!m_nodeType type of the node
	int m_nodeType;
	
	//!m_active signifying whether node is active or not
	bool m_active;
	
	//!m_preArray array of predecessors node
	Node** m_preArray;
	
	//!m_preArray array of successors node
	Node** m_succArray;	

	//!m_model pointer to its corresponding HMModel
	HMModel* m_model;
	
	//!m_stateToken array of tokens one for each state
	token *m_stateToken;
		
	//!m_timeStamp time stamp for the node
	int m_timeStamp;

	//!m_wL wordLink Record
	wordLink *m_wL;

	
	//!m_exitToken denotes the token value that the node has to pass to its successors
	token *m_exitToken;
	
public:
	

	//!Default Constructor
	Node();
	
	
	//! Overloaded Constructor
	/*!
	 *\param symbolName name of the Node
	 */
	Node(const string& symbolName);
	

	//!Copy Constructor
	Node(const Node* node);


	//!Copy Constructor
	Node(const Node& node);

	
	//!OverLoaded Constructor
	/*!
	 *\param symbolName name of the Node
	 *\param maxPre max no. of parents
	 *\param maxSucc max no. of successors
	 */
	Node(const string& symbolName,int maxPre,int maxSucc);

	//!OverLoaded Constructor
	/*!
	 *\param symbolName name of the Node
	 *\param hModel pointer to its corresponding HMModel
	 *\param maxPre max no. of parents
	 *\param maxSucc max no. of successors
	 */
	Node(const string& symbolName,int nodeType,HMModel* hModel,int maxPre,int maxSucc);

	//!OverLoaded Constructor
	/*!
	 *\param symbolName name of the Node
	 *\param nodeType type of the node
	 */
	Node(const string& symbolName,HMModel* hModel,int nodeType);
	
	//!Function for adding predecessor
	/*! To be used when initialization is from wordList
	 *\param preNode node to predecessor
	 */
	void addPreArray(Node* preNode);
	
	//!Function for adding successor
	/*! To be used when initialization is from wordList
	 *\param succNode node to successor
	 */
	void addSuccArray(Node* succNode);

	//!Function for adding successor
	/*! To be used when initialization is from network File
	 *\param succNode node to successor
	 */
	void addSuccArrayNet(Node* succNode);

	//!Function for adding predecessor
	/*! To be used when initialization is from network File
	 *\param preNode node to predecessor
	 */
	void addPreArrayNet(Node* preNode);
	
	//!Function for displaying Node
	void displayNode();
	
	//!Destructor
	~Node();
};


struct _token
{
	Node *node;
	_wordLink *wL;
	float score;
};
struct _wordLink
{
	token tok;
	int timeStamp;

};

#endif /*NODE_H_*/
