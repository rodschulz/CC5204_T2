/**
 * Author: rodrigo
 * 2015
 */
#include "Match.h"

Match::Match(const int &_frameNumber)
{
	targetVideoFrame = _frameNumber;
}

Match::~Match()
{
}

void Match::addMatch(const string &_queryName, const vector<Descriptor> &_matchingFrames)
{
	matches[_queryName] = _matchingFrames;
}

//double Match::getMinDistance(const string &_queryName)
//{
//	return matches[_queryName][0];
//}
