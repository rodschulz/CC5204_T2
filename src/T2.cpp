/**
 * Author: rodrigo
 * 2015
 */
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "Descriptor.h"
#include "Index.h"
#include "Helper.h"

using namespace std;

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
	 * Magic
	 */
	
	// Index calculation (for the searched file)
	Index index = Index();
	
	vector<string> queryLocation = Helper::getQueryLocations(inputFile);
	for (string location : queryLocation)
	{
		cout << "Procesing query: " << location << "\n";

		vector<Descriptor> queryDescriptor;
		
		/**
		 * Magic
		 */
		
		// Nearest neighbor search
	}
	
	return EXIT_SUCCESS;
}
