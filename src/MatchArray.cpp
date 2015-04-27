/**
 * Author: rodrigo
 * 2015
 */
#include "MatchArray.h"

MatchArray::MatchArray(const int &_frameNumber)
{
	targetVideoFrame = _frameNumber;
	matches.clear();
	minDistanceQuery = maxDistanceQuery = "";
	minDistanceMatch = Match();
	maxDistanceMatch = Match();
}

MatchArray::~MatchArray()
{
}

void MatchArray::addMatch(const string &_queryName, const vector<Match> &_matchingFrames)
{
	matches[_queryName] = _matchingFrames;
	minDistanceMatch = Match();
}

Match MatchArray::getQueryMinDistance(const string &_queryName)
{
	return matches[_queryName][0];
}

double MatchArray::getMinDistance()
{
	if (abs(minDistanceMatch.distance + 1) < 1E-10)
		findMinDistanceMatch();

	return minDistanceMatch.distance;
}

double MatchArray::getMaxDistance()
{
	if (abs(maxDistanceMatch.distance + 1) < 1E-10)
		findMaxDistanceMatch();

	return maxDistanceMatch.distance;
}

string MatchArray::getMinDistanceQuery()
{
	if (abs(minDistanceMatch.distance + 1) < 1E-10)
		findMinDistanceMatch();

	return minDistanceQuery;
}

Match MatchArray::getMinDistanceMatch() const
{
	return minDistanceMatch;
}

void MatchArray::findMinDistanceMatch()
{
	double minDist = numeric_limits<double>::max();
	for (pair<string, vector<Match>> entry : matches)
	{
		if (minDist > entry.second[0].distance)
		{
			minDist = entry.second[0].distance;
			minDistanceMatch = entry.second[0];
			minDistanceQuery = entry.first;
		}
	}
}

void MatchArray::findMaxDistanceMatch()
{
	double maxDist = numeric_limits<double>::max();
	for (pair<string, vector<Match>> entry : matches)
	{
		if (maxDist < entry.second.back().distance)
		{
			maxDist = entry.second.back().distance;
			maxDistanceMatch = entry.second.back();
			maxDistanceQuery = entry.first;
		}
	}
}

void MatchArray::swapMinDistanceQuery(const string &_query)
{
	minDistanceMatch = matches[_query][0];
	minDistanceQuery = _query;
}
