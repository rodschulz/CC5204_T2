/**
 * Author: rodrigo
 * 2015
 */
#include "MatchArray.h"

MatchArray::MatchArray(const int &_frameNumber)
{
	targetVideoFrame = _frameNumber;
}

MatchArray::~MatchArray()
{
}

void MatchArray::addMatch(const string &_queryName, const vector<Match> &_matchingFrames)
{
	matches[_queryName] = _matchingFrames;
}

Match MatchArray::getQueryMinDistance(const string &_queryName)
{
	return matches[_queryName][0];
}

double MatchArray::getMinDistance()
{
	double minDist = numeric_limits<double>::max();
	for (pair<string, vector<Match>> entry : matches)
		minDist = minDist > entry.second[0].distance ? entry.second[0].distance : minDist;

	return minDist;
}

double MatchArray::getMaxDistance()
{
	double maxDist = -numeric_limits<double>::max();
	for (pair<string, vector<Match>> entry : matches)
		maxDist = maxDist < entry.second[0].distance ? entry.second[0].distance : maxDist;

	return maxDist;
}
