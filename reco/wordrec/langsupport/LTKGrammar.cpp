/****************************************************************************
* Copyright (c) HP Labs India. All rights reserved.
* This source code is a property of HP Labs India. and is copyright protected.
* UNATHORISED ACCESS, COPYING, REPRODUCTION OR DISTRIBUTION of this program
* is strictly PROHIBITED.
****************************************************************************/

/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate: 2008-07-18 15:00:39 +0530 (Fri, 18 Jul 2008) $
 * $Revision: 561 $
 * $Author: sharmnid $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR: Implementation of LTKRecognitionContext that holds the context
 *             for recognition
 *
 * CONTENTS: 
 *   initialize 
 *   isValid 
 *   isValidWord
 *   findPossiblePaths 
 *   replacePaths 
 *   getPossibleTerminals
 *   doesPathStartWith 
 *   trim
 *
 * AUTHOR:     Bharath A.
 *
 * DATE:       August 16, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#include "LTKGrammar.h"
#include "LTKLoggerUtil.h"

/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: LTKGrammar
* DESCRIPTION	: Default Constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/             
   	
LTKGrammar::LTKGrammar():isInitialized(false)
{
   LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered constructor of LTKGrammar" <<endl;
   LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting constructor of LTKGrammar" <<endl;
}



/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: LTKGrammar
* DESCRIPTION	: Default Destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/             

		LTKGrammar::~LTKGrammar()
		{
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered destructor of LTKGrammar" <<endl;

			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting destructor of LTKGrammar" <<endl;
		}




/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: initialize
* DESCRIPTION	: This function sets the grammar file, reads the CFG, populates the data structures(maps) and determines possible paths
* ARGUMENTS		: grammarFileName - rules file name that has grammar specified as CFG and terminal-symbol mapping
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     
		
		void LTKGrammar::initialize(string grammarFileName)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKGrammar::intialize" <<endl;
			vector<string> symbols;		//holds the terminals and productions
			vector<string> terminals;	//holds terminals

			ifstream input(grammarFileName.c_str());

			if(input)
			{
				bool gotStartSymbol=false;

				//reading the grammar file line by line
				while(!input.eof())
				{
					string eachLine;	//holds each line of the grammar file

					getline(input,eachLine);

					if(!eachLine.empty())
					{

						//if the line is production definition and NOT symbol ID map
						if(eachLine.find_first_of("{")==eachLine.npos)
						{
							eachLine=trim(eachLine);	//trim is called everwhere to remove leading and trailing spaces of the string
							
							vector<string> ruleLeftRight;	//holds the CFG rule (Production name and its values)

							LTKStringUtil::tokenizeString(eachLine, "=", ruleLeftRight);

							vector<string> termsVec;	//holds each expression of the production value

							LTKStringUtil::tokenizeString(trim(ruleLeftRight[1]), "|", termsVec);

							vector<vector<string> > cfgMapValue;	//Holds all value expressions for a production

							for(int i=0;i<termsVec.size();++i)
							{
								vector<string> termContents;

								LTKStringUtil::tokenizeString(trim(termsVec[i]), ",", termContents);

								for(int j=0;j<termContents.size();++j)
								{
									termContents[j]=trim(termContents[j]);
									bool containsSymbol=false;

									//making sure the list of terminals and production names in the symbols vector is unique
									for(int k=0;k<symbols.size();++k)
									{
										if(symbols[k]==termContents[j])	//if it already contains
										{

											containsSymbol=true;
											break;
										}
										
									}
									if(!containsSymbol)	//if it doesn't contain
									{

										symbols.push_back(termContents[j]);
									}
								}
								cfgMapValue.push_back(termContents);
							}

							//populating the m_cfgMap which represents the CFG
							m_cfgMap.insert(make_pair(trim(ruleLeftRight[0]),cfgMapValue));

							if(!gotStartSymbol)
							{
								m_startSymbol=trim(ruleLeftRight[0]);	//getting the start symbol of the CFG
								gotStartSymbol=true;
							}

						}
						else	//It is map for symbol IDs
						{
							vector<string> keyValue;	//holds the terminal name

							LTKStringUtil::tokenizeString(eachLine, "=", keyValue);

							keyValue[0]=trim(keyValue[0]);
							keyValue[1]=trim(keyValue[1]);
							keyValue[1]=keyValue[1].substr(1,keyValue[1].length()-2);	//getting the comma-delimited symbol IDs

							vector<string> stringIDs;

							LTKStringUtil::tokenizeString(keyValue[1], ",", stringIDs);

							vector<int> ids(stringIDs.size());

							//converting the string symbol IDs to integer symbol IDs
							for(int k=0;k<ids.size();++k)
							{
								ids[k]=atoi(trim(stringIDs[k]).c_str());
								m_idTerminalsMap.insert(make_pair(ids[k],keyValue[0]));	//constructing the symbol ID to terminal map
							}
							
							m_terminalsIDMap.insert(make_pair(keyValue[0],ids));	//constructing the terminals to symbol ID map
							
							//constructing unique list of terminal names
							bool containsTerminal=false;
							for(int a=0;a<terminals.size();++a)
							{
								if(terminals[a]==keyValue[0])
								{
									containsTerminal=true;
									break;
								}

							}
							if(!containsTerminal)
							{
								terminals.push_back(keyValue[0]);
							}

						}
					}

				}

				input.close();
			}
			else
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Grammar file "+grammarFileName+" does not exist!" <<endl;
				throw LTKException(EGRAMMER_FILE_NOT_EXIST);
			}

			bool valuesProvided=false;

			//checking whether list of symbol IDs is given for all the terminals
			for(int s=0;s<symbols.size();++s)
			{
				if(m_cfgMap.find(symbols[s])==m_cfgMap.end())	//if the term is not a production
				{
					valuesProvided=false;
					for(int v=0;v<terminals.size();++v)
					{
						if(terminals[v]==symbols[s])	//value provided for the terminal
						{
							valuesProvided=true;
							break;
						}
					}
					if(!valuesProvided)	//The term is not a production and values (list of symbol IDs) are not provided for it
					{
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Values for the terminal '"+symbols[s]+"' is not provided!" <<endl;
						throw LTKException(EVALUES_NOT_PROVIDED);

					}
				}
			}

			if(!valuesProvided)
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "No productions or terminals identified in the CFG. Please check the CFG format" <<endl;
				throw LTKException(ECONFIG_FILE_FORMAT);
				return;
			}

			findPossiblePaths();	//finds all possible paths for the CFG with the start symbol

			//To determin unique paths
			vector<vector<string> > uniquePaths;
			for(int i=0;i<m_paths.size();++i)
			{
				bool exists=false;
				for(int j=0;j<uniquePaths.size();++j)
				{
					if(uniquePaths[j]==m_paths[i])
					{
						exists=true;
						break;
					}
				}
				if(!exists)
				{
					uniquePaths.push_back(m_paths[i]);
				}
			}

			m_paths=uniquePaths;	//re-assigning unique paths to the paths determined

            isInitialized=true;
          
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::intialize" <<endl;

		}


/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: isValid
* DESCRIPTION	: This function tells whether the given word (vector of symbol IDs) is in compliance with the grammar specified.
*				  Returns false if it doesn't comply. Returns true and a list of possible symbols that could follow if it complies with the grammar
* ARGUMENTS		: word to be checked for the compliance
*				  nextSymbols list of symbol IDs that could follow
* RETURNS		: TRUE/FALSE
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

		bool LTKGrammar::isValid(vector<int>& word, vector<int>& nextSymbols)
		{
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Entering LTKGrammar::isValid" <<endl;   
            
             if(!isInitialized)
             {
                return true;                  
             }


			set<string> nextTerminals;	//holds the list of terminals that could follow the word

			bool valid = getPossibleTerminals(word,nextTerminals);

			if(valid)	//If valid according to the grammar
			{
				
				set<string>::iterator nextTerminalsIter;

				//constructing the list of symbols that could follow the word
				for(nextTerminalsIter=nextTerminals.begin();nextTerminalsIter!=nextTerminals.end();++nextTerminalsIter)
				{

					nextSymbols.insert(nextSymbols.end(),m_terminalsIDMap[*nextTerminalsIter].begin(),m_terminalsIDMap[*nextTerminalsIter].end());
					
				}

				
			}
			
			
			LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::isValid" <<endl;
			return valid;

		}


/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: isValidWord
* DESCRIPTION	: This function tells whether the given word (vector of symbol IDs) is in compliance with the grammar specified.
*				  Returns false if it doesn't comply. Returns true if it complies with the grammar
* ARGUMENTS		: word to be checked for the compliance
* RETURNS		: TRUE/FALSE
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

	bool LTKGrammar::isValidWord(vector<int> word)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKGrammar::isValidWord" <<endl;

             if(!isInitialized)
             {
                return true;                  
             }


		vector<vector<string> >::iterator pathIterator;

		bool returnValue=false;

		
		for(pathIterator=m_paths.begin();pathIterator!=m_paths.end();++pathIterator)
		{
			int nextSymbolIndex;

			bool validPath=doesPathStartWith(*pathIterator,word,nextSymbolIndex);	//checking whether the path starts with the word

			if(validPath)
			{

				vector<int> offsetWord(word.begin()+nextSymbolIndex,word.end());	//offseting the test word

				if(offsetWord.empty())
				{

					if(nextSymbolIndex==(*pathIterator).size())	//the word is complete
					{
						returnValue=true;
					}
					else	//the word does not match the path completely
					{
						returnValue=false;
					}
				}
				else	//if more characters left in the word
				{

					if(nextSymbolIndex==(*pathIterator).size())
					{
						bool check = isValidWord(offsetWord);	//checking the validity for the offset word

						if(check && returnValue!=check)	//even if atleast one of the paths match with the test word, the return value is true
						{
							returnValue=check;
						}
					}
					
				}
				
			}
	
		}

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::isValidWord" <<endl;

		return returnValue;

	}


/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: findPossiblePaths
* DESCRIPTION	: This function finds all possible paths with the given start symbol, productions and terminals
* ARGUMENTS		: 
* RETURNS		: void
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

	void LTKGrammar::findPossiblePaths()
	{

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Entering LTKGrammar::findPossiblePaths" <<endl;

		m_paths=m_cfgMap[m_startSymbol];	//Getting the definition of start symbol production
		
		vector<string> replacedStrings;
		
		for(int i=0;i<m_paths.size();++i)
		{
			for(int j=0;j<m_paths[i].size();++j)
			{
				if(m_cfgMap.find(m_paths[i][j])!=m_cfgMap.end())	//if the term is a production
				{
						if(replacePaths(m_paths[i][j],m_paths,m_cfgMap[m_paths[i][j]]))	//replace in the list of paths wherever the term occurs
						{
							j--;
						}
						else	//if unable to replace the production
						{
							m_paths.clear();
							return;
						}

					if(m_paths[i].size()>10000)	//if the path size has exploded, which could be due to recursive definition
					{
						LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Cyclic dependency exists! Unable to find paths!" <<endl;
						m_paths.clear();
						throw LTKException(ECYCLIC_DEPENDENCY);
						return ;
					}

										
				}
			}


		}

	
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Exiting LTKGrammar::findPossiblePaths" <<endl;
	}



/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: replacePaths
* DESCRIPTION	: This function replaces the production values in terms of terminals for the given production and list of paths so far determined
* ARGUMENTS		: term production to be replaced
*				  originalVector paths determined so far
*				  replaceVector paths corresponding to the production term 
* RETURNS		: TRUE/FALSE whether the term is found or not
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

	bool LTKGrammar::replacePaths(string term,vector<vector<string> >& originalVector,vector<vector<string> >& replaceVector)
	{
		
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Entering LTKGrammar::replacePaths" <<endl;

		bool found=false;	//indicates whether is search term is found

		int iValue=0,jValue=0;	//indices whether the term is found

		bool checkedForRecursion=false;	

		int recursionCount=0;	//indicates how many times the term replacement happens

		while(true)
		{
			++recursionCount;

			if(recursionCount>150)
			{
				LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Cyclic dependency exists! Unable to find paths!" <<endl;
				throw LTKException(ECYCLIC_DEPENDENCY);
				return false;
			}
		
			bool foundTerm=false;

			//searching for the term in the paths determined so far			
			for(int i=iValue;i<originalVector.size();++i)
			{

				for(int j=0;j<originalVector[i].size();++j)
				{
					if(originalVector[i][j]==term)
					{
						iValue=i;
						jValue=j;
						if(!checkedForRecursion)
						{
							checkedForRecursion=true;
							for(int c=0;c<replaceVector.size();++c)
							{
								for(int d=0;d<replaceVector[c].size();++d)
								{
									if(replaceVector[c][d]==term)
									{
										LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Cyclic dependency exists! Unable to find paths!" <<endl;
										throw LTKException(ECYCLIC_DEPENDENCY);
										return false;
									}
								}
							}
						}
						originalVector[i].erase(originalVector[i].begin()+j);
						foundTerm=true;
						break;
					}
				}
				if(foundTerm)
				{
					break;
				}
			}
			if(foundTerm)	//Replacing the found term with its production values
			{
				originalVector.insert(originalVector.begin()+iValue+1,replaceVector.size()-1,*(originalVector.begin()+iValue));
							
				for(int k=iValue;k<(iValue+replaceVector.size());++k)
				{
					originalVector[k].insert(originalVector[k].begin()+jValue,replaceVector[k-iValue].begin(),replaceVector[k-iValue].end());

				}
				
				found=true;

				continue;
			}
			else
			{
				break;
			}
		}

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::replacePaths" <<endl;

		return found;
	}



/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: getPossibleTerminals
* DESCRIPTION	: This function finds the possible terminals that could follow the given word (list of symbol IDs)
*				  Returns true when the test word is in accordance with the grammar and false otherwise.
* ARGUMENTS		: word list of symbol IDs for test
*		          nextTerminals list of terminals returned 
* RETURNS		: TRUE/FALSE
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

	bool LTKGrammar::getPossibleTerminals(vector<int> word,set<string>& nextTerminals)
	{
		
		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKGrammar::getPossibleTerminals" <<endl;
		
		vector<vector<string> >::iterator pathIterator;

		bool returnValue=false;

		
		for(pathIterator=m_paths.begin();pathIterator!=m_paths.end();++pathIterator)
		{
			int nextSymbolIndex;

			bool validPath=doesPathStartWith(*pathIterator,word,nextSymbolIndex);	//checking whether the path starts with the word

			if(validPath)
			{

				vector<int> offsetWord(word.begin()+nextSymbolIndex,word.end());	//offseting the test word

				if(offsetWord.empty())
				{

					if(nextSymbolIndex==(*pathIterator).size())	//the word is complete
					{
						for(int i=0;i<m_paths.size();++i)
						{
							nextTerminals.insert(m_paths[i][0]);
						}

						returnValue=true;
					}
					else	//the word does not match the path completely and it is half way through the path
					{
						nextTerminals.insert((*pathIterator)[nextSymbolIndex]);
						returnValue=true;
					}
				}
				else	//if more characters left in the word
				{

					if(nextSymbolIndex==(*pathIterator).size())	
					{
						bool check = getPossibleTerminals(offsetWord,nextTerminals);	//checking the validity for the offset word

						if(check && returnValue!=check)	//even if atleast one of the paths start with the test word, the return value is true
						{
							returnValue=check;
						}
					}

					
				}
				
			}

		}

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::getPossibleTerminals" <<endl;

		return returnValue;

	}


/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: doesPathStartWith
* DESCRIPTION	: This function finds whether the given path starts with the word given.
*				  Returns true if it starts else false if not
* ARGUMENTS		: path path to test
*		          word list of symbol IDs to test
*				  nextSymbolIndex index of the term in the path that follows the word
* RETURNS		: TRUE/FALSE
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

	bool LTKGrammar::doesPathStartWith(vector<string>& path,vector<int>& word,int& nextSymbolIndex)
	{

		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKGrammar::doesPathStartWith" <<endl;

		bool doesStart=true;

		nextSymbolIndex=0;	//index in the path after the word is compared completely

		
		for(int i=0;i<path.size();++i)
		{
			if(i<word.size())
			{
				if(path[i]==m_idTerminalsMap[word[i]])
				{
					++nextSymbolIndex;
				}
				else	//path doesn't start with the word
				{
					return false;
				}
			}
			else	//path size greater than the word size
			{
				break;
			}
		}

		if(nextSymbolIndex==0 && word.size()!=0)	//if the path doesn't start with the word
		{
			doesStart=false;
			
		}


		LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::doesPathStartWith" <<endl;

		return doesStart;

	}


/**********************************************************************************
* AUTHOR		: Bharath A.
* DATE			: 16-AUG-2005
* NAME			: trim
* DESCRIPTION	: This function trims the leading and trailing spaces of the given word
* ARGUMENTS		: stringValue string to be trimmed
* RETURNS		: trimmed string
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/                     

	string LTKGrammar::trim(string& stringValue)
	{

		  LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKGrammar::trim" <<endl;

		  int firstValidIndex=0,lastValidIndex=stringValue.length();

		  bool firstNonSpaceFound=false,lastNonSpaceFound=false;

		 for(int i=0;i<stringValue.length();++i)
		 {
				 if(!firstNonSpaceFound)
				 {
					 if(stringValue[i]!=' ')	//if the character is not a SPACE
					 {
					  firstNonSpaceFound=true;
					  firstValidIndex=i;                   
					 }
				 }
				 else
				 {
				   if(stringValue[i]==' ')	//if the character is a SPACE
				   {
					   //updating the last valid index
					   if(!lastNonSpaceFound)
					   {                               
							lastValidIndex=i;
					   }

						lastNonSpaceFound=true;                   
				   }      
				   else
				   {
						lastValidIndex=stringValue.length();   
						lastNonSpaceFound=false;
				   }
				 }
          
		 }

		 LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKGrammar::trim" <<endl;

			return stringValue.substr(firstValidIndex,lastValidIndex-firstValidIndex);
      
	}
