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

using namespace std;
using namespace cv;

/*
 * Retorna un vector que corresponde al descriptor del video capture completo.
 * Cada Descriptor es el descriptor de un frame, contiene los 4 histogramas.
 */
vector<Descriptor> getVideoDescriptor(VideoCapture capture){
	vector<Descriptor> videoDescriptor;

	int fps = capture.get(CV_CAP_PROP_FPS);
	int totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);

	cout << "fps " << fps << endl;
	cout << "fps/3 = " << fps/3 << endl;
	cout << "total " << totalFrames << endl;

	cout << "width: " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
	cout << "height: " << capture.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

	Mat frame, grayFrame;
	for (int j = 0; j <= totalFrames; j++) {
		if (!capture.grab() || !capture.retrieve(frame))
			break;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		int frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

		int areaWidth = frameWidth/2;
		int areaHeight = frameHeight/2;
		Mat area1, area2, area3, area4;
		/* | 1 | 2 |
		 * | 3 | 4 |
		 */
		area1 = grayFrame(Rect(0, 0, areaWidth, areaHeight));
		area2 = grayFrame(Rect(areaWidth, 0, areaWidth, areaHeight));
		area3 = grayFrame(Rect(0, areaHeight, areaWidth, areaHeight));
		area4 = grayFrame(Rect(areaWidth, areaHeight, areaWidth, areaHeight));

		//Histogram
		int histSize = 128;
		float range[] = {0, 256}; //the upper boundary is exclusive
		const float* histRange = {range};
		bool uniform = true; bool accumulate = false;
		Mat hist1, hist2, hist3, hist4;
		// Calculate histogram for each area
		calcHist(&area1, 1, 0, Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&area2, 1, 0, Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&area3, 1, 0, Mat(), hist3, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&area4, 1, 0, Mat(), hist4, 1, &histSize, &histRange, uniform, accumulate);

		// Create images to show histograms
		int hist_h = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		int bin_w = cvRound((double)frameWidth/histSize);
		Mat histImage1(hist_h, frameWidth, CV_8UC3, Scalar(0, 0, 0));
		Mat histImage2(hist_h, frameWidth, CV_8UC3, Scalar(0, 0, 0));
		Mat histImage3(hist_h, frameWidth, CV_8UC3, Scalar(0, 0, 0));
		Mat histImage4(hist_h, frameWidth, CV_8UC3, Scalar(0, 0, 0));

		// Normalize each histrogram
		normalize(hist1, hist1, 0, histImage1.rows, NORM_MINMAX, -1, Mat());
		normalize(hist2, hist2, 0, histImage2.rows, NORM_MINMAX, -1, Mat());
		normalize(hist3, hist3, 0, histImage3.rows, NORM_MINMAX, -1, Mat());
		normalize(hist4, hist4, 0, histImage4.rows, NORM_MINMAX, -1, Mat());

		// Frame descriptor: the 4 histograms
		Descriptor frameDescriptor(hist1, hist2, hist3, hist4);
		videoDescriptor.push_back(frameDescriptor);

		// Draw histograms
		/*for(int i = 1; i < histSize; i++ ){
			line(histImage1, Point( bin_w*(i-1), hist_h - cvRound(hist1.at<float>(i-1)) ) ,
							 Point( bin_w*(i), hist_h - cvRound(hist1.at<float>(i)) ),
							 Scalar( 255, 0, 0), 2, 8, 0  );
		}
		for(int i = 1; i < histSize; i++ ){
			line(histImage2, Point( bin_w*(i-1), hist_h - cvRound(hist2.at<float>(i-1)) ) ,
							 Point( bin_w*(i), hist_h - cvRound(hist2.at<float>(i)) ),
							 Scalar( 255, 0, 0), 2, 8, 0  );
		}
		for(int i = 1; i < histSize; i++ ){
			line(histImage3, Point( bin_w*(i-1), hist_h - cvRound(hist3.at<float>(i-1)) ) ,
							 Point( bin_w*(i), hist_h - cvRound(hist3.at<float>(i)) ),
							 Scalar( 255, 0, 0), 2, 8, 0  );
		}
		for(int i = 1; i < histSize; i++ ){
			line(histImage4, Point( bin_w*(i-1), hist_h - cvRound(hist4.at<float>(i-1)) ) ,
							 Point( bin_w*(i), hist_h - cvRound(hist4.at<float>(i)) ),
							 Scalar( 255, 0, 0), 2, 8, 0  );
		}*/

		// Create big image to display all histograms
		/*Mat dst(frameHeight, frameWidth, CV_8UC3);
		Mat h1 = Mat(dst, Range(areaHeight, frameHeight), Range(areaWidth, frameWidth));
		histImage1.copyTo(h1);*/


		/*imshow("histogram1", histImage1);
		imshow("histogram2", histImage2);
		imshow("histogram3", histImage3);
		imshow("histogram4", histImage4);
		imshow("video", grayFrame);

		char c = waitKey(33);
		if (c == 27)
			break;*/

	}
	return videoDescriptor;
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

	// Descriptors estimation
	vector<Descriptor> videoDescriptor;
	string targetLocation = Helper::getTargetLocation(inputFile);
	
	cout << "Target video: " << targetLocation << "\n";
	
	/**
	 * Magic ~°~°~ Creating videoDescriptor ~°
	 */
	VideoCapture capture;
	capture.open(targetLocation);

	if (!capture.isOpened()) {
		cout << "Can't open target video " << targetLocation << endl;
		return 1;
	}

	videoDescriptor = getVideoDescriptor(capture);

	
	// Index calculation (for the searched file)
	Index index = Index();
	
	vector<string> queryLocation = Helper::getQueryLocations(inputFile);
	for (string location : queryLocation)
	{
		cout << "Procesing query: " << location << "\n";

		vector<Descriptor> queryDescriptor;
		
		VideoCapture capture;
		capture.open(location);

		int fps = capture.get(CV_CAP_PROP_FPS);

		/**
		 * Magic
		 */
		queryDescriptor = getVideoDescriptor(capture);
		
		// Nearest neighbor search
	}
	
	return EXIT_SUCCESS;
}
