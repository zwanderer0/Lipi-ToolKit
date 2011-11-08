/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2007-05-14 14:33:50 +0530 (Mon, 14 May 2007) $
 * $Revision: 94 $
 * $Author: sharmnid $
 *
 ************************************************************************/
#include "LTKDictionary.h"

/*int main()
{
	Dictionary d;
	d.loadDictionary("E:/Bharath/experiments/dictionary/dictionary.txt");
	d.printVectors();

	vector<int> testWord;

	testWord.push_back(1);
	testWord.push_back(12);
	testWord.push_back(15);
	testWord.push_back(5);


	vector<int> nextSymbols;
	cout<<"Valid:"<<d.getNextSymbols(testWord,nextSymbols)<<endl;
	cout<<"Next Symbols..."<<endl;

	for(int n=0;n<nextSymbols.size();++n)
	{
		cout<<nextSymbols[n]<<",";
	}

	vector<int> word1;
	vector<int> word2;

	word1.push_back(1);
	word1.push_back(2);

	word1.push_back(3);
	word1.push_back(4);



	word2.push_back(1);
	word2.push_back(2);

	word2.push_back(3);
	word2.push_back(4);
	word2.push_back(5);
	word2.push_back(6);

	vector<int> word3;
	word3.push_back(1);
	word3.push_back(12);
	word3.push_back(15);


	vector<int> word4;
	word4.push_back(2);
	word4.push_back(5);





	cout<<"Edit dist:"<<d.getEditDistance(word3,word4)<<endl;

	cout<<endl;


	multimap<float,vector<int> > editDistanceWordMap;
	d.getTopNMatches(word3,editDistanceWordMap,10);

	cout<<"Top N Matches..."<<endl;
	for(multimap<float,vector<int> >::iterator mapIter=editDistanceWordMap.begin();mapIter!=editDistanceWordMap.end();++mapIter)
	{
		cout<<mapIter->first<<"-";
		vector<int>& matchedWord=mapIter->second;
		for(int i=0;i<matchedWord.size();++i)
		{
			cout<<matchedWord[i]<<",";
		}
		cout<<endl;
	}

	return 0;
}*/
