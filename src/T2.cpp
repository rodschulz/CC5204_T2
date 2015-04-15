/**
 * Author: rodrigo
 * 2015
 */
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "Descriptor.h"
#include "Index.h"

using namespace std;

int main(int _nargs, char** _vargs)
{
	if (_nargs < 2)
	{
		cout << "Not enough arguments.\nUsage:\n\tT2 <input_file>\n";
		return EXIT_FAILURE;
	}
	
	string inputFile = _vargs[0];
	
	// Descriptors estimation
	vector<Descriptor> videoDescriptor;
	vector<Descriptor> queryDescriptor;
	
	/**
	 * Magic
	 */
	
	// Index calculation
	Index index = Index();
	
	// Search
	
	return EXIT_SUCCESS;
}
