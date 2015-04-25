/**
 * Author: rodrigo
 * 2015
 */
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Descriptor.h"
#include "Helper.h"
#include "MatchArray.h"
#include "Metric.h"

using namespace std;
using namespace cv;

bool getVideoDescriptor(const string &_videoLocation, vector<DescriptorPtr> &_outputDescriptor, const DescType &_type, const int &_subsamplingFrameRate)
{
	bool statusOk = true;

	if (_subsamplingFrameRate < 1)
	{
		cout << "ERROR: invalid frame rate to calculate descriptor\n";
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
			int skipFrames = Helper::getSkipFrames(_subsamplingFrameRate, capture);
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

				if (k == skipFrames)
				{
					cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
					_outputDescriptor.push_back(DescriptorPtr(new Descriptor(grayFrame, j, _type)));

					k = 0;
				}
				k++;
			}
		}
	}

	return statusOk;
}

void getQueryDescriptors(map<string, vector<DescriptorPtr>> &_queryDescriptors, const vector<string> &_queryLocations, const DescType &_descriptorType)
{
	for (String query : _queryLocations)
	{
		cout << "Processing query: " << query << "\n";

		_queryDescriptors[query] = vector<DescriptorPtr>();
		if (!getVideoDescriptor(query, _queryDescriptors[query], _descriptorType, 5))
			_queryDescriptors.erase(query);

		cout << "Query video descriptor length: " << _queryDescriptors[query].size() << "\n";
	}
}

void getQueryAppearanceTimes(const double _globalMinDist, const vector<MatchArrayPtr> &_matchesPerFrame)
{
	// Discard frames where the closest frame is farther than two times the global minimum distance
	double discardThreshold = _globalMinDist;

	vector<MatchArrayPtr> clearedArray;
	clearedArray.reserve(_matchesPerFrame.size());
//	for (size_t i )

}

int main(int _nargs, char** _vargs)
{
	if (_nargs < 2)
	{
		cout << "Not enough arguments.\nUsage:\n\tT2 <input_file>\n";
		return EXIT_FAILURE;
	}

	DescType descriptorType = OMD;
	MetricType metricType = MANHATTAN;

	// Get input file name
	string inputFile = _vargs[1];

	// Get descriptors for each query video
	vector<string> queryLocations = Helper::getQueryLocations(inputFile);
	map<string, vector<DescriptorPtr>> queryDescriptors;
	getQueryDescriptors(queryDescriptors, queryLocations, descriptorType);

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
		int skipFrames = Helper::getSkipFrames(3, capture);
		double totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
		vector<MatchArrayPtr> matches;
		matches.reserve((int) (totalFrames / skipFrames) + 1);

		cout << "Total frames in target: " << totalFrames << "\n";

		// Distance limits to be used later
		double minDistance = numeric_limits<double>::max();

		// Process the target video searching for each query
		Mat frame, grayFrame;
		int k = 0, t = 0;
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
				DescriptorPtr currentFrameDescriptor(new Descriptor(grayFrame, j, descriptorType));

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
				minDistance = minDistance > lastFrameMin ? lastFrameMin : minDistance;
			}
			k++;

			if (t == 10000)
			{
				cout << "Processing frame " << j << "/" << totalFrames << "\n";
				t = 0;
			}
			t++;
		}

		cout << "Extracted " << matches.size() << " from target video.\n";

		getQueryAppearanceTimes(minDistance, matches);

		/////////////////////////////

//		map<string, Match> closestFrame;
//		for (String query : queryLocations)
//			closestFrame[query] = Match(numeric_limits<double>::max(), Descriptor(), Descriptor());
//
//		// Extract the closer frame to each query
//		for (MatchArray array : matches)
//		{
//			for (String query : queryLocations)
//			{
//				Match m = array.getQueryMinDistance(query);
//				if (closestFrame[query].distance > m.distance)
//					closestFrame[query] = m;
//			}
//		}
//
//		double fps = capture.get(CV_CAP_PROP_FPS);
//		for (pair<string, Match> p : closestFrame)
//		{
//			double matchTime = p.second.target.getFrame() / fps;
//			cout << "Closer time to video " << p.first << " is " << matchTime << "\n";
//		}
	}

	return EXIT_SUCCESS;
}
