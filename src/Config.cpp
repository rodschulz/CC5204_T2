/**
 * Author: rodrigo
 * 2015
 */
#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <math.h>

Config::Config()
{
	descriptorType = HIST;
	param = 64; // Param: # of bins if HIST, # of areas (param x param) if OMD
	metricType = EUCLIDEAN;
	minVideoLength = 3;
	maxJump = 5;
	querySkippedFrames = 1;
	targetFrameRate = 3;
}

Config::~Config()
{
}

void Config::load(const string &_filename)
{
	string line;
	ifstream inputFile;
	inputFile.open(_filename.c_str(), fstream::in);
	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			// Parse string line
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));

			parse(tokens[0], tokens[1]);
		}
		inputFile.close();
	}
	else
		cout << "Unable to open input: " << _filename;
}

void Config::parse(const string _key, const string _value)
{
	if (_key.compare("descriptor") == 0)
	{
		if (_value.compare("OMD") == 0)
			getInstance()->descriptorType = OMD;
		else if (_value.compare("HIST") == 0)
			getInstance()->descriptorType = HIST;
	}
	else if (_key.compare("metric") == 0)
	{
		if (_value.compare("MANHATTAN") == 0)
			getInstance()->metricType = MANHATTAN;
		else if (_value.compare("EUCLIDEAN") == 0)
			getInstance()->metricType = EUCLIDEAN;
		else if (_value.compare("MAX") == 0)
			getInstance()->metricType = MAX;
	}
	else if (_key.compare("minLength") == 0)
		getInstance()->minVideoLength = atof(_value.c_str());
	else if (_key.compare("querySkippedFrames") == 0)
		getInstance()->querySkippedFrames = atoi(_value.c_str());
	else if (_key.compare("targetFrameRate") == 0)
		getInstance()->targetFrameRate = atoi(_value.c_str());
	else if (_key.compare("maxJump") == 0)
		getInstance()->maxJump = atof(_value.c_str());
	else if (_key.compare("param") == 0)
		getInstance()->param = atoi(_value.c_str());
}

DescType Config::getDescriptorType()
{
	return getInstance()->descriptorType;
}

MetricType Config::getMetricType()
{
	return getInstance()->metricType;
}

double Config::getMinVideoLength()
{
	return getInstance()->minVideoLength;
}

int Config::getQuerySkippedFrames()
{
	return getInstance()->querySkippedFrames;
}

int Config::getTargetFrameRate()
{
	return getInstance()->targetFrameRate;
}

int Config::getMaxJump()
{
	return getInstance()->maxJump;
}

int Config::getDescriptorParam()
{
	return getInstance()->param;
}
