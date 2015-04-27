/**
 * Author: rodrigo
 * 2015
 */
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Descriptor.h"
#include "Helper.h"
#include "MatchArray.h"
#include "Metric.h"
#include "Config.h"

using namespace std;
using namespace chrono;
using namespace cv;

bool getVideoDescriptor(const string &_videoLocation, vector<DescriptorPtr> &_outputDescriptor, const DescType &_type, const int &_skipFrames, const int param)
{
	bool statusOk = true;

	if (_skipFrames < 0)
	{
		cout << "ERROR: number of frames to skip\n";
		statusOk = false;
	}
	else
	{
		VideoCapture capture;
		capture.open(_videoLocation);
		if (!capture.isOpened())
		{
			cout << "ERROR: Can't open target video " << _videoLocation << endl;
			statusOk = false;
		}
		else
		{
			_outputDescriptor.clear();
			double totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);

			cout << "Total frames in query: " << totalFrames << "\n";

			Mat frame, grayFrame;
			int k = 0;
			for (int j = 0; j < totalFrames; j++)
			{
				if (!capture.grab() || !capture.retrieve(frame))
				{
					cout << "ERROR: Can't get new frame. Finishing loop.\n";
					statusOk = false;
					break;
				}

				if (k == _skipFrames)
				{
					cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
					_outputDescriptor.push_back(DescriptorPtr(new Descriptor(grayFrame, j, _type, param)));

					k = -1;
				}
				k++;
			}
		}
	}

	return statusOk;
}

void getQueryDescriptors(map<string, vector<DescriptorPtr>> &_queryDescriptors, const vector<string> &_queryLocations, const DescType &_descriptorType, const int _skipFrames, const int param)
{
	for (String query : _queryLocations)
	{
		cout << "Processing query: " << query << "\n";

		_queryDescriptors[query] = vector<DescriptorPtr>();
		if (!getVideoDescriptor(query, _queryDescriptors[query], _descriptorType, _skipFrames, param))
			_queryDescriptors.erase(query);

		cout << "Query video descriptor length: " << _queryDescriptors[query].size() << "\n";
	}
}

map<string, vector<Appearance>> extractQueryAppearanceTimes(vector<MatchArrayPtr> &_matchesPerFrame, const double _targetFrameRate, const double _discardThreshold, const double _minLength)
{
	vector<MatchArrayPtr> refinedArray1;
	vector<double> refinedDistances1;
	for (MatchArrayPtr array : _matchesPerFrame)
	{
		if (array->getMinDistance() < _discardThreshold)
		{
			refinedArray1.push_back(array);
			refinedDistances1.push_back(array->getMinDistance());
		}
	}
	cout << "Cleared matches size: " << refinedArray1.size() << "\n";

	//for (MatchArrayPtr array : refinedArray1)
	//	printf("Refined match 1 => %-50s\td: %-6.3f\tframe: %-6d\ttime: %-6.2f\n", array->getMinDistanceQuery().c_str(), array->getMinDistance(), array->getFrame(), array->getFrame() / _targetFrameRate);

	// Calculate a refined threshold
	sort(refinedDistances1.begin(), refinedDistances1.end());
	double medianDistance = Helper::getMedian(refinedDistances1);
	Scalar meanDistance, stdDev;
	meanStdDev(refinedDistances1, meanDistance, stdDev);
	double refinedThreshold = fmin(medianDistance, meanDistance[0] - stdDev[0]);

	vector<MatchArrayPtr> refinedMatches;
	vector<double> refinedDistances2;
	for (MatchArrayPtr array : refinedArray1)
	{
		if (array->getMinDistance() < refinedThreshold)
		{
			refinedMatches.push_back(array);
			refinedDistances2.push_back(array->getMinDistance());
		}
	}

	cout << "Clearing outliers\n";
	Helper::clearOutliers(refinedMatches);

	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	cout << "Refined matches size: " << refinedArray1.size() << "\n";
	for (MatchArrayPtr array : refinedMatches)
		printf("Refined match 1 => %-50s\td: %-6.3f\tframe: %-6d\ttime: %-6.2f\n", array->getMinDistanceQuery().c_str(), array->getMinDistance(), array->getFrame(), array->getFrame() / _targetFrameRate);

	// Extract appearances
	double maxJump = Config::getMaxJump();
	map<string, vector<Appearance>> appearances;
	string lastQuery = refinedMatches[0]->getMinDistanceQuery();
	double startTime = refinedMatches[0]->getFrame() / _targetFrameRate;
	double lastTime = startTime;
	for (MatchArrayPtr array : refinedMatches)
	{
		double thisTime = array->getFrame() / _targetFrameRate;
		string query = array->getMinDistanceQuery();

		// Finish a sequence if the query has changed or if a jump in time has been done
		if (lastQuery.compare(query) != 0 || thisTime - lastTime > maxJump)
		{
			if (appearances.find(lastQuery) == appearances.end())
				appearances[lastQuery] = vector<Appearance>();

			appearances[lastQuery].push_back(Appearance(startTime, lastTime - startTime));
			startTime = thisTime;
		}

		lastQuery = query;
		lastTime = thisTime;
	}

	// Add last appearance
	if (appearances.find(lastQuery) == appearances.end())
		appearances[lastQuery] = vector<Appearance>();
	appearances[lastQuery].push_back(Appearance(startTime, lastTime - startTime));

	// Remove short appearances
	for (map<string, vector<Appearance>>::iterator it = appearances.begin(); it != appearances.end(); it++)
	{
		size_t insertPoint = 0;
		size_t seekPoint = 0;
		for (seekPoint = 0; seekPoint < it->second.size(); seekPoint++)
		{
			if (it->second[seekPoint].length >= _minLength)
			{
				if (seekPoint != insertPoint)
					it->second[insertPoint] = it->second[seekPoint];
				insertPoint++;
			}
		}

		it->second.erase(it->second.begin() + insertPoint, it->second.end());
	}

	return appearances;
}

int main(int _nargs, char** _vargs)
{
	if (_nargs < 2)
	{
		cout << "Not enough arguments.\nUsage:\n\tT2 <input_file>\n";
		return EXIT_FAILURE;
	}

	Config::load("./config/config");

	DescType descriptorType = Config::getDescriptorType();
	MetricType metricType = Config::getMetricType();
	int param = Config::getDescriptorParam();

	// Get input file name
	string inputFile = _vargs[1];

	// Get descriptors for each query video
	vector<string> queryLocations = Helper::getQueryLocations(inputFile);
	map<string, vector<DescriptorPtr>> queryDescriptors;
	getQueryDescriptors(queryDescriptors, queryLocations, descriptorType, Config::getQuerySkippedFrames(), param);

	string targetLocation = Helper::getTargetLocation(inputFile);
	cout << "Target video: " << targetLocation << "\n";

	VideoCapture capture;
	capture.open(targetLocation);
	if (!capture.isOpened())
	{
		cout << "ERROR: Can't open target video " << targetLocation << endl;
		return EXIT_FAILURE;
	}
	else
	{
		int skipFrames = Helper::getSkipFrames(Config::getTargetFrameRate(), capture);
		double totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
		vector<MatchArrayPtr> matches;
		matches.reserve((int) (totalFrames / skipFrames) + 1);

		cout << "Total frames in target: " << totalFrames << "\n";

		// Distance limits to be used later
		double minDistance = numeric_limits<double>::max();
		vector<double> minDistances;
		minDistances.reserve(81000 / skipFrames + 1);

		// Process the target video searching for each query
		Mat frame, grayFrame;
		int k = 0, t = 0;
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		for (int j = 0; j < totalFrames; j++)
		{
			if (!capture.grab() || !capture.retrieve(frame))
			{
				cout << "ERROR: Can't get frame from target video. Finishing.\n";
				break;
			}

			if (k == skipFrames)
			{
				cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
				DescriptorPtr currentFrameDescriptor(new Descriptor(grayFrame, j, descriptorType, param));

				// Generate object to store matches for this frame
				matches.push_back(MatchArrayPtr(new MatchArray(j)));

				// Search the current frame in each query video
				for (pair<string, vector<DescriptorPtr>> entry : queryDescriptors)
				{
					vector<Match> queryMatch;
					Helper::findNearestFrames(currentFrameDescriptor, entry.second, metricType, queryMatch);
					matches.back()->addMatch(entry.first, queryMatch);
				}

				k = 0;
				double lastFrameMin = matches.back()->getMinDistance();
				minDistances.push_back(lastFrameMin);
				minDistance = minDistance > lastFrameMin ? lastFrameMin : minDistance;
			}
			k++;

			if (t == 10000)
			{
				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
				cout << "Processing frame " << j << "/" << totalFrames << " - Elapsed time: " << duration / 1E6 << " seg\n";
				t = 0;
			}
			t++;
		}

		sort(minDistances.begin(), minDistances.end());
		double medianDistance = Helper::getMedian(minDistances);
		Scalar meanDistance, stdDev;
		meanStdDev(minDistances, meanDistance, stdDev);

		double thresholdDistance = fmax(minDistance * 2, fmin(medianDistance, meanDistance[0] - stdDev[0]));

		cout << "Extracted " << matches.size() << " from target video.\n";

		// Extract the appearances of each query video
		double fps = capture.get(CV_CAP_PROP_FPS);
		map<string, vector<Appearance>> appearances = extractQueryAppearanceTimes(matches, fps, thresholdDistance, Config::getMinVideoLength());

		// Print appearance times
		FILE *resFile;
		string resFileName = "../results/DESCTYPE_";
		resFileName += Descriptor::ToString(descriptorType);
		resFileName += "-PARAM_";
		resFileName += to_string(param);
		resFileName += "-METRIC_";
//		resFileName += Metric::ToString(metricType);
		resFile = fopen(resFileName.c_str(), "w");
		for (pair<string, vector<Appearance>> entry : appearances)
		{
			for (Appearance ap : entry.second)
			{
				printf("Query: %-50s --start-time=%.2f --run-time=%.2f\n", entry.first.c_str(), ap.startTime, ap.length);
//				fprintf(resFile, "Query: %-50s --start-time=%.2f --run-time=%.2f\n", entry.first.c_str(), ap.startTime, ap.length);
			}
		}
	}

	return EXIT_SUCCESS;
}
