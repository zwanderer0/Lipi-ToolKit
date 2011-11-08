/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2007-05-14 14:33:50 +0530 (Mon, 14 May 2007) $
 * $Revision: 94 $
 * $Author: sharmnid $
 *
 ************************************************************************/

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
//#include <LTKInc.h>

//#include "LTKWordRecoResult.h"

using namespace std;



class LTKDictionary
{
private:

	vector<vector<int> > m_dictionaryWords;			//dictionary word

	vector<vector<bool> > m_endOfWord;		//end of word tag

	vector<vector<long> > m_nextChar;		//position of the next char that could match

	vector<vector<int> > m_trie;

	float m_insertionCost;
	float m_deletionCost;


	int tokenizeString(const string& str, vector<string>& tokens, const string& delimiters)
	{
		//	clearing the token list

		tokens.clear();

		//	skipping delimiters at beginning

		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		//	find first "non-delimiter"

		string::size_type pos = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			//	a token found. adding it to the vector

			tokens.push_back(str.substr(lastPos, pos - lastPos));
   
			//	skipping delimiters
        
			lastPos = str.find_first_not_of(delimiters, pos);

			//	find next "non-delimiter"

			pos = str.find_first_of(delimiters, lastPos);
		}

		return 0;
	}



public:

	LTKDictionary():m_insertionCost(1.0f),m_deletionCost(1.0f)
	{
		
	}

	int loadDictionary(string dictionaryFilePath)
	{
		ifstream input(dictionaryFilePath.c_str());

		if(!input)
		{
			cout<<"Dictionary file "<<dictionaryFilePath<<" does not exit"<<endl;
			return 1;
		}
		else
		{
			while(input)
			{
				string eachLine;
				
				getline(input,eachLine);

				//cout<<"eachLine:"<<eachLine<<endl;

				if(!eachLine.empty())
				{
					
					bool found = false;

					vector<string> strWord;

					tokenizeString(eachLine,strWord,",");


					vector<int> currentWord(strWord.size());

					for(int currWordIndex=0;currWordIndex<currentWord.size();++currWordIndex)
					{
						currentWord[currWordIndex]=atoi(strWord[currWordIndex].c_str());
						
					}

					m_dictionaryWords.push_back(currentWord);



					int currentWordIndex=0;
					int toChangeOmitCharRow=0,toChangeOmitCharCol=0;
					bool matchesExactly=false;

					for(int wordListIndex=0;wordListIndex<m_trie.size();++wordListIndex)
					{
						
						for(int eachWordIndex=0;eachWordIndex<m_trie[wordListIndex].size();++eachWordIndex)
						{
							//cout<<"comparing "<<m_trie[wordListIndex][eachWordIndex]<<" & "<<currentWord[currentWordIndex]<<" wli:"<<wordListIndex<<" ewi:"<<eachWordIndex<<endl;
							//getchar();
							
							
							if(m_trie[wordListIndex][eachWordIndex]==currentWord[currentWordIndex])
							{

								found=true;

								++currentWordIndex;
								
								if(currentWordIndex==currentWord.size())
								{
									matchesExactly=true;
									m_endOfWord[wordListIndex][eachWordIndex]=true;
									
									break;
								}

							}
							else
							{
								if(m_nextChar[wordListIndex][eachWordIndex]>0)
								{
									wordListIndex=m_nextChar[wordListIndex][eachWordIndex]-1;
									found=true;
									break;

								}
								else
								{
									toChangeOmitCharRow=wordListIndex;
									toChangeOmitCharCol=eachWordIndex;
									found=false;
									
									break;
								}
							}

						

						}

						if(matchesExactly || !found)
						{
							break;
						}

						
					}

					if(!found)
					{
						vector<int> remainingWord(currentWord.begin()+currentWordIndex,currentWord.end());
						int newWordSize = remainingWord.size();
						vector<long> nextCharVec(newWordSize);
						vector<bool> endOfWordVec(newWordSize);
						if(m_trie.size()>0)
						{
							m_nextChar[toChangeOmitCharRow][toChangeOmitCharCol]=m_trie.size();
						}
						m_trie.push_back(remainingWord);
						m_nextChar.push_back(nextCharVec);
						m_endOfWord.push_back(endOfWordVec);
						m_endOfWord[m_endOfWord.size()-1][endOfWordVec.size()-1]=true;

						
					}

						
				}
			}
		}


					/*cout<<"Original dictionary..."<<endl;

					for(int x=0;x<m_dictionaryWords.size();++x)
					{
						for(int y=0;y<m_dictionaryWords[x].size();++y)
						{
							cout<<m_dictionaryWords[x][y]<<",";
						}
						cout<<endl;
					}*/


	}


	bool getNextPossibleSymbols(vector<int>& word,vector<int>& nextSymbols)
	{
		int findWordIndex=0;

		//bool isValid=false;

		int wordSize=word.size();

		for(int wordListIndex=0;wordListIndex<m_trie.size();++wordListIndex)
		{
			for(int eachWordIndex=0;eachWordIndex<m_trie[wordListIndex].size();++eachWordIndex)
			{
				if(findWordIndex<wordSize)
				{
					if(word[findWordIndex]==m_trie[wordListIndex][eachWordIndex])
					{
						if(findWordIndex==wordSize-1)
						{
							if(m_endOfWord[wordListIndex][eachWordIndex]==true && eachWordIndex==m_trie[wordListIndex].size()-1)
							{
								return true;
							
							}
						}
						else if(m_endOfWord[wordListIndex][eachWordIndex]==true && eachWordIndex==m_trie[wordListIndex].size()-1)
						{
							return false;
						}
						++findWordIndex;
					}
					else
					{
						if(m_nextChar[wordListIndex][eachWordIndex]>0)
						{
							wordListIndex=m_nextChar[wordListIndex][eachWordIndex]-1;
							break;
						}
						else
						{
							return false;
						}

						
					}
				}
				else
				{

					nextSymbols.push_back(m_trie[wordListIndex][eachWordIndex]);
					if(m_nextChar[wordListIndex][eachWordIndex]>0)
					{
						wordListIndex=m_nextChar[wordListIndex][eachWordIndex]-1;
						break;
					}
					else
					{
						return true;
					}

				}


			}
		}


		return false;

		
	}




	/*void getTopNMatches(vector<int>& word,vector<vector<int> >& topNMatches,int N)
	{
		multimap<float,vector<int> > editDistanceWordMap;
		
		vector<vector<float> > costMatrix; float eachWordEditDistance=0.0f;

		int findWordIndex=0;

		vector<int> eachWord;
		vector<int> substringWord;

		for(int wordListIndex=0;m_trie.size();++wordListIndex)
		{
			for(int eachWordIndex=0;eachWordIndex<m_trie[wordListIndex].size();++eachWordIndex)
			{
				eachWord.push_back(m_trie[wordListIndex][eachWordIndex]);
				substringWord.push_back(m_trie[wordListIndex][eachWordIndex]);
				if(m_endOfWord[wordListIndex][eachWordIndex]==true)
				{
					vector<vector<float> > substringCostMatrix;
					eachWordEditDistance=getEditDistanceWithCostMatrix(word,substringWord,substringCostMatrix);
					substringWord.clear();
					costMatrix.insert(costMatrix.end(),substringCostMatrix.begin(),substringCostMatrix.end());
					editDistanceWordMap.insert(make_pair(eachWordEditDistance,eachWord));
					if(editDistanceWordMap.size()>N)
					{
						multimap<float,vector<int> >::iterator mapIter=editDistanceWordMap.begin();
						for(int s=0;s<N;++s)
						{
							++mapIter;
						}
						editDistanceWordMap.erase(mapIter);
					}
					if(eachWordIndex==m_trie[wordListIndex].size()-1)
					{
						for(int backwardIndex=eachWordIndex;backwardIndex>=0;--backwardIndex)
						{
							if(m_nextChar[wordListIndex][eachWordIndex]>0)
							{
							eachWord.pop_back();
							costMatrix.pop_back();
							}
						}
					}
				}
			}
		}
		
	}*/


	void setEditCosts(float insertionCost,float deletionCost)
	{
		m_insertionCost=insertionCost;
		m_deletionCost=deletionCost;
	}

	void getTopNMatches(vector<int>& word,multimap<float,vector<int> >& topNMatches,int N)
	{
	
		multimap<float,vector<int> >::iterator mapIter;

		float eachWordEditDistance;

		for(vector<vector<int> >::iterator wordListIter=m_dictionaryWords.begin();wordListIter!=m_dictionaryWords.end();++wordListIter)
		{
			eachWordEditDistance=getEditDistance(word,*wordListIter,m_insertionCost,m_deletionCost);

			topNMatches.insert(make_pair(eachWordEditDistance,*wordListIter));

			if(topNMatches.size()>N)
			{
				mapIter=topNMatches.begin();
				for(int s=0;s<N;++s)
				{
					++mapIter;
				}
				topNMatches.erase(mapIter);
			}

		}



	}


	static float findCharDist(const int x,const int y)
	{
		
		return (x==y)?0.0f:1.0f;
	}


	static float getEditDistance(vector<int>& word1,vector<int>& word2,float insertionCost=1.0f,float deletionCost=1.0f)
	{
		vector<vector<float> > distanceMatrix;
		return getEditDistanceWithCostMatrix(word1,word2,distanceMatrix,insertionCost,deletionCost);
	}


	static float getEditDistanceWithCostMatrix(vector<int>& word1,vector<int>& word2,vector<vector<float> >& distanceMatrix,float insertionCost=1.0,float deletionCost=1.0)
	{
		float editDistance;

		int m=word1.size();
		int n=word2.size();

		int row=0; int col=0;

		//vector<vector<float> > distanceMatrix(m);
		distanceMatrix.resize(m+1);
		for(row=0;row<=m;++row)
		{
			vector<float> eachRow(n+1);
			distanceMatrix[row]=eachRow;
		}

		for(col=0;col<=n;++col)
		{
			distanceMatrix[0][col]=col;
		}

		for(row=0;row<=m;++row)
		{
			distanceMatrix[row][0]=row;
		}

		for(row=1;row<=m;++row)
		{
			for(col=1;col<=n;++col)
			{
				float insCost=distanceMatrix[row-1][col]+insertionCost;
				float delCost=distanceMatrix[row][col-1]+deletionCost;
				float subCost=distanceMatrix[row-1][col-1]+findCharDist(word1[row],word2[col]);

				if(insCost < delCost && insCost < subCost)
				{
					distanceMatrix[row][col]= insCost;
				}
				else if(delCost < insCost && delCost < subCost)
				{
					distanceMatrix[row][col]=delCost;
				}
				else
				{
					distanceMatrix[row][col]=subCost;
				}
			}
		}


		editDistance=distanceMatrix[m][n];

		/*cout<<endl<<"Printing the matrix..."<<endl;

		for(int x=0;x<distanceMatrix.size();++x)
		{
			for(int y=0;y<distanceMatrix[x].size();++y)
			{
				cout<<distanceMatrix[x][y]<<",";
			}
			cout<<endl;
		}*/
		return editDistance;
	}





	void printVectors()
	{
		cout<<"printing m_dictionary..."<<endl;
		for(int i=0;i<m_trie.size();++i)
		{
			for(int ii=0;ii<m_trie[i].size();++ii)
			{
				cout<<m_trie[i][ii]<<",";
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<"printing m_nextChar..."<<endl;
		for(int j=0;j<m_nextChar.size();++j)
		{
			for(int jj=0;jj<m_nextChar[j].size();++jj)
			{
				cout<<m_nextChar[j][jj]<<",";
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<"printing m_endOfWord..."<<endl;
		for(int k=0;k<m_endOfWord.size();++k)
		{
			for(int kk=0;kk<m_endOfWord[k].size();++kk)
			{
				cout<<m_endOfWord[k][kk]<<",";
			}
			cout<<endl;
		}
		cout<<endl;
		
	}

};

