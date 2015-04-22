/**
 * Author: rodrigo
 * 2015
 */
#include "Result.h"

Result::Result(const string &_queryName, const int _frameRate)
{
	queryName = _queryName;
	frameRate = _frameRate;
}

Result::~Result()
{
}

void Result::addMatchingFrame(const pair<int, const Descriptor *> &_frame)
{
	frameSequence.push_back(_frame);
}

double Result::getStartTime()
{
	double time = frameSequence.front().first;
	time /= frameRate;
	return time;
}

double Result::getEndTime()
{
	double time = frameSequence.back().first;
	time /= frameRate;
	return time;
}
