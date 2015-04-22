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

/* Normaliza histograma segun el total de pixeles del area */
vector<double> normalizeHist(vector<double> hist, int bins, double total){
	for(int i = 0; i < bins; i++){
		hist[i] = hist[i]/total;
	}
	return hist;
}


void getOMDDescriptor(VideoCapture &_capture, vector<Descriptor> &_videoDescriptor){
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
			int frameWidth = _capture.get(CV_CAP_PROP_FRAME_WIDTH);
			int frameHeight = _capture.get(CV_CAP_PROP_FRAME_HEIGHT);

			/**
			 * | 1 | 2 | 3 |
		 	 * | 4 | 5 | 6 |
		 	 * | 7 | 8 | 9 |
		 	 */
			int areaWidth = frameWidth / 3;
			int areaHeight = frameHeight / 3;

			Mat area1, area2, area3, area4, area5, area6, area7, area8, area9;
			area1 = grayFrame(Rect(0, 0, areaWidth, areaHeight));
			area2 = grayFrame(Rect(areaWidth, 0, areaWidth, areaHeight));
			area3 = grayFrame(Rect(2 * areaWidth, 0, areaWidth, areaHeight));
			area4 = grayFrame(Rect(0, areaHeight, areaWidth, areaHeight));
			area5 = grayFrame(Rect(areaWidth, areaHeight, areaWidth, areaHeight));
			area6 = grayFrame(Rect(2 * areaWidth, areaHeight, areaWidth, areaHeight));
			area7 = grayFrame(Rect(0, 2 * areaHeight, areaWidth, areaHeight));
			area8 = grayFrame(Rect(areaWidth, 2 * areaHeight, areaWidth, areaHeight));
			area9 = grayFrame(Rect(2 * areaWidth, 2 * areaHeight, areaWidth, areaHeight));

			// Calculate average (mean) intensity on each area, add to vector
			vector<double> meanIntensities;
			meanIntensities.push_back(mean(area1)[0]);
			meanIntensities.push_back(mean(area2)[0]);
			meanIntensities.push_back(mean(area3)[0]);
			meanIntensities.push_back(mean(area4)[0]);
			meanIntensities.push_back(mean(area5)[0]);
			meanIntensities.push_back(mean(area6)[0]);
			meanIntensities.push_back(mean(area7)[0]);
			meanIntensities.push_back(mean(area8)[0]);
			meanIntensities.push_back(mean(area9)[0]);

			// Calculate the position on the intensity scale of the image, for each area
			vector<double> sortedIntensities(meanIntensities.size());
			copy(meanIntensities.begin(), meanIntensities.end(), sortedIntensities.begin());
			sort(sortedIntensities.begin(), sortedIntensities.end());

			vector<double> omd(meanIntensities.size());
			for(int i = 0; i < meanIntensities.size(); i++){
				int pos = find(sortedIntensities.begin(), sortedIntensities.end(), meanIntensities[i]) - sortedIntensities.begin();
				omd[i] = pos;
			}

			_videoDescriptor.push_back(Descriptor(omd, NULL, NULL, NULL));

			k = 0;
		}
		k++;
	}

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
			float range[] = {0, 256};
			const float *histRange = {range};
			bool uniform = true;
			bool accumulate = false;
			Mat hist1, hist2, hist3, hist4;

			// Calculate histogram for each area
			calcHist(&area1, 1, 0, Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
			calcHist(&area2, 1, 0, Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);
			calcHist(&area3, 1, 0, Mat(), hist3, 1, &histSize, &histRange, uniform, accumulate);
			calcHist(&area4, 1, 0, Mat(), hist4, 1, &histSize, &histRange, uniform, accumulate);

			// Normalize each histogram and copy to vector
			double totalPxs = areaHeight * areaWidth;
			vector<double> normHist1 = normalizeHist(hist1, histSize, totalPxs);
			vector<double> normHist2 = normalizeHist(hist2, histSize, totalPxs);
			vector<double> normHist3 = normalizeHist(hist3, histSize, totalPxs);
			vector<double> normHist4 = normalizeHist(hist4, histSize, totalPxs);

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
