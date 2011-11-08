#ifndef DECODER_H_
#define DECODER_H_

#include "HMMList.h"
#include "Node.h"
#include "FSN.h"
#include<list>
#include<ctime>

using namespace std;

//! Decoder Class
/*!
	Decoder Class Responsible for decoding the user-specified feature file
	with user-specified beam width.
*/

class Decoder
{
public:
	
	//! m_listActivatedNodes list of all the nodes in the FSN that are active
	 Node** m_listActivatedNodes;
	

	 //! m_hList Instance of HMMList Class
	 HMMList* m_hList;
	
	 //!m_maxScore maximum score achieved by any active node in FSN
	 float m_maxScore;
	
	 //!m_pruningThreshold user specified beam width
	 float m_pruningThreshold;

	//! m_numActivatedNodes no. of active nodes in the FSN
	 int m_numActivatedNodes;
	
	 //! m_startDummyNode source node of FSN
	 Node* m_startDummyNode;
	
	 //!m_endDummyNode sink node of FSN
	 Node* m_endDummyNode;
	
	 //!m_vecSize length of single feature Vector
	 int m_vecSize;

	 //!m_doPruning boolean flag for specifying whether pruning should be done or not
	 bool m_doPruning;

	 //!m_wordLinkBuffer buffer for keeping wordLinks
	 list<wordLink*> m_wordLinkBuffer;

	





private:

	//!Function for activating a particular Node of the FSN
	/*!
	 *\param node a pointer to a particular Node of the FSN
	 */
	void activateNode(Node *node);
	

	//! Function for pruning active Nodes of the FSN
	void pruneActiveNodes();
	
	//!Function for activating the network
	/*!
	 * Function activates the Network by simply activating all
	 * the successor Nodes of startDummyNode
	 */
	void activateNetwork();

	//! Function for decoding a single Feature Vector with respect to a particular Node of the FSN
	/*!
	 *\param node a pointer to a particular Node of the FSN
	 */
	void decodeCurrentFeature(Node *node,float* featureArray,int timeStamp);

	
	//! Function for advancing HMMs of all the active Nodes of Network by a one timeframe.
	/*!
	 * Function advances HMMs by calling decodeCurrentFeature(Node *node,float* featureArray) function
	 * for each active node
	 */
	void advanceActivatedNodes(float* featureArray,int timeStamp);
	
	
	//! Function for transferring tokens between parent node to child node
	void passTokensToSuccessors();
	
	//!Function for creating wordLink Records whenever we complete a character
	/*!
	 *\param timeStamp integer variable denoting the time when the character is completed
	 */
	void createWordLink(int timeStamp);

	//!Function for trace backing in order to find the optimal path for word completion
	/*!
	 *\param t integer variable denoting the time
	 */
	void traceBack(int t);


	//!Function for getting N-Best Results in case of character recognition
	/*!
	 *\param N no. of choices required
	 *\param results vector in which choices has to be inserted
	 *\param scores vector in which corresponding scores has to be inserted
	 */
	void getNBest( int N,vector<string>& results,vector<float>& scores );

	
	//!Function for printing the results by following the wordLink Record
	/*!
	 *\param wL pointer to wordLink Record
	 */
	void printResults(wordLink *wL);
	
	//!Debugging function for displaying token values
	/*!
	 *\param node a pointer to a particular Node of the FSN
	 */
	void displayTokens(Node *node);

	//!Function for tokenized a string based on its delimiters
	/*!
	 *\param str string to be tokenized
	 *\param tokens vector of string of tokens that are formed out of str
	 *\param delimiters delimiters based on which string has to be tokenized
	 */
	int tokenizeString(const string& str, vector<string>& tokens, const string& delimiters);

	//!Function for deactivating the network
	void deactivateNetwork();

	//!Debugging function to display feature Array
	void displayFeatureArray(float* featureArray,int vecSize);

	//!Function for getting the final result after processing all the features
	/*!
	 *\param wL pointer to a wordLink
	 */
	string getResult(wordLink *wL);

	void clearWordLinkBuffer();

	
	
public:

	//!Default Constructor
	Decoder();

	//!Function for decoding a fetaureVector
	/*!
	 *\param featureVector vector containing all the featureVector
	 */
	void decode(const vector< vector<float> > &featureVector,string& result,double& timeElapsed);


	void decodeNBestChars(const vector< vector<float> >& featureVector,int N, vector<string>& outRecoResults,vector<float>& outRecoScores);

	//!Destructor
	~Decoder();

	//!Function for initializing the Decoder Object
	/*!
	 *\param network pointer to a FSN Object
	 *\param pruningThreshold user-specified beam width
	 *\param setPruningTrue whether pruning should be done or not
	 */
	void initialize(FSN *network,float pruningThreshold,bool setPruningTrue);

	

	//!Function for getting the recognized word
	/*
	 *\param recognizedWords vector in which recognized words has to be inserted
	 */
	void getRecognizedWord(vector<string>& recognizedWords);
	
	//!Function for decoding the list of test Feature Files
	/*
	 *\param testFileList file containing paths of testFeature Files
	 */
	void decodeBatchFiles(const string& testFileList);

	//!Function for measuring the time elapsed between a function call
	/*
	 *\param clock1 time when function has returned
	 *\param clock2 time when function has been called
	 */
	double diffclock(clock_t clock1,clock_t clock2);
	
	
	void initializeEntryTokens();

	void passTokensToSuccessorsNPenStyle();
	
	
	
	
};

#endif /*DECODER_H_*/
