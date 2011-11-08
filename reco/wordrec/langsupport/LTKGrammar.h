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
 * FILE DESCR: Definitions for LTKGrammar Module
 *
 * CONTENTS:   
 *
 * AUTHOR:     Bharath A.
 *
 * DATE:       August 16, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/
#ifndef __LTKGRAMMAR_H 
#define __LTKGRAMMAR_H

	#include <string>
	#include <vector>
	#include <list>
	#include <map>
	#include <set>
	#include <iostream>
	#include <fstream>

	#include "LTKStringUtil.h"
	#include "LTKException.h"
	#include "LTKLoggerUtil.h"
	#include "LTKErrors.h"
	#include "LTKErrorsList.h"


	using namespace std;

/**
 * @class LTKGrammar This class determines whether the given word is in accordance with the grammar specified
 *		<p> This modules requires the rules of the Grammar to be specified as a CFG (Context-free grammar) and
 *			a map to be provided for the terminals in the CFG and the symbol IDs</p>
 */

	class LTKGrammar
	{
     
	private:
	map<string,vector<vector<string> > > m_cfgMap;	//Holds the CFG specified. Key-Production Value-Production values
	map<string,vector<int> > m_terminalsIDMap;		//Holds each terminal as key and the corresponding list of symbol IDs as value
	vector<vector<string> > m_paths;				//Holds the possible paths that contain only terminals. It is decoded from the CFG
	string m_startSymbol;							//Holds the start symbol of the CFG. The first production in the CFG is  considered as the start symbol
	map<int,string> m_idTerminalsMap;				//Holds symbol ID and terminal name mapping
	bool isInitialized;

	public:

		/**
         *	@name Constructor and Destructor
		 */
  		//@{

		LTKGrammar();

		~LTKGrammar();

		//@}

		/**
		* This function sets the grammar file, reads the CFG, populates the data structures(maps) and determines possible paths
		* @param grammarFileName rules file name that has grammar specified as CFG and terminal-symbol mapping
		* @return void
		*/
		void initialize(string grammarFileName);

		/**
		* This function tells whether the given word (vector of symbol IDs) is in compliance with the grammar specified.
		* Returns false if it doesn't comply. Returns true and a list of possible symbols that could follow if it complies with the grammar
		* @param word to be checked for the compliance
		* @param nextSymbols list of symbol IDs that could follow
		* @return bool
		*/
		bool isValid(vector<int>& word, vector<int>& nextSymbols);



		/**
		* This function tells whether the given word (vector of symbol IDs) is in compliance with the grammar specified.
		* Returns false if it doesn't comply. Returns true if it complies with the grammar
		* @param word to be checked for the compliance
		* @return bool
		*/
		bool isValidWord(vector<int> word);


	private:
		
	
		/**
		* This function finds all possible paths with the given start symbol, productions and terminals
		* @return void
		*/
		void findPossiblePaths();
		
		

		/*
		* This function replaces the production values in terms of terminals for the given production and list of paths so far determined
		* @param term production to be replaced
		* @param originalVector paths determined so far
		* @param replaceVector paths corresponding to the production term
		* @return bool whether the term is found or not
		*/
		bool replacePaths(string term,vector<vector<string> >& originalVector,vector<vector<string> >& replaceVector);

		/*
		* This function finds the possible terminals that could follow the given word (list of symbol IDs)
		* Returns true when the test word is in accordance with the grammar and false otherwise.
		* @param word list of symbol IDs for test
		* @param nextTerminals list of terminals returned 
		* @return bool
		*/
		bool getPossibleTerminals(vector<int> word,set<string>& nextTerminals);
		
		/*
		* This function finds whether the given path starts with the word given.
		* Returns true if it starts else false if not
		* @param path path to test
		* @param word list of symbol IDs to test
		* @param nextSymbolIndex index of the term in the path that follows the word
		* @return bool
		*/
		bool doesPathStartWith(vector<string>& path,vector<int>& word,int& nextSymbolIndex);

		/*
		* This function trims the leading and trailing spaces of the given word
		* @param stringValue string to be trimmed
		* @return string after trimming
		*/

		string trim(string& stringValue);


        
	};


	#endif // #ifndef __LTKGRAMMAR_H
