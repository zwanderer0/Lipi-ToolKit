
#ifndef __TRAINHMM_H__
#define __TRAINHMM_H__

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#pragma  once
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "LTKMacros.h"
#include "LTKInc.h"
#include "LTKTypes.h"
#include "createhtkdata.h"
#include "htktrain.h"

// List of all possible command line options
#define OPTION_PROJECT "-project"
#define OPTION_PROFILE "-profile"
#define OPTION_FEATURE_FILE "-featurefile"
#define OPTION_VERSION "-ver"
#define OPTION_HELP1 "/?"
#define OPTION_HELP2 "-help"
#define LIPI_ROOT "-lipiroot"
#define DEFAULT_PROFILE "default"
#define MIN_ARGS 2

#define MAX_PROJECT_NAME 128
#define MAX_PROFILE_NAME 128

#ifndef _WIN32
	#define MAX_PATH 1024
#endif





int validateAndPopulateConfigValues(const string& configFilePath);
int processCommandLineArgs();
int getStringValue(int index, char* strOption);
int checkForOption(char* strVal);

#endif //#ifndef __TRAINHMM_H__