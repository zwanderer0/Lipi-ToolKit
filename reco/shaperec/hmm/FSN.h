#ifndef FSN_H_
#define FSN_H_

#include"Node.h"
#include "HMMList.h"
#include<map>
#include<iostream>
#include<string>
#include<vector>

using namespace std;

//! FSN Class
/*! FSN Class responsible for creating the Finite State Network of Nodes
 */
class FSN
{
private:
	
	//!m_startDummyNode source node of FSN
	Node *m_startDummyNode;
	
	//!m_endDummyNode sink node of FSN
	Node *m_endDummyNode;

	//!m_nodeMap containing mapping between Node name and its HMModel pointer in m_modelList array
	map<string,HMModel*> m_nodeMap;
	
	//!m_hlist pointer to an instance of HMMList Class
	HMMList* m_hList;

	//!m_listNodes array of pointers to activated nodes in the FSN
	Node** m_listNodes;

	//!m_numNodes no. of nodes in the FSN
	int m_numNodes;
	
	//!m_numEdges no. of edges in the FSN
	int m_numEdges;

	//!m_wordListInitializtion specifies whether network has been created from wordList or not
	bool m_wordListInitializtion;

	//!m_wordAdded specifies whether word has been added to the FSN at run-time or not
	bool m_wordAdded;

	//!m_wordListFileName file Name from which wordList has been read
	string m_wordListFileName;

	//!m_wordsAdded list of words that are added at run-time
	vector<string> m_wordsAdded;

private:

	//!Function for creating the network when initialization is from network file
	/*!
	 *\param networkFile name of the networkFile
	 */
	 void processNetwork(const string&  networkFile);

	//!Function for creating nodes of the network when initialization is from network file
	/*!
	 *\param networkFile name of the network File
	 */
	  void processNodes(const string& networkFile);

	//!Function for creating edges between nodes of the network when initialization is from network file
	/*!
	 *\param networkFile name of the network File
	 */
	  void processEdges(const string& networkFile);

	//!Function for reading dictionary file and creating mapping between node name and HMModel pointer
	/*!
	 *\param dictionaryFile name of the dictionary File
	 */
	 void readDictionaryFile(const string& dictionaryFile);
	
	//!Function for adding word to a network
	/*!
	 *\param word word to be added
	 *\param node start node of the network
	 *\param delimiter delimiter of the word
	 */
	 void addToNetwork(string word,Node *node,string delimiter);

	//!Function for tokenizing a string based on its delimiters
	/*!
	 *\param str string to be tokenized
	 *\param tokens vector of string of tokens that are formed out of str
	 *\param delimiters delimiters based on which string has to be tokenized
	 */
     void tokenizeString(const string& str,vector<string> &tokens,const string& delimiters);

	//!Function for checking whether a particular symbol is present in its successors
	/*!
	 *\param parentNode node whose successors has to be checked
	 *\param symbolName symbol whose presence has to be checked
	 */
	 int matchNode(Node *parentNode,string symbolName);

	//!Function for adding a node to the list
	/*!
	 *\param node pointer to a particular node in the network
	 */
	 void addToList(Node* node);


public:

	//!Default Constructor
	FSN();
	
	//!Destructor
	~FSN();
	
	//!Function for returning start node of the FSN
	Node* getStartNode();
	
	//!Function for returning end node of the FSN
	Node* getEndNode();

	//!Function for initialization from network File
	/*!
	 *\param dictionaryFile name of the dictionaryFile
	 *\param networkFile name of the networkFile
	 *\param modelsList name of modelsList File
	 *\param trainedHMMFile name of trainedHMMFile
	 */
	void initializeFromNetworkFile(const string& dictionaryFile,const string&  networkFile,const string& modelsList,const string& trainedHMMFile);

	//!Function for returning no. of node in the FSN
	int getNumNodes(){return m_numNodes;}

	//!Function for adding word to the FSN
	/*!
	 *\param word word to be added
	 *\param delimiter delimiter of the word
	 */
	void addWord(string word,string delimiter);

	//!Function for displaying the TRIE
	/*!
	 *\param node pointer to start Node of FSN
	 *\param path string 
	 */
	void displayTRIE(Node *node,string path);

	//!Function for writing network in HTK format to file
	/*!
	 *\param outputNetworkFile name of network File
	 */
	void writeNetwork(const string& outputNetworkFile);
	
	//!Function for initialization from word File
	/*!
	 *\param dictionaryFile name of the dictionaryFile
	 *\param wordFile name of the wordFile
	 *\param modelsList name of modelsList File
	 *\param trainedHMMFile name of trainedHMMFile
	 *\param delimiter delimiter used in word representation
	 */
	void initializeFromWordFile(const string& dictionaryFile,const string&  wordFile,const string& modelsList,const string& trainedHMMFile,const string& delimiter);
	
	//!Function for reading a word File
	/*!
	 *\param wordFile name of the word File
	 *\param wordList vector in which words are to be inserted
	 */
	void readWordFile(const string& wordFile,vector<string>& wordList);

	//!Function for adding word in case of initialization from word File
	/*!
	 *\param word word to be added
	 *\param node start node of the network
	 *\delimiter delimiter used in word representation
	 */
	void addWordToTRIE(string word,Node* node,const string& delimiter);

	//!Function for returning HMMList
	HMMList* getHMMList(){return m_hList;}

	
};
#endif /*FSN_H_*/
