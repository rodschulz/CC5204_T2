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

vector<double> normalizeHist(vector<double> hist, double total){
	for(int i = 0; i < 128; i++){
		//cout << "HIST: " << hist[i] << endl;
		hist[i] = hist[i]/total;
		//cout << "NORM: " << hist[i] << endl;
	}
	return hist;
}

/*
 * Retorna un vector que corresponde al descriptor del video capture completo.
 * Cada Descriptor es el descriptor de un frame, contiene los 4 histogramas.
 */
void getVideoDescriptor(VideoCapture &_capture, vector<Descriptor> &_videoDescriptor)
{
	_videoDescriptor.clear();

	double fps = _capture.get(CV_CAP_PROP_FPS);
	double totalFrames = _capture.get(CV_CAP_PROP_FRAME_COUNT);
	/*double length = totalFrames / fps;

	 cout << "Video dura " << length << " segundos." << endl;
	 cout << "Deberia tener " << length*3 << " frames." << endl;

	 cout << "fps " << fps << endl;*/

	int skipFrames = fps / 3 - 1;

	Mat frame, grayFrame;
	int k = 0;
	for (int j = 0; j <= totalFrames; j++) {
		if (!_capture.grab() || !_capture.retrieve(frame))
			break;

		if (k == skipFrames) {
			cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
			int frameWidth = _capture.get(CV_CAP_PROP_FRAME_WIDTH);
			int frameHeight = _capture.get(CV_CAP_PROP_FRAME_HEIGHT);

			/**
			 * | 1 | 2 |
		 	 * | 3 | 4 |
		 	 */
			int areaWidth = frameWidth / 2;
			int areaHeight = frameHeight / 2;
			Mat area1, area2, area3, area4;
			area1 = grayFrame(Rect(0, 0, areaWidth, areaHeight));
			area2 = grayFrame(Rect(areaWidth, 0, areaWidth, areaHeight));
			area3 = grayFrame(Rect(0, areaHeight, areaWidth, areaHeight));
			area4 = grayFrame(Rect(areaWidth, areaHeight, areaWidth, areaHeight));

			//Histogram
			int histSize = 128;
			//the upper boundary is exclusive
			float range[] =
					{0, 256};
			const float *histRange =
					{range};
			bool uniform = true;
			bool accumulate = false;
			Mat hist1, hist2, hist3, hist4;

			// Calculate histogram for each area
			calcHist(&area1, 1, 0, Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
			calcHist(&area2, 1, 0, Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);
			calcHist(&area3, 1, 0, Mat(), hist3, 1, &histSize, &histRange, uniform, accumulate);
			calcHist(&area4, 1, 0, Mat(), hist4, 1, &histSize, &histRange, uniform, accumulate);

			// Normalize each histogram
			double totalPxs = areaHeight * areaWidth;
			vector<double> normHist1 = normalizeHist(hist1, totalPxs);
			vector<double> normHist2 = normalizeHist(hist2, totalPxs);
			vector<double> normHist3 = normalizeHist(hist3, totalPxs);
			vector<double> normHist4 = normalizeHist(hist4, totalPxs);

			// Frame descriptor: the 4 histograms
			_videoDescriptor.push_back(Descriptor(normHist1, normHist2, normHist3, normHist4));

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
	getVideoDescriptor(capture, videoDescriptor);

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
		getVideoDescriptor(capture, queryDescriptors);

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
