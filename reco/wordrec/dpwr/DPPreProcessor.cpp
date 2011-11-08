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
 * FILE DESCR: Definition of DPPreProcessor contains the specific pre-processing
 *			   techniques like 'shiro-rekha' detection
 *
 * CONTENTS: 
 *	detectShiroRekha
 *
 * AUTHOR:     Mudit Agrawal.
 *
 * DATE:       Mar 10, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of 
 ************************************************************************/

#include "DPPreProcessor.h"

/**********************************************************************************
* AUTHOR		: Mudit Agrawal
* DATE			: 10-MAR-2005
* NAME			: DPPreProcessor
* DESCRIPTION	: DEFAULT CONSTRUCTOR
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/


DPPreProcessor::DPPreProcessor()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered default constructor of DPPreProcessor" );

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting default constructor of DPPreProcessor" );
}

/**********************************************************************************
* AUTHOR		: Mudit Agrawal
* DATE			: 10-MAR-2005
* NAME			: detectShiroRekha
* DESCRIPTION	: detects the trace(s) which are shiro-rekha(s) among the tracegroup
* ARGUMENTS		: traceGroup - group of traces which may contains shiro-rekha(s)
*				  shiroRekhaIndices - vector of indices specifying shiro-rekha(s) withing the traceGroup.
* RETURNS		: SUCCESS on successful reads
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

int DPPreProcessor::detectShiroRekha(const LTKTraceGroup &traceGroup, vector<int> & shiroRekhaIndices)
{

	int retValue;
	const LTKTraceVector &traceVector = traceGroup.getAllTraces();		//TraceVector containing whole traceGroup
	retValue = detectShiroRekha(traceVector,shiroRekhaIndices);
    return retValue;											//returning success


}

int DPPreProcessor::detectShiroRekha(const LTKTraceVector &traceVector, vector<int> & shiroRekhaIndices)
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered DPPreProcessor::detectShiroRekha" );

	int numOfTraces=traceVector.size();	//number of Traces in the traceGroup

	
	vector<float> slopes(numOfTraces);
	vector<float> widths(numOfTraces);
	vector<float> meanHeights(numOfTraces);
	vector<float> error(numOfTraces);
	vector<float> intercepts(numOfTraces);

	int indexOfMinRank;						//index of the trace in the traceGroup with minimum Rank
	int minRank;							//value of the minimum Rank
	int rank;								//rank of a trace in the traceGroup
	int numChannels;						//number of channels in the trace 

	for(int traceIndex = 0; traceIndex < numOfTraces; traceIndex++)		//interating through all the traces in the traceGroup
	{
		numChannels = (traceVector.at(traceIndex)).getNumChannels();
		if(numChannels>0)
		{	
			int numOfPoints = traceVector[traceIndex].getNumberOfPoints();	//storing number of Points in the trace

			float x;														//abscissa of the trace
			float y;														//ordinate of the trace
			float sigmaX = 0.0;												//sum of all abscissa of a trace
			float sigmaY = 0.0;												//sum of all ordinates of a trace
			float sigmaXX = 0.0;											//sum of squares of all abscissa of a trace
			float sigmaYY = 0.0;											//sum of squares of all ordinates of a trace
			float sigmaXY = 0.0;											//sum of product of abscissa and ordinate of a trace

			vector<float> maxValueOfTrace;									//vector containing minimum values of channels of a trace
			vector<float> minValueOfTrace;									//vector containing maximum values of channels of a trace
			vector<string> channelName(1,"X");								//vector containing the channel Names of the trace

			const vector<float>& abscissaOfTrace = traceVector[traceIndex].getChannelValues("X");		//vector containing "X" channel from the trace
			const vector<float>& ordinateOfTrace = traceVector[traceIndex].getChannelValues("Y");		//vector containing "Y" channel from the trace

			for(int pointIndex = 0; pointIndex < numOfPoints; pointIndex++)	//iterating through all the points in a trace
			{
				x = abscissaOfTrace[pointIndex];							//abscissa of a point in the trace
				y = ordinateOfTrace[pointIndex];							//ordinate of a point in the trace
				sigmaX += x;												//sum of all abscissa of a trace
				sigmaY += y;												//sum of all ordinates of a trace
				sigmaXX += x*x;												//sum of squares of all abscissa of a trace
				sigmaYY += y*y;												//sum of squares of all ordinates of a trace
				sigmaXY += x*y;												//sum of product of abscissa and ordinate of a trace
			}

			if((numOfPoints*sigmaXX - (sigmaX*sigmaX)) != 0)
			{
				slopes[traceIndex] = (float)(numOfPoints*sigmaXY - sigmaX*sigmaY)/(1.0*(numOfPoints*sigmaXX - (sigmaX*sigmaX)));		//calculating the slope of the regression line 
			}
			else
			{
				slopes[traceIndex] = FLT_MAX;								//if trace is perpendicular to Y-axis, slope = infinity
			}
			intercepts[traceIndex] = (sigmaY - slopes[traceIndex]*sigmaX)/numOfPoints;	//calculating the intercept of the regression line
			error[traceIndex] = sigmaYY - slopes[traceIndex]*sigmaXY - intercepts[traceIndex]*sigmaY;		//error of the trace from the fitted regression line
			meanHeights[traceIndex] = sigmaY/numOfPoints;					//height of the trace from the origin (assumed top-left)
			LTKInkUtils::computeChannelMaxMin(traceVector[traceIndex], channelName, maxValueOfTrace, minValueOfTrace);  //calculating min and max values of x-dimensions of the trace
			widths[traceIndex] = maxValueOfTrace[0] - minValueOfTrace[0];	//width of the trace

		}
		else
		{
			slopes[traceIndex]      =  FLT_MAX;
			error[traceIndex]       =  FLT_MAX;
			widths[traceIndex]      = -FLT_MAX;
			meanHeights[traceIndex] =  FLT_MAX;

		}
		/*cout <<"traceIndex = "<<traceIndex<<endl;
		cout <<"slope "<<slopes[traceIndex]<<"error "<<error[traceIndex]<<"width "
			<<widths[traceIndex]<<"meanHeights "<<meanHeights[traceIndex]<<endl;*/
	}

	//BORDA COUNT method to rank traces on the basis of slopes, widths, error and meanHeights
	//Lesser the rank, more optimal for selection as shiro-rekha

	minRank = 5 * numOfTraces;
	indexOfMinRank = -1;
	
	for(int traceIndex1 = 0; traceIndex1 < numOfTraces; traceIndex1++)
	{
		rank = 0;
		for(int traceIndex2 = 0; traceIndex2 < numOfTraces; traceIndex2++)
		{
	         if(fabs(slopes[traceIndex1]) > fabs(slopes[traceIndex2]))
				 rank++;									//absolute slope of shiro-rekha should be minimum
             if(widths[traceIndex1]  < widths[traceIndex2]) 
				 rank += 2;									//width is given double weight: width of shiro-rekha should be maximum
             if(error[traceIndex1]  > error[traceIndex2]) 
				 rank++;									//error of shiro-rekha from fitted regression line should be minimum
             if(meanHeights[traceIndex1] > meanHeights[traceIndex2]) 
				 rank++;									//height of shiro-rekha from top of bounding box (or y-axis) should be minimum

		}
		if(rank < minRank)
		{
			minRank = rank;
			indexOfMinRank = traceIndex1;					//updating the minimum ranked traceIndex
		}

	}

	 //<TODO>: Multiple shiroRekha detection

	shiroRekhaIndices.push_back(indexOfMinRank);			//pushing the index of traces with minimum rank(s) in the vector

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG, "Existing DPPreProcessor::detectShiroRekha" );

    return SUCCESS;											//returning success


}


/**********************************************************************************
* AUTHOR		: Mudit Agrawal
* DATE			: 03-MAR-2005
* NAME			: DPPreProcessor
* DESCRIPTION	: DEFAULT DESTRUCTOR
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of 
*************************************************************************************/

DPPreProcessor::~DPPreProcessor()
{
	(LTKLogger::LTK_LOGLEVEL_DEBUG, "Entered destructor of DPPreProcessor" );

	(LTKLogger::LTK_LOGLEVEL_DEBUG, "Exiting destructor of DPPreProcessor" );
}
