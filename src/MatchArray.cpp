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

Match MatchArray::getMinDistance(const string &_queryName)
{
	return matches[_queryName][0];
}
