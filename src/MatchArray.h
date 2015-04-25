/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Descriptor.h"

using namespace std;

// Forward declarations
class MatchArray;
struct Match;

// Shared pointers declarations
typedef shared_ptr<MatchArray> MatchArrayPtr;
typedef shared_ptr<Match> MatchPtr;

// Structure holding pairs of matching descriptors
struct Match
{
	double distance;
	DescriptorPtr query;
	DescriptorPtr target;

	Match()
	{
		distance = -1;
		query = NULL;
		target = NULL;
	}

	Match(const double _distance, const DescriptorPtr &_query, const DescriptorPtr &_target)
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
	// Returns the minimum distance for the given query
	Match getQueryMinDistance(const string &_queryName);
	// Returns the minimum distance in the current array
	double getMinDistance();
	// Returns the maximum distance in the current array
	double getMaxDistance();

private:
	int targetVideoFrame;
	map<string, vector<Match>> matches;
};

