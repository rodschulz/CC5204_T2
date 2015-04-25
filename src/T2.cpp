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

bool getVideoDescriptor(const string &_videoLocation, vector<Descriptor> &_outputDescriptor, const DescType &_type, const int &_subsamplingFrameRate)
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

			double fps = capture.get(CV_CAP_PROP_FPS);
			int skipFrames = fps / _subsamplingFrameRate - 1;

			Mat frame, grayFrame;
			int k = 0;
			double totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);

			cout << "Total frames in query: " << totalFrames << "\n";

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
					_outputDescriptor.push_back(Descriptor(grayFrame, j, _type));

					k = 0;
				}
				k++;
			}
		}
	}

	return statusOk;
}

int main(int _nargs, char** _vargs)
{
	if (_nargs < 2)
	{
		cout << "Not enough arguments.\nUsage:\n\tT2 <input_file>\n";
		return EXIT_FAILURE;
	}

	DescType descriptorType = OMD;
	MetricType metricType = EUCLIDEAN;

	// Get input file name
	string inputFile = _vargs[1];

	// Calculate the descriptors for each query video
	vector<string> queryLocations = Helper::getQueryLocations(inputFile);
	map<string, vector<Descriptor>> queryDescriptors;
	for (String query : queryLocations)
	{
		cout << "Processing query: " << query << "\n";

		queryDescriptors[query] = vector<Descriptor>();
		if (!getVideoDescriptor(query, queryDescriptors[query], descriptorType, 5))
			queryDescriptors.erase(query);

		cout << "Query video descriptor length: " << queryDescriptors[query].size() << "\n";
	}

	// Descriptors estimation
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
		int samplingFrameRate = 3;
		double fps = capture.get(CV_CAP_PROP_FPS);
		int skipFrames = fps / samplingFrameRate - 1;

		double totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
		vector<MatchArray> matches;
		matches.reserve((int) (totalFrames / skipFrames) + 1);

		cout << "Total frames in target: " << totalFrames << "\n";

		// Process the target video searching for each query
		Mat frame, grayFrame;
		int k = 0;
		int t = 0;
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
				Descriptor descriptor(grayFrame, j, descriptorType);

				// Generate object to store matches for this frame
				matches.push_back(MatchArray(j));

				// Search the current frame in each query video
				for (pair<string, vector<Descriptor>> p : queryDescriptors)
				{
					vector<Match> queryMatch;
					Helper::findNearestFrame(descriptor, p.second, metricType, queryMatch);
					matches.back().addMatch(p.first, queryMatch);
				}

				k = 0;
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

		map<string, Match> closestFrame;
		for (String query : queryLocations)
			closestFrame[query] = Match(numeric_limits<double>::max(), Descriptor(), Descriptor());

		// Extract the closer frame to each query
		for (MatchArray array : matches)
		{
			for (String query : queryLocations)
			{
				Match m = array.getMinDistance(query);
				if (closestFrame[query].distance > m.distance)
					closestFrame[query] = m;
			}
		}

		for (pair<string, Match> p : closestFrame)
		{
			double matchTime = p.second.target.getFrame() / fps;
			cout << "Closer time to video " << p.first << " is " << matchTime << "\n";
		}
	}

	return EXIT_SUCCESS;
}
