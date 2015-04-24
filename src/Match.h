/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include "Descriptor.h"

using namespace std;

class Match
{
public:
	Match(const int &_frameNumber);
	~Match();

	void addMatch(const string &_queryName, const vector<Descriptor> &_matchingFrames);
	double getMinDistance(const string &_queryName);

private:
	int targetVideoFrame;
	map<string, vector<Descriptor>> matches;
};

