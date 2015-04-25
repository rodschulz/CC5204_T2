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

// Structure holding pairs of matching descriptors
struct Match
{
	double distance;
	Descriptor query;
	Descriptor target;

	Match()
	{
		distance = -1;
		query = Descriptor();
		target = Descriptor();
	}

	Match(const double _distance, const Descriptor &_query, const Descriptor &_target)
	{
		distance = _distance;
		query = _query;
		target = _target;
	}
};

class MatchArray
{
public:
	MatchArray(const int &_frameNumber);
	~MatchArray();

	void addMatch(const string &_queryName, const vector<Match> &_matchingFrames);
	Match getMinDistance(const string &_queryName);

private:
	int targetVideoFrame;
	map<string, vector<Match>> matches;
};

