#include "Decoder.h"
#include"ConstantMacros.h"
#include<fstream>
#include<iostream>
#include<ctime>
#include<algorithm>
#include<list>
#include<cmath>
using namespace std;



//************************************
// Method:    diffclock
// FullName:  Decoder::diffclock
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: clock_t clock1
// Parameter: clock_t clock2
//************************************
double Decoder::diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
} 



//************************************
// Method:    Decoder
// FullName:  Decoder::Decoder
// Access:    public 
// Returns:   
// Qualifier:
//************************************
Decoder::Decoder()
{
}

//************************************
// Method:    initializeEntryTokens
// FullName:  Decoder::initializeEntryTokens
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void Decoder::initializeEntryTokens()
{
	static token nullToken = {NULL,NULL,LogZero};
	Node *node;
	for(int i=0;i<m_numActivatedNodes;i++)
	{
		node = m_listActivatedNodes[i];
		node->m_stateToken[0] = nullToken;
	}
}

//************************************
// Method:    activateNode
// FullName:  Decoder::activateNode
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: Node * node
//************************************
void Decoder::activateNode(Node *node)
{
	static token nullToken = {NULL,NULL,LogZero};
	
	if(node->m_active==false)
	{
		if(node->m_symbolName=="!NULL")
		{
			node->m_active = true;
			m_listActivatedNodes[m_numActivatedNodes++] = node;
			node->m_stateToken[0] = nullToken;
		}
		else
		{
			node->m_active = true;
			m_listActivatedNodes[m_numActivatedNodes++] = node;
			int numStates = node->m_model->m_numStates;
			for(int i=0;i<numStates;i++)
				node->m_stateToken[i] = nullToken;
		}
	}
}


//************************************
// Method:    pruneActiveNodes
// FullName:  Decoder::pruneActiveNodes
// Access:    private 
// Returns:   void
// Qualifier:
//************************************
void Decoder::pruneActiveNodes()
{
	int i,j;
	float beamLimit = m_maxScore-m_pruningThreshold;

	for(i=0;i<m_numActivatedNodes;i++)
	{
		
		if(m_listActivatedNodes[i]->m_bestScore<beamLimit)
		{
				m_listActivatedNodes[i]->m_active = false;
				m_listActivatedNodes[i]->m_timeStamp = 0;
				m_listActivatedNodes[i]->m_wL = NULL;
				m_listActivatedNodes[i]=NULL;
				
		}
	}

	for(i=0,j=0;i<m_numActivatedNodes;i++)
	{
		if(m_listActivatedNodes[i]!=NULL)
			m_listActivatedNodes[j++] = m_listActivatedNodes[i];
	}
	m_numActivatedNodes = j;

}


//************************************
// Method:    activateNetwork
// FullName:  Decoder::activateNetwork
// Access:    private 
// Returns:   void
// Qualifier:
//************************************
void Decoder::activateNetwork()
{
	Node *node = m_startDummyNode;
	int i;
	static token startToken ={NULL,NULL,0};
	for(i=0;i<node->m_numSucc;i++)
	{
		activateNode(node->m_succArray[i]);
		startToken.node = node->m_succArray[i];
		node->m_succArray[i]->m_stateToken[0] = startToken;
	}
}


//************************************
// Method:    decodeCurrentFeature
// FullName:  Decoder::decodeCurrentFeature
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: Node * node
//************************************
void Decoder::decodeCurrentFeature(Node *node,float* featureArray,int timeStamp)
{
	node->m_bestScore = node->m_stateToken[0].score;
	int i,j,bestState,counter = 0,numPreStates = 0;;
	float bestScore,x,a;
	static token nullToken = {NULL,NULL,LogZero};
	token *tokenBuffer;
	token tempToken;
	int numStates = node->m_model->m_numStates;
	
	tokenBuffer = new token[numStates];
	for(j=1;j<numStates-1;j++)
	{
		bestScore = LogZero;
		bestState = -1;

		numPreStates = node->m_model->m_stateArray[j]->m_numPreStates;
		counter = 0;
		while(counter<numPreStates)
		{
			i = node->m_model->m_PreIndices[j][counter];
			if(node->m_stateToken[i].node!=NULL)
			{
				x = node->m_stateToken[i].score+node->m_model->m_transProb[i][j];
				if(x>bestScore)
				{
						bestScore = x;
						bestState = i;
				}
			}
			counter++;
		}
		if(bestState>=0)
		{
			tokenBuffer[j] = node->m_stateToken[bestState];
			tokenBuffer[j].score = bestScore;
			if(tokenBuffer[j].score>LogThreshold)
			{
			  tokenBuffer[j].score+=node->m_model->m_stateArray[j]->calcObsProb(featureArray,timeStamp);
			}
			m_maxScore = (tokenBuffer[j].score>m_maxScore)?tokenBuffer[j].score:m_maxScore;
		}
		else tokenBuffer[j] = nullToken;
	}
	
	for(i=1;i<numStates-1;i++)
	{
		node->m_stateToken[i] = tokenBuffer[i];
		node->m_bestScore = (tokenBuffer[i].score>node->m_bestScore)?tokenBuffer[i].score:node->m_bestScore;
	}
	
	
	bestScore = LogZero,bestState = -1;
	
	numPreStates = node->m_model->m_stateArray[numStates-1]->m_numPreStates;
	counter = 0;
	while(counter<numPreStates)
	{
		i = node->m_model->m_PreIndices[numStates-1][counter];
		if(node->m_stateToken[i].node!=NULL)
		{
			a = node->m_model->m_transProb[i][numStates-1];
			x = node->m_stateToken[i].score+a;
			if(x>bestScore)
			{
					bestScore = x;
					bestState = i;
			}
		}
		counter++;
	}
	
	if(bestState>=0)
	{
		tempToken = node->m_stateToken[bestState];
		tempToken.score = bestScore;
		m_maxScore = (bestScore>m_maxScore)?bestScore:m_maxScore;
		node->m_bestScore = (bestScore>node->m_bestScore)?bestScore:node->m_bestScore;
		node->m_stateToken[numStates-1] = tempToken;
		node->m_exitToken[0] = node->m_stateToken[numStates-1];
	}
	else 
	{
		node->m_stateToken[numStates-1] = nullToken;
		node->m_exitToken[0] = nullToken;
	}
	delete[] tokenBuffer;
}


//************************************
// Method:    advanceActivatedNodes
// FullName:  Decoder::advanceActivatedNodes
// Access:    private 
// Returns:   void
// Qualifier:
//************************************
void Decoder::advanceActivatedNodes(float* featureArray,int timeStamp)
{
	m_maxScore = LogZero;
	int i;
	Node * node ;
	for(i=0;i<m_numActivatedNodes;i++)
	{
		node = m_listActivatedNodes[i];
		if(node->m_symbolName!="!NULL")
		{
			decodeCurrentFeature(m_listActivatedNodes[i],featureArray,timeStamp);
		}
		else 
		{
			node->m_bestScore = node->m_stateToken[0].score;
			node->m_exitToken[0] = node->m_stateToken[0];
		}
	}
}


//************************************
// Method:    passTokensToSuccessors
// FullName:  Decoder::passTokensToSuccessors
// Access:    private 
// Returns:   void
// Qualifier:
//************************************
void Decoder::passTokensToSuccessors()
{

	Node* succNode;
	int i,j,numPreviousActiveNodes = m_numActivatedNodes;
	float exitScore;
	token exitToken;
	for(i=0;i<numPreviousActiveNodes;i++)
	{
		exitToken = m_listActivatedNodes[i]->m_exitToken[0];
		exitScore = exitToken.score;
		if(exitScore>LogThreshold)
		{
			for(j=0;j<m_listActivatedNodes[i]->m_numSucc;j++)
			{
				succNode = m_listActivatedNodes[i]->m_succArray[j];
				if(succNode!=NULL)
				{
					if(succNode->m_active==false)
					{
						activateNode(succNode);
					}
					if(succNode->m_stateToken[0].score<exitScore)
					{
						succNode->m_stateToken[0] = exitToken;
						succNode->m_stateToken[0].score = exitScore;
					}
				}
			}

		}
		
	}	    
}



//************************************
// Method:    createWordLink
// FullName:  Decoder::createWordLink
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: int timeStamp
//************************************
void Decoder::createWordLink(int timeStamp)
{
	int i;
	Node* node ;
	token initialToken;
	wordLink *wL;
	for(i=0;i<m_numActivatedNodes;i++)
	{
		node = m_listActivatedNodes[i];
		initialToken = node->m_stateToken[0];
		if(initialToken.score>LogThreshold)
		{
			if(timeStamp!=initialToken.node->m_timeStamp)
			{
				wL = new wordLink();
				if(wL==NULL)
					cout<<"Error in creating wordLink\n";
				wL->tok = initialToken;
				wL->timeStamp = timeStamp;
				initialToken.node->m_timeStamp = timeStamp;
				initialToken.node->m_wL = wL;
				m_wordLinkBuffer.push_back(wL);
			}
			else 
			{
				wL = initialToken.node->m_wL;
			}
			node->m_stateToken[0].wL = wL;
			node->m_stateToken[0].node = node;
		}
	}
}

//************************************
// Method:    traceBack
// FullName:  Decoder::traceBack
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: int t
//************************************
void Decoder::traceBack(int t)
{
	Node *bestNode = NULL;
	float bestScore = LogZero;
	token bestToken;
	bestToken = m_endDummyNode->m_stateToken[0];
	bestNode = m_endDummyNode;
	printResults(bestToken.wL);
}



//************************************
// Method:    printResults
// FullName:  Decoder::printResults
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: wordLink * wL
//************************************
void Decoder::printResults(wordLink *wL)
{
	int timeStamp;
	if(wL!=NULL)
	{
		printResults(wL->tok.wL);
		if(wL->tok.wL!=NULL)
			timeStamp = wL->tok.wL->timeStamp+1;
		else timeStamp = 1;
		cout<<timeStamp<<" "<<wL->timeStamp<<" "<<wL->tok.node->m_symbolName<<" "<<wL->tok.node->m_model->m_modelName<<endl;
	}
}

//************************************
// Method:    tokenizeString
// FullName:  Decoder::tokenizeString
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: const string & str
// Parameter: vector<string> & tokens
// Parameter: const string & delimiters
//************************************
int Decoder::tokenizeString(const string& str, vector<string>& tokens, const string& delimiters)
{
    //	clearing the token list
	string strTemp;

	char strDelim[128];
	char *strToken;
	char strString[1000];

	tokens.clear();

	strcpy(strString, str.c_str());
	strcpy(strDelim, delimiters.c_str());

	strToken = strtok(strString, strDelim);
	while( strToken != NULL )
	{
		strTemp = strToken;
		tokens.push_back(strTemp);
		/* Get next token: */
		strToken = strtok(NULL, strDelim);
	}

	return 1;
}


//************************************
// Method:    decode
// FullName:  Decoder::decode
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & testFileName
//************************************
void Decoder::decode(const vector< vector<float> >& featureVector,string& result,double& timeElapsed)
{
	clock_t startDecoding,endDecoding;
	int i,j,timeStamp = 0;
	float *featureArray = new float[featureVector[0].size()];
	m_maxScore = LogZero;
	activateNetwork();
	startDecoding = clock();
	vector<float> eachFeature;
	for(i=0;i<featureVector.size();i++)
	{
		eachFeature = featureVector[i];
		for(j=0;j<eachFeature.size();j++)
		{
			featureArray[j] = eachFeature[j];
		}
		advanceActivatedNodes(featureArray,timeStamp);
		if(m_doPruning==true)
		{
			pruneActiveNodes();
			
		}
		initializeEntryTokens();
		passTokensToSuccessorsNPenStyle();
		//passTokensToSuccessors();
		createWordLink(timeStamp);
		timeStamp++;
	}
	endDecoding = clock();
	vector<string> res;
	getRecognizedWord(res);
	timeElapsed = diffclock(endDecoding,startDecoding)/1000;
	if(res[0].length()>0)
		result = res[0];
	else result = "";
	delete [] featureArray;
	deactivateNetwork();
	clearWordLinkBuffer();
}


void Decoder::decodeNBestChars(const vector< vector<float> >& featureVector,int N, vector<string>& outRecoResults,vector<float>& outRecoScores)
{


	
	int i,j,timeStamp = 0;
	float *featureArray = new float[featureVector[0].size()];
	m_maxScore = LogZero;
	activateNetwork();
	
	vector<float> eachFeature;
	for(i=0;i<featureVector.size();i++)
	{
		eachFeature = featureVector[i];
		for(j=0;j<eachFeature.size();j++)
		{
			featureArray[j] = eachFeature[j];
		}
		advanceActivatedNodes(featureArray,timeStamp);
		if(m_doPruning==true)
		{
			pruneActiveNodes();
			
		}
		initializeEntryTokens();
		passTokensToSuccessorsNPenStyle();
		//passTokensToSuccessors();
		createWordLink(timeStamp);
		timeStamp++;
	}
	
	getNBest(N,outRecoResults,outRecoScores);

	delete [] featureArray;
	deactivateNetwork();
	clearWordLinkBuffer();
}



//************************************
// Method:    deactivateNetwork
// FullName:  Decoder::deactivateNetwork
// Access:    private 
// Returns:   void
// Qualifier:
//************************************
void Decoder::deactivateNetwork()
{
	Node *node ;
	for(int i=0;i<m_numActivatedNodes;i++)
	{
		node = m_listActivatedNodes[i];
		node->m_active = false;
		node->m_timeStamp = 0;
		node->m_wL = NULL;
	}
	m_numActivatedNodes = 0;
}

//************************************
// Method:    displayTokens
// FullName:  Decoder::displayTokens
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: Node * node
//************************************
void Decoder::displayTokens(Node *node)
{
	cout<<"Displaying Tokens of "<<node->m_symbolName<<endl;
	cout<<"---------------------------------------\n";
	int numStates = node->m_model->m_numStates;
	for(int i=0;i<numStates;i++)
		cout<<node->m_stateToken[i].score<<" ";
	cout<<endl;
	cout<<"---------------------------------------\n";

}
//************************************
// Method:    ~Decoder
// FullName:  Decoder::~Decoder
// Access:    public 
// Returns:   
// Qualifier:
//************************************
Decoder::~Decoder()
{
	//delete m_hList;
	if(m_listActivatedNodes!=NULL)
	{
		delete [] m_listActivatedNodes;
		m_listActivatedNodes = NULL;
	}
	
}

//************************************
// Method:    displayFeatureArray
// FullName:  Decoder::displayFeatureArray
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: float * featureArray
// Parameter: int vecSize
//************************************
void Decoder::displayFeatureArray(float* featureArray,int vecSize)
{

	for(int i=0;i<vecSize;i++)
		cout<<featureArray[i]<<" ";
	cout<<endl;
}


//************************************
// Method:    getResult
// FullName:  Decoder::getResult
// Access:    private 
// Returns:   std::string
// Qualifier:
// Parameter: wordLink * wL
//************************************
string Decoder::getResult(wordLink *wL)
{
	string res;
	while(wL!=NULL)
	{

		if(wL->tok.node->m_symbolName!="!NULL")
		{	
				res=wL->tok.node->m_symbolName+res;
		}
		wL = wL->tok.wL;
	}	
	return res;
		
}


//************************************
// Method:    initialize
// FullName:  Decoder::initialize
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: FSN * network
// Parameter: float pruningThreshold
//************************************
void Decoder::initialize(FSN *network,float pruningThreshold,bool setPruningTrue)
{
	m_doPruning = setPruningTrue;
	m_startDummyNode = network->getStartNode();
	m_endDummyNode = network->getEndNode();

	m_pruningThreshold = pruningThreshold;
	m_hList = network->getHMMList();
	m_vecSize = m_hList->m_vecSize;
	m_listActivatedNodes = NULL;
	m_listActivatedNodes = new Node*[network->getNumNodes()];
	m_numActivatedNodes = 0;
}

void Decoder::getNBest( int N,vector<string>& results,vector<float>& scores )
{
	int i,j;
	float tempScore;
	string tempName;

	vector<float> tokScores;
	vector<string> nodeName;
	Node *node,*temp;
	temp = m_endDummyNode;
	for(i=0;i<temp->m_numPre;i++)
	{
		node = temp->m_preArray[i];
		if(node->m_symbolName=="!NULL")
		{
			temp = node;
			i = -1;
			continue;
		}
		int numStates = node->m_model->m_numStates;
		tokScores.push_back(node->m_stateToken[numStates-1].score);
		nodeName.push_back(node->m_symbolName);
	}
	for(i=0;i<tokScores.size();i++)
	{
		for(j=tokScores.size()-1;j>=(i+1);j--)
		{
			if(tokScores[j-1]<tokScores[j])
			{
				tempScore = tokScores[j-1];
				tokScores[j-1] = tokScores[j];
				tokScores[j] = tempScore;
				tempName = nodeName[j-1];
				nodeName[j-1] = nodeName[j];
				nodeName[j] = tempName;
			}
		}
	}
	

	int possibleNumChoices = N < nodeName.size() ? N : nodeName.size() ;

	if(N <=0)
	{
		possibleNumChoices = 1;
	}
	
	for(i=0;i<possibleNumChoices;i++)
	{
		if(tokScores[i] > LogThreshold)
		{
			results.push_back(nodeName[i]);
			scores.push_back(tokScores[i]);
		}
		
		//cout<<"Choice "<<(i+1)<<":"<<results[i]<<" Score:"<<scores[i]<<endl;
	}

}

//************************************
// Method:    getRecognizedWord
// FullName:  Decoder::getRecognizedWord
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: vector<string> & recognizedWords
//************************************
void Decoder::getRecognizedWord( vector<string>& recognizedWords )
{
	string result;
	if(m_endDummyNode->m_active==true)
	{
		Node *bestNode = m_endDummyNode;
		token bestToken = m_endDummyNode->m_stateToken[0];
		result  = getResult(bestToken.wL);
	}
	
	recognizedWords.push_back(result);
}
//************************************
// Method:    decodeBatchFiles
// FullName:  Decoder::decodeBatchFiles
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & testFileList
//************************************
void Decoder::decodeBatchFiles(const string& testFileList)
{
	ifstream readTestFileName(testFileList.c_str());
	float* featureArray = new float[m_vecSize];
	int timeStamp;
	clock_t startTimer,endTimer;
	while(readTestFileName)
	{
		
		string fileName;
		getline(readTestFileName,fileName,'\n');
		if(!fileName.empty())
		{	
			startTimer = clock();
			ifstream readFeature(fileName.c_str());
			if(readFeature!=NULL)
			{
				timeStamp = 0;
				activateNetwork();
			}
			if(readFeature==NULL)
			{
				cout<<"error in opening feature file\n";
				//	exit(0);
				
			}
			while(readFeature)
			{
				
				string eachLine;
				getline(readFeature,eachLine,'\n');
				vector<string> strFeatVec;
				tokenizeString(eachLine,strFeatVec," ");
				for(int i=0;i<strFeatVec.size();i++)
					featureArray[i] = atof(strFeatVec[i].c_str());
				advanceActivatedNodes(featureArray,timeStamp);
				if(m_doPruning==true)
				{
					pruneActiveNodes();
				}
				initializeEntryTokens();
				passTokensToSuccessorsNPenStyle();
				createWordLink(timeStamp);
				timeStamp++;
			}
			vector<string> res;
			getRecognizedWord(res);
			endTimer = clock();
			if(res[0].length()>0)
				cout<<res[0]<<" ";
			else cout<<"REJECTED ";
			double timeElapsed = diffclock(endTimer,startTimer);
			timeElapsed=timeElapsed/1000;
			cout<<timeElapsed<<endl;
			deactivateNetwork();
			clearWordLinkBuffer();
			
		}
		delete [] featureArray;
	}

	/*cout<<"Total Time Taken in resetting probabilities "<<timeResetting<<endl;
	cout<<"Total Time Taken in advancing nodes "<<timeAdvancing<<endl;
	cout<<"Total Time Taken in pruning nodes "<<timePruning<<endl;
	cout<<"Total Time Taken in initializing entry tokens "<<timeEntry<<endl;
	cout<<"Total Time Taken in passing tokens "<<timePassing<<endl;
	cout<<"Total Time Taken in creating wordLinks "<<timeCrossLink<<endl;
	*/

	readTestFileName.close();
	
	delete [] featureArray;
}

void Decoder::clearWordLinkBuffer()
{
	wordLink *wL;
	while(!m_wordLinkBuffer.empty())
	{
		wL = m_wordLinkBuffer.back();
		if(wL!=NULL)
		{
			delete wL;
			wL = NULL;
		}
		m_wordLinkBuffer.pop_back();
	}
	
}

//************************************
// Method:    passTokensToSuccessorsNPenStyle
// FullName:  Decoder::passTokensToSuccessorsNPenStyle
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void Decoder::passTokensToSuccessorsNPenStyle()
{
	Node* succNode;
	int i,j,numPreviousActiveNodes = m_numActivatedNodes;
	float exitScore;
	token exitToken;
	float beamLimit = m_maxScore-m_pruningThreshold;
	for(i=0;i<numPreviousActiveNodes;i++)
	{
		exitToken = m_listActivatedNodes[i]->m_exitToken[0];
		exitScore = exitToken.score;
		if(m_listActivatedNodes[i]->m_numSucc>0)
			//[Balaji][13th Nov 2008] 'log': ambiguous call to overloaded function
			exitScore-=log((float)m_listActivatedNodes[i]->m_numSucc);
		if(exitScore>beamLimit)
		{
			for(j=0;j<m_listActivatedNodes[i]->m_numSucc;j++)
			{
				succNode = m_listActivatedNodes[i]->m_succArray[j];
				if(succNode!=NULL)
				{
					if(succNode->m_active==false)
					{
						activateNode(succNode);
					}
					if(succNode->m_stateToken[0].score<exitScore)
					{
						succNode->m_stateToken[0] = exitToken;
						succNode->m_stateToken[0].score = exitScore;
					}
				}
			}


		}
	
	}	    
	
}