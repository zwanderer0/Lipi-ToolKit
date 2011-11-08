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
 * FILE DESCR: Implementation of LTKWordPreprocessor that performs word 
 *             level normalization
 *
 * CONTENTS: 
 *   normalizeWordSize 
 *
 * AUTHOR:     Deepu V.
 *
 * DATE:       March 1, 2005
 * CHANGE HISTORY:
 * Author		Date			Description of change
 ************************************************************************/

#include "LTKWordPreprocessor.h"
#include "LTKLoggerUtil.h"

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 01-MAR-2005
* NAME			: LTKWordPreprocessor
* DESCRIPTION	: Initialization constructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

LTKWordPreprocessor::LTKWordPreprocessor()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered LTKWordPreprocessor::LTKWordPreprocessor" <<endl;


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKWordPreprocessor::LTKWordPreprocessor" <<endl;

}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 01-MAR-2005
* NAME			: normalizeWordSize
* DESCRIPTION	: this function finds an initial estimate for 
*               : mid-line.  which is the best fit horizontal 
*               : line for all points in the trace.  
*               : 
*               : The second stage  refines the initial 
*               : estimate by fitting a line through all 
*               : the stroke midpoints that are intercepted 
*               : by the initially estimated line. 
* ARGUMENTS		: inTraces - input traces
*                 preProcessedTraces - output preprocessed traces
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
* Deepu V           04-MAR-05           changed these function to take vector of traces
*                                       than to take LTKTraceGroups
*************************************************************************************/

int LTKWordPreprocessor::normalizeWordSize(const vector<LTKTrace>& inTraces, vector<LTKTrace>& preProcessedTraces)
{
	const LTKTraceVector& allTraces = inTraces;	//Getting the traces vector
	
	int numTraces = allTraces.size(); //number of traces

	int totalPoints =0 ;              //total points in the TraceGroups

	int numPoints = 0;                //number of points in a stroke;

	float yMean =0;                   //variable to find yMean in initial estimate

	intVector pointsPerStroke;        //points per each stroke

	floatVector xMidPoints, yMidPoints; // stores the mid points of each stroke

	floatVector yMaxPoints, yMinPoints; //stores  ymax and ymin for each stroke

	float Sxx, Sxy, Sx, Sy, Su, Sl;   //for the second stage Sxx = sum( x * x) 
	
	float Sxw, Syw, Suw, Slw;  // for similar partial sums.

	float covXY, varX;         // cov and var of midpoits 

	float slope, cmid, ctop, cbot; //parameters for the lines

	int i, j;                  //index variables
	
	int numStrkIntrcpt ;       // number of strokes that intercepts with the initial estimate

	LTKTrace trace;            //temp variable for making a trace.

	float scale ;               //scale value of the word

	float ySum = 0;  //sum of all Y points in this trace

	float xMin, yMin, xMax, yMax; // (x and y ) (max and min) of each stroke

	float x, y;   // x and y values of the stroke
	//finding initial estimate of mid line
	//model of line is y = mx + c 
	//m = 0; c = mean(y)
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered LTKWordPreprocessor::normalizeWordSize" <<endl;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"Exiting LTKWordPreprocessor::normalizeWordSize" <<endl;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "numTraces = "<< numTraces <<endl;

	for(i=0; i<numTraces; ++i)
	{
		if(allTraces[i].getTraceFormat().getNumChannels() >0)
		{
			ySum = 0;

			//retrieving x and y channel values

			floatVector yChannel, xChannel;
			
			allTraces.at(i).getChannelValues("Y", yChannel);

			allTraces.at(i).getChannelValues("X", xChannel);

			numPoints = yChannel.size(); //finding the number of points
 
			totalPoints += numPoints;  //updating the total number of points

			if( numPoints >0 )
			{
				xMax = xMin = xChannel.at(0);

				yMax = yMin = yChannel.at(0);

				//iterating through each point in the trace
				for (j =0; j<numPoints; ++j)
				{
					x = xChannel[j];
					y = yChannel[j];

					ySum += y;  //accumulating for estimating c

					//tracking x and y min and max
					if(xMin > x)
						xMin = x;
 
					if(yMin >y)
						yMin = y;
					
					if(xMax <x)
						xMax = x;

					if(yMax <y)
						yMax = y;
				}
				
				yMean += ySum; //accumulating sum in yMean for estimating c

				//Saving the mid points of strokes for 
				//second stage (refining stage)
				xMidPoints.push_back( (xMin + xMax)/2.0 );

				yMidPoints.push_back( (yMin + yMax)/2.0 );

				yMaxPoints.push_back( yMax );

				yMinPoints.push_back( yMin );

				pointsPerStroke.push_back (numPoints);
			}

		}
	}

	//finding the mean 
	yMean /= totalPoints;

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Initial Model :  y = " << yMean <<endl;
	//initial model is now y = yMean;

	/*****************second stage ******************************
	*   REFINING MID LINE + FITTING TOP AND BOTTOM REF LINES
	*Line is fit only through mid points
	* m = cov(x,y) / var(x)   
    *     # x and y are midpoints of strokes that are intercepted
	* cmid [mid line] = (Syw - m*Sxw)/Sw
	*     # weighted reg line fit of mid points (xmid, ymid)
	* cu   [top line] = (Suw - m*Sxw)/Sw 
	*     # weighted reg line fit of top points (xmid, ymax) 
	* cl   [bot line] = Slw - m*Sxw)/Sw  
	*     # weighted reg line fit of bot points (xmid, ymin)
	*/

	//initializing sums
	Sxx = Sxy = Sx = Sy = Su = Sl =0;  
	Sxw = Syw = Suw = Slw = 0; 
	numStrkIntrcpt = 0;
	totalPoints = 0;

	//going through each stroke again
	numTraces = xMidPoints.size();
	for(i =0; i<numTraces; ++i)
	{
		float x,y,u,l,w;

		x = xMidPoints.at(i);
		y = yMidPoints.at(i);
		w = pointsPerStroke.at(i);
		u = yMaxPoints.at(i);
		l = yMinPoints.at(i);

		Su += u; //for a coarse estimation in case of no overlapping strokes 
		Sl += l; 

		if( (u>yMean) && (l<yMean) ) //checking if the stroke overlaps
		{
			++numStrkIntrcpt;

			totalPoints += w;
			Sxx += (x*x);
			Sxy += (x*y);
			Sx  += x;
			Sy  += y;
			Sxw += (x*w);
			Syw += (y*w);
			Suw += (u*w);
			Slw += (l*w);
		}
		
	}
	//finding the estimates
	varX = (numStrkIntrcpt > 0)?(Sxx - Sx*Sx/totalPoints):EPS;
	covXY = (numStrkIntrcpt > 0)?(Sxy - Sx*Sy/totalPoints):EPS;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Cov(X,Y) = " <<covXY <<endl;
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<"VarX = "<<varX<<endl;


	//<TODO> generally it is okay what abt spl cases
	slope = ((varX - EPS) > EPS)?( covXY/varX): 0;

	//estimating modified estimates
	cmid = (totalPoints > 0 )?(Syw - slope * Sxw)/totalPoints:yMean;
	ctop = (totalPoints > 0 )?(Suw - slope * Sxw)/totalPoints:Su/numTraces;
	cbot = (totalPoints > 0 )?(Slw - slope * Sxw)/totalPoints:Sl/numTraces;
	

	//now the reference lines are 
	//     mid line - y = slope*x + cmid
	// top ref line - y = slope*x + ctop
	// bot ref line - y = slope*x + cbot

    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Refined Models " <<endl;


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<"mid line : y = "<<slope<<"*x + "<<cmid<<endl;


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<"top line : y = "<<slope<<"*x + "<<ctop<<endl;


	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG)<<"bot line : y = "<<slope<<"*x + "<<cbot<<endl;


	//Normalizing the character
	//This involves correcting the shear (modeled as slope)
	//as well as normalizing the scale ( vertical size is normalized to unity)

	
    scale = fabs(ctop - cbot);

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "scale = " << scale <<endl;

	//creating the normalized character
	//normalizing the estimated reference
	//lines to y=0 and y=1

	numTraces = allTraces.size();

	for (i =0; i<numTraces; ++i)
	{

		trace = allTraces.at(i);  //DONT CHANGE TRACE TO A REFERENCE 

		if(trace.getTraceFormat().getNumChannels() > 0)
		{

			//Getting the channel values
			floatVector xChannelVals, yChannelVals;
				
			trace.getChannelValues("X", xChannelVals); // retrieving X values

			trace.getChannelValues("Y", yChannelVals); // retrieving Y values

			numPoints = xChannelVals.size();

			for(j =0; j<numPoints; ++j)
			{
				float x,y,sub;

				x = xChannelVals[j];
				y = yChannelVals[j];

				//Normalizing
				sub = slope * x + cbot;
				y = (y - sub)/scale;
				x /= scale;

				xChannelVals[j] = x;
				yChannelVals[j] = y;
			}
			
			trace.setChannelValues(string("X"),xChannelVals);

			trace.setChannelValues(string("Y"),yChannelVals);
		}

		//Creating a new set of normalized traces
		preProcessedTraces.push_back(trace);

		LOG(LTKLogger::LTK_LOGLEVEL_INFO) << "PreProcessed stroke:" << i <<endl;
	}

	return SUCCESS; 
}


/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 01-MAR-2005
* NAME			: normalizeWordSize
* DESCRIPTION	: Normalizes a LTKTraceGroup that corresponds to a word
*               : 
* ARGUMENTS		: inInk
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int LTKWordPreprocessor::normalizeWordSize(const LTKTraceGroup& inInk, LTKTraceGroup& preProcessedInk)
{
     const LTKTraceVector& allTraces = inInk.getAllTraces();	//Getting the traces vector
	
	 LTKTraceVector normalizedTraces; //the pre procesed traces;


	 LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKWordPreprocessor::normalizeWordSize" <<endl;
	 //normalizing the word
	 normalizeWordSize(allTraces,normalizedTraces);

	//assigning to new ink traces.
	preProcessedInk.setAllTraces(normalizedTraces);

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKWordPreprocessor::normalizeWordSize" <<endl;

	return SUCCESS; 
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 01-MAR-2005
* NAME			: normalizeWordSize
* DESCRIPTION	: Normalizes a LTKTraceGroup that corresponds to a word
*               : Assumes a UI with two horiztonal lines  
* ARGUMENTS		: inInk
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int LTKWordPreprocessor::normalizeWordSize(const vector<LTKTrace>& inTraces, const LTKScreenContext& sc ,vector<LTKTrace>& preProcessedTraces, int startTraceIndex, int endTraceIndex)
{
	
	vector<LTKTrace>::const_iterator traceIter,traceStart, traceEnd; //iterator for the traces

	int numTraces;

	LTKTrace newTrace;   //pre processed trace

	int numPoints;       //number of points in  a trace

	float topRefLine, botRefLine;  // Top and bottom reference lines in the UI

	int pointIndex;       //index to iterate through the trace

	float scale ;


    LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKWordPreprocessor::normalizeWordSize" <<endl;

	numTraces = inTraces.size();

	if(endTraceIndex == -1)
	{
		endTraceIndex = numTraces;
	}

	//validating the starting stroke index
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Start Index="<<startTraceIndex<<" End Index="<<endTraceIndex<<endl;

	if((startTraceIndex <0) || (startTraceIndex > numTraces ) )
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Invalid stroke start index" <<endl;
		return FAILURE;
	}

	//validating the ending stroke index
	if((endTraceIndex < startTraceIndex)||(endTraceIndex> numTraces))
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Invlaid stroke end index"<<endl;
		return FAILURE;
	}

	const floatVector& horizLines = sc.getAllVLines(); //The horizontal lines in the UI

	//There should be two horizontal lines
	if (horizLines.size() != 2)
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "The number of horizontal lines in screen context need to be two" <<endl;
		return FAILURE;
	}

	topRefLine = horizLines.at(0);
	botRefLine = horizLines.at(1);

	//it is badly scaled if the top Line and bottom line are "too close:
	if( (topRefLine - botRefLine) <EPS )
	{
		LOG(LTKLogger::LTK_LOGLEVEL_ERR) << "Top Reference line equals bottom reference line" <<endl;
		return FAILURE;
	}


	//interchange if they are the other way around
	if(topRefLine > botRefLine)
	{
		float temp = topRefLine;
		topRefLine = botRefLine;
		botRefLine = temp;
	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) <<" Top Ref Line="<<topRefLine<<" Bot Ref Line="<<botRefLine<<endl;

	//scale value
	scale = botRefLine - topRefLine;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << " scale = "<<scale<<endl;

	traceStart =  inTraces.begin() + startTraceIndex; 
	traceEnd = traceStart + endTraceIndex;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << " Rescaling traces " <<endl;
	
	//iterating through the traces
	for(traceIter = traceStart; traceIter != traceEnd; ++traceIter)
	{
		newTrace = *traceIter;  

		//Do if it is not a marker stroke
		if(newTrace.getTraceFormat().getNumChannels() > 0)
		{

			floatVector xChannelVals, yChannelVals;
			
			newTrace.getChannelValues("X", xChannelVals); // retrieving X values
			newTrace.getChannelValues("Y", yChannelVals); // retrieving Y values

			numPoints = newTrace.getNumberOfPoints();

			for (pointIndex =0; pointIndex < numPoints; ++pointIndex)
			{
				//rescaling 
				xChannelVals[pointIndex] /= scale;
				yChannelVals[pointIndex] = (yChannelVals[pointIndex] - topRefLine)/scale;
			}

			newTrace.setChannelValues(string("X"),xChannelVals);
			newTrace.setChannelValues(string("Y"),yChannelVals);
		}

		//saving the pre processed strokes
		preProcessedTraces.push_back(newTrace);

	}

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKWordPreprocessor::normalizeWordSize" <<endl;

	return SUCCESS;
}

/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 01-MAR-2005
* NAME			: normalizeWordSize
* DESCRIPTION	: Normalizes a LTKTraceGroup that corresponds to a word
*               : 
* ARGUMENTS		: inInk
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

int LTKWordPreprocessor::normalizeWordSize(const LTKTraceGroup& inInk, const LTKScreenContext& sc, LTKTraceGroup& preProcessedInk)
{
     const LTKTraceVector& allTraces = inInk.getAllTraces();	//Getting the traces vector
	
	 LTKTraceVector normalizedTraces; //the pre procesed traces;

	 LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entering LTKWordPreprocessor::normalizeWordSize"<<endl;
	 //normalizing the word
	 normalizeWordSize(allTraces,sc,normalizedTraces);

	//assigning to new ink traces.
	preProcessedInk.setAllTraces(normalizedTraces);

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKWordPreprocessor::normalizeWordSize" <<endl;

	return SUCCESS; 
}


/**********************************************************************************
* AUTHOR		: Deepu V.
* DATE			: 01-MAR-2005
* NAME			: ~LTKWordPreprocessor
* DESCRIPTION	: destructor
* ARGUMENTS		: 
* RETURNS		: 
* NOTES			:
* CHANGE HISTROY
* Author			Date				Description of change
*************************************************************************************/

LTKWordPreprocessor::~LTKWordPreprocessor()
{
	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Entered LTKWordPreprocessor::LTKWordPreprocessor" <<endl;

	LOG(LTKLogger::LTK_LOGLEVEL_DEBUG) << "Exiting LTKWordPreprocessor::LTKWordPreprocessor" <<endl;
}
