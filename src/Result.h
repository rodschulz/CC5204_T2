/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <stdlib.h>
#include <vector>
#include <string>
#include "Descriptor.h"

using namespace std;

class Result
{
public:
	// Builds a result object where stored data was extracted using the given frame rate
	Result(const string &_queryName, const int _frameRate);
	~Result();

	// Appends a new matching frame (where matching actually means a most similar frame found)
	void addMatchingFrame(const pair<int, const Descriptor *> &_frame);
	// Returns the star time of the current result
	double getStartTime();
	// Returns the end time of the current result
	double getEndTime();

private:
	// Sequence of matched frames
	vector<pair<int, const Descriptor *>> frameSequence;
	// Frame rate used in the extraction of descriptors
	int frameRate;
	// Name
	string queryName;
};

