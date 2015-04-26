/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include <string>
#include "Descriptor.h"
#include "Metric.h"
#include "MatchArray.h"
#include <opencv2/highgui/highgui.hpp>

using namespace std;

class Helper
{
public:
	/** Location of the video to be searched according to the input file */
	static string getTargetLocation(const string &_inputFile);
	/** Location of the video to be searched according to the input file */
	static vector<string> getQueryLocations(const string &_inputFile);
	/** Returns a random number between the given limits */
	static int getRandomNumber(const int _min, const int _max);
	/** Find the nearest frame in the given set of frames */
	static void findNearestFrames(const DescriptorPtr &_targetFrame, const vector<DescriptorPtr> &_queryFrames, const MetricType &_metric, vector<Match> &_output);
	/** Returns the number of frames to be skipped to reach the desired sampling rate */
	static int getSkipFrames(const int _samplingRate, VideoCapture &_capture);
	/** Returns the median of the given vector of data */
	static double getMedian(const vector<double> &_data);
	/** Prints the given array to a text file */
	static void printToFile(const string &_outputFile, const vector<double> &_data);

private:
	Helper();
	~Helper();

	static bool compare(const Match &_m1, const Match &_m2);
};
