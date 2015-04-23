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
#include "Index.h"
#include "Helper.h"
#include "Result.h"

using namespace std;
using namespace cv;

void getVideoDescriptor(VideoCapture &_capture, vector<Descriptor> &_videoDescriptor, const DescType &_type){
	_videoDescriptor.clear();

	double fps = _capture.get(CV_CAP_PROP_FPS);
	double totalFrames = _capture.get(CV_CAP_PROP_FRAME_COUNT);

	int skipFrames = fps / 3 - 1;

	Mat frame, grayFrame;
	int k = 0;
	for (int j = 0; j <= totalFrames; j++) {
		if (!_capture.grab() || !_capture.retrieve(frame))
			break;

		if (k == skipFrames) {
			cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
			_videoDescriptor.push_back(Descriptor(grayFrame, _type));

			k = 0;
		}
		k++;
	}

}

int main(int _nargs, char** _vargs)
{
	if (_nargs < 2)
	{
		cout << "Not enough arguments.\nUsage:\n\tT2 <input_file>\n";
		return EXIT_FAILURE;
	}

	// Get input file name
	string inputFile = _vargs[1];
	srand(time(NULL));

	// Descriptors estimation
	string targetLocation = Helper::getTargetLocation(inputFile);
	cout << "Target video: " << targetLocation << "\n";

	VideoCapture capture;
	capture.open(targetLocation);
	if (!capture.isOpened())
	{
		cout << "Can't open target video " << targetLocation << endl;
		return EXIT_FAILURE;
	}

	// Create the descriptor of the video
	vector<Descriptor> videoDescriptor;
	getVideoDescriptor(capture, videoDescriptor, OMD);

	/** TEST *
	 int n = Helper::getRandomNumber(505, 600);
	 videoDescriptor.clear();
	 videoDescriptor.reserve(n);
	 for (int i = 0; i < n; i++)
	 {
	 vector<vector<double>> v(4);
	 for (int k = 0; k < 4; k++)
	 {
	 v[k] = vector<double>(128, 0);
	 for (int j = 0; j < 128; j++)
	 {
	 double d = Helper::getRandomNumber(1, 100);
	 v[k][j] = d / 100.0;
	 }
	 }

	 videoDescriptor.push_back(Descriptor(v[0], v[1], v[2], v[3]));
	 }
	 ** TEMPORAL */

	cout << "Target video descriptor length: " << videoDescriptor.size() << "\n";

	// Index construction (put the magic into the magic)
	Index index = Index(videoDescriptor, EUCLIDEAN);

	/** TEST *
	 Descriptor query = videoDescriptor[500];
	 query.whole[0] += 0.2;
	 query.whole[10] -= 0.3;
	 pair<int, const Descriptor *> nn = index.findNearestNeighbor(query);
	 ** TEST */

	vector<Result> results;
	vector<string> queryLocation = Helper::getQueryLocations(inputFile);
	for (string location : queryLocation)
	{
		cout << "Processing query: " << location << "\n";

		VideoCapture queryCapture;
		queryCapture.open(location);

		if (!queryCapture.isOpened())
		{
			cout << "Can't open target video " << targetLocation << endl;
			return EXIT_FAILURE;
		}

		// Do the hustle
		vector<Descriptor> queryDescriptors;
		getVideoDescriptor(capture, queryDescriptors, HIST);

		Result queryResults(location, 3);
		for (Descriptor d : queryDescriptors)
		{
			pair<int, const Descriptor *> NN = index.findNearestNeighbor(d);
			queryResults.addMatchingFrame(NN);
		}

		results.push_back(queryResults);
	}

	return EXIT_SUCCESS;
}
