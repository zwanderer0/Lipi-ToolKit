#include "FSN.h"
#include "HMModel.h"
#include<fstream>
#include<string>
#include<vector>
#include<iostream>
#include<algorithm>
#include<map>


using namespace std;

static int endNodeIndex=-1 , endDummyIndex=-1;


//************************************
// Method:    FSN
// FullName:  FSN::FSN
// Access:    public 
// Returns:   
// Qualifier:
//************************************
FSN::FSN()
{
}

//************************************
// Method:    ~FSN
// FullName:  FSN::~FSN
// Access:    public 
// Returns:   
// Qualifier:
//************************************
FSN::~FSN()
{
	if(m_wordAdded==true && m_wordListInitializtion==false)
	{
		writeNetwork("newNetwork.txt");
	}
	if(m_wordAdded==true && m_wordListInitializtion==true)
	{
		ofstream wordListOutput(m_wordListFileName.c_str(),ios::app);
		for(int i=0;i<m_wordsAdded.size();i++)
		{
			wordListOutput<<m_wordsAdded[i]<<endl;
		}
		wordListOutput.close();
	}
	if(m_hList!=NULL)
	{
		delete m_hList;
		m_hList = NULL;
	}
	if(m_listNodes!=NULL)
	{
		for(int i=0;i<m_numNodes;i++)
			delete  m_listNodes[i];
		delete [] m_listNodes;
		m_listNodes = NULL;
	}
}

//************************************
// Method:    tokenizeString
// FullName:  FSN::tokenizeString
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: const string & str
// Parameter: vector<string> & tokens
// Parameter: const string & delimiters
//************************************
void FSN::tokenizeString(const string& str,vector<string> &tokens,const string& delimiters)
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

}
//************************************
// Method:    readDictionaryFile
// FullName:  FSN::readDictionaryFile
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: const string & dictionaryFile
//************************************
void FSN::readDictionaryFile( const string& dictionaryFile )
{
	ifstream readDictionary(dictionaryFile.c_str());
	int numModels;
	HMModel *newSerialModel;
	while(readDictionary)
	{
		string eachLine;
		getline(readDictionary,eachLine,'\n');
		if(!eachLine.empty())
		{
			vector<string> lineTokens;
			tokenizeString(eachLine,lineTokens," ");
			if(lineTokens.size()>2)
			{
				HMModel** hModels = new HMModel*[lineTokens.size()-1];
				numModels = lineTokens.size()-1;
				for(int i=1;i<lineTokens.size();i++)
				{
					
					hModels[i-1] = m_hList->getModel(lineTokens[i]);
				}
				newSerialModel = new HMModel();
				newSerialModel->combineModelsSerial(hModels,numModels);
				m_hList->addModelToList(newSerialModel);
				delete[] hModels;
			}
			else
			{
				newSerialModel = m_hList->getModel(lineTokens[1]);
			}
			
			
			if(m_nodeMap.count(lineTokens[0])>0)
			{
				HMModel** parallelHMMs = new HMModel*[2];
				numModels = 2;
				HMModel* newParallelModel = new HMModel();
				parallelHMMs[0] = newSerialModel;
				parallelHMMs[1] = m_nodeMap[lineTokens[0]];
				newParallelModel->combineModelsParallel(parallelHMMs,numModels);
				m_nodeMap.erase(lineTokens[0]);
				m_hList->addModelToList(newParallelModel);
				m_nodeMap[lineTokens[0]] = newParallelModel;
				delete [] parallelHMMs;
			}
			
			else
			{
				m_nodeMap[lineTokens[0]]= newSerialModel;
			}
		}
	}
	readDictionary.close();


}



//************************************
// Method:    matchNode
// FullName:  FSN::matchNode
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: Node * parentNode
// Parameter: string symbolName
//************************************
int FSN::matchNode(Node *parentNode,string symbolName)
{
	for(int i=0;i<parentNode->m_numSucc;i++)
	{
		if(parentNode->m_succArray[i]->m_symbolName==symbolName)
			return i;
	}
	return -1;
}
//************************************
// Method:    addToNetwork
// FullName:  FSN::addToNetwork
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: string word
// Parameter: Node * node
// Parameter: string delimiter
//************************************
void FSN::addToNetwork(string word,Node *node,string delimiter)
{
	vector<string> wordTokens;
	cout<<"Word to be inserted is "<<word<<endl;
	int matchResult=-1,index = 0,nodeType = 0,modelNum = -1,numStates=0;
	Node *newNode;
	tokenizeString(word,wordTokens,delimiter);
	while(node!=NULL && index<(int)wordTokens.size())
	{
		matchResult = matchNode(node,wordTokens[index]);
		if(matchResult==-1)
			break;
		else 
		{
			
			node = node->m_succArray[matchResult];
			if(index==wordTokens.size()-1)
			{
				cout<<"Node to be added is "<<node->m_symbolName<<endl;	
				node->addSuccArrayNet(m_endDummyNode);
				m_endDummyNode->addPreArrayNet(node);
				m_numEdges++;;
			}
			index++;
		}	
	}
	for(int i=index;i<(int)wordTokens.size();i++)
	{
		if(i==0)
		{
			nodeType = 0;
		}
		if(i==(wordTokens.size()-1))
		{
			nodeType = 2;
		}
		if(i>0 && i<(wordTokens.size()-1))
		{
			nodeType = 1;
		}
		numStates = m_nodeMap[wordTokens[i]]->m_numStates;
		newNode = new Node(wordTokens[i],m_nodeMap[wordTokens[i]],nodeType);
		addToList(newNode);
		if(nodeType==2)
		{
			newNode->addSuccArrayNet(m_endDummyNode);
			m_endDummyNode->addPreArrayNet(newNode);
			m_numEdges++;
		}
		node->addSuccArrayNet(newNode);
		newNode->addPreArrayNet(node);
		m_numEdges++;
		node = newNode;

	}
}
//************************************
// Method:    displayTRIE
// FullName:  FSN::displayTRIE
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: Node * node
// Parameter: string path
//************************************
void FSN::displayTRIE(Node *node,string path)
{
	if(node->m_nodeType==4)
		cout<<path<<endl;
	for(int i=0;i<node->m_numSucc;i++)
	{
		if(node->m_symbolName!="!NULL")
			displayTRIE(node->m_succArray[i],path+node->m_symbolName);
		else displayTRIE(node->m_succArray[i],path);
	}
	
}
//************************************
// Method:    getStartNode
// FullName:  FSN::getStartNode
// Access:    public 
// Returns:   Node*
// Qualifier:
//************************************
Node* FSN::getStartNode()
{
	return m_startDummyNode;
}

//************************************
// Method:    getEndNode
// FullName:  FSN::getEndNode
// Access:    public 
// Returns:   Node*
// Qualifier:
//************************************
Node* FSN::getEndNode()
{
	return m_endDummyNode;
}


//************************************
// Method:    processNetwork
// FullName:  FSN::processNetwork
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: const string & networkFile
//************************************
void FSN::processNetwork( const string& networkFile )
{
	processNodes(networkFile);
	processEdges(networkFile);
}

//************************************
// Method:    processNodes
// FullName:  FSN::processNodes
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: const string & networkFile
//************************************
void FSN::processNodes( const string& networkFile )
{
	FILE *netFile = fopen(networkFile.c_str(),"r");



	int i,j;

	vector<int> indexNullNodes;

	if(netFile==NULL)
	{
		cout<<"Error in opening network file\n";
	//	exit(0);
	}

	char version[100],nodeName[1000];

	string nodeNameStr;

	int startNodeNum,endNodeNum,nodeNum,numEdges,numNodes,numStates,nodeType=0,edgeNum;

	fgets(version,100,netFile);

	fscanf(netFile,"%*1s%*1s%d %*1s%*1s%d",&numNodes,&numEdges);

	m_numEdges = numEdges;

	m_listNodes = new Node*[numNodes];

	m_numNodes = numNodes;

	for(i=0;i<m_numNodes;i++)
	{
		fscanf(netFile,"%*1s%*1s%d %*1s%*1s%s",&nodeNum,&nodeName);

		nodeNameStr.assign(nodeName);
		
		if(nodeNameStr=="!NULL")
		{

			m_listNodes[i] = new Node(nodeNameStr);
			m_listNodes[i]->m_listIndex = i;
			indexNullNodes.push_back(i);
		}
		else 
		{
			numStates = m_nodeMap[nodeNameStr]->m_numStates;
			m_listNodes[i] = new Node(nodeNameStr,m_nodeMap[nodeNameStr],0);
			m_listNodes[i]->m_listIndex = i;
			
		}
	}

	for(j=0;j<numEdges;j++)
	{
		fscanf(netFile,"%*1s%*1s%d %*1s%*1s%d %*1s%*1s%d",&edgeNum,&startNodeNum,&endNodeNum);
		m_listNodes[startNodeNum]->m_numSucc++;
		m_listNodes[endNodeNum]->m_numPre++;
	}
	fclose(netFile);
	for(i=0;i<indexNullNodes.size();i++)
	{

		if(m_listNodes[indexNullNodes[i]]->m_numPre==0)
			m_startDummyNode = m_listNodes[indexNullNodes[i]];


		else if(m_listNodes[indexNullNodes[i]]->m_numSucc==0)
			m_endDummyNode = m_listNodes[indexNullNodes[i]];

		else 
			endDummyIndex = indexNullNodes[i];

	}
}

//************************************
// Method:    processEdges
// FullName:  FSN::processEdges
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: const string & networkFile
//************************************
void FSN::processEdges( const string& networkFile )
{
	FILE *netFile = fopen(networkFile.c_str(),"r");

	if(netFile==NULL)
	{
		cout<<"Error in opening network file\n";
		exit(0);
	}

	for(int m=0;m<m_numNodes;m++)
	{
		int numPre = m_listNodes[m]->m_numPre;
		int numSucc = m_listNodes[m]->m_numSucc;
		m_listNodes[m]->m_preArray = new Node*[numPre];
		m_listNodes[m]->m_succArray = new Node*[numSucc];

	}

	char version[100],nodeName[1000];

	int startNodeNum,endNodeNum,nodeNum,numEdges,numNodes,edgeNum;

	fgets(version,100,netFile);

	fscanf(netFile,"%*1s%*1s%d %*1s%*1s%d",&numNodes,&numEdges);

	for(int i=0;i<m_numNodes;i++)
	{
		fscanf(netFile,"%*1s%*1s%d %*1s%*1s%s",&nodeNum,&nodeName);

		
	}

	for(int j=0;j<numEdges;j++)
	{
		fscanf(netFile,"%*1s%*1s%d %*1s%*1s%d %*1s%*1s%d",&edgeNum,&startNodeNum,&endNodeNum);
		m_listNodes[startNodeNum]->addSuccArrayNet(m_listNodes[endNodeNum]);
		m_listNodes[endNodeNum]->addPreArrayNet(m_listNodes[startNodeNum]);			
	}

	fclose(netFile);
	m_endDummyNode->m_nodeType = 4;
}


//************************************
// Method:    initializeFromNetworkFile
// FullName:  FSN::initializeFromNetworkFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & dictionaryFile
// Parameter: const string & networkFile
// Parameter: const string & modelsList
// Parameter: const string & trainedHMMFile
//************************************
void FSN::initializeFromNetworkFile( const string& dictionaryFile,const string& networkFile,const string& modelsList,const string& trainedHMMFile )
{
	m_wordAdded = false;
	m_listNodes = NULL;
	m_wordListInitializtion = false;
	m_hList = new HMMList();
	m_hList->readHmmDefinition(trainedHMMFile,modelsList);

	readDictionaryFile(dictionaryFile);

	processNetwork(networkFile);

}




//************************************
// Method:    addWord
// FullName:  FSN::addWord
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string word
// Parameter: string delimiter
//************************************
void FSN::addWord(string word,string delimiter)
{
	m_wordAdded = true;
	if(m_wordListInitializtion==false)
		addToNetwork(word,m_startDummyNode,delimiter);
	else 
	{
		addWordToTRIE(word,m_startDummyNode,delimiter);
		m_wordsAdded.push_back(word);
	}
}

//************************************
// Method:    addToList
// FullName:  FSN::addToList
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: Node * node
//************************************
void FSN::addToList(Node* node)
{
	Node** tempArray ;
	tempArray = new Node*[m_numNodes+1];
	for(int i=0;i<m_numNodes;i++)
		tempArray[i] = m_listNodes[i];
	delete [] m_listNodes;
	m_listNodes = tempArray;
	m_listNodes[m_numNodes] = node;
	m_listNodes[m_numNodes]->m_listIndex = m_numNodes;
	m_numNodes++;
}

//************************************
// Method:    writeNetwork
// FullName:  FSN::writeNetwork
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & outputNetworkFile
//************************************
void FSN::writeNetwork( const string& outputNetworkFile )
{
	ofstream writeOutput(outputNetworkFile.c_str());
	int i,j;
	writeOutput<<"VERSION=1.0"<<endl;
	writeOutput<<"N="<<m_numNodes<<" L="<<m_numEdges<<endl;
	for(i=0;i<m_numNodes;i++)
	{
		writeOutput<<"I="<<i<<" W="<<m_listNodes[i]->m_symbolName<<endl;
	}
	int startNodeNum,endNodeNum,edgeNum=0;
	for(i=0;i<m_numNodes;i++)
	{
		int numSucc = m_listNodes[i]->m_numSucc;
		startNodeNum = m_listNodes[i]->m_listIndex;
		for(j=0;j<numSucc;j++)
		{
			endNodeNum = m_listNodes[i]->m_succArray[j]->m_listIndex;
			writeOutput<<"J="<<edgeNum<<"  S="<<startNodeNum<<" E="<<endNodeNum<<endl;
			edgeNum++;
		}
	}
	writeOutput.close();

}


//************************************
// Method:    initializeFromWordFile
// FullName:  FSN::initializeFromWordFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & dictionaryFile
// Parameter: const string & wordFile
// Parameter: const string & modelsList
// Parameter: const string & trainedHMMFile
//************************************


void FSN::initializeFromWordFile( const string& dictionaryFile,const string& wordFile,const string& modelsList,const string& trainedHMMFile,const string& delimiter )
{
	m_wordListFileName = wordFile;
	m_numNodes = 0;
	m_numEdges = 0;
	m_listNodes = NULL;
    m_wordListInitializtion = true;
	int maxSuccTRIE = 0, maxPreTRIE = 0, maxEndPre = 0;
	m_hList = new HMMList();
	m_hList->readHmmDefinition(trainedHMMFile,modelsList);
	readDictionaryFile(dictionaryFile);
	vector<string> wordList;
	readWordFile(wordFile,wordList);
	maxSuccTRIE = m_nodeMap.size();
	maxPreTRIE = m_nodeMap.size();
	maxEndPre = wordList.size();
	m_startDummyNode = new Node("!NULL",maxPreTRIE,maxSuccTRIE);
	m_numNodes++;
	m_endDummyNode = new Node("!NULL",maxEndPre,maxSuccTRIE);
	m_numNodes++;
	for(int i=0;i<wordList.size();i++)
		addWordToTRIE(wordList[i],m_startDummyNode,delimiter);
}

//************************************
// Method:    readWordFile
// FullName:  FSN::readWordFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const string & dictionaryFile
// Parameter: vector<string> & wordList
//************************************
void FSN::readWordFile(const string &wordFile,vector<string>& wordList )
{
	ifstream readWord(wordFile.c_str());
	while(readWord)
	{
		string eachLine;
		getline(readWord,eachLine,'\n');
		if(!eachLine.empty())
		{
			wordList.push_back(eachLine);
		}
	}
	readWord.close();
	
}

//************************************
// Method:    addWordToTRIE
// FullName:  FSN::addWordToTRIE
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string word
// Parameter: Node * node
// Parameter: int maxSucc
// Parameter: int maxPre
//************************************
void FSN::addWordToTRIE( string word,Node* node,const string& delimiter )
{
	vector<string> wordTokens;
	int matchResult,index = 0,nodeType = 0,modelNum = -1,numStates=0;
	int maxSucc = m_nodeMap.size();
	int maxPre = m_nodeMap.size();
	Node *newNode;
	tokenizeString(word,wordTokens,delimiter);
	while(node!=NULL && index<(int)wordTokens.size())
	{
		matchResult = matchNode(node,wordTokens[index]);
		if(matchResult==-1)
			break;
		else 
		{
			node = node->m_succArray[matchResult];
			if(index==wordTokens.size()-1)
			{
				//cout<<"Node to be added is "<<node->m_symbolName<<endl;	
				node->addSuccArray(m_endDummyNode);
				m_endDummyNode->addPreArray(node);
				m_numEdges++;;
			}
			index++;
		}	
	}
	for(int i=index;i<(int)wordTokens.size();i++)
	{
		if(i==0)
		{
			nodeType = 0;
		}
		if(i==(wordTokens.size()-1))
		{
			nodeType = 2;
		}
		if(i>0 && i<(wordTokens.size()-1))
		{
			nodeType = 1;
		}
		//cout<<"name of the node is "<<wordTokens[i]<<endl;
		numStates = m_nodeMap[wordTokens[i]]->m_numStates;
		newNode = new Node(wordTokens[i],nodeType,m_nodeMap[wordTokens[i]],maxPre,maxSucc);
		m_numNodes++;
		
		newNode->displayNode();
		if(nodeType==2)
		{
			newNode->addSuccArray(m_endDummyNode);
			m_endDummyNode->addPreArray(newNode);
		}
		node->addSuccArray(newNode);
		newNode->addPreArray(node);
		node = newNode;	
	}
}