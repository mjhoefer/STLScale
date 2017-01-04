// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Model.h"
#include <iostream>

int main(int argc, char** argv)
{
	// argument 0 is the name of the exectuable 
	// argument 1 will be the path to the input STL file
	// argument 2 should be the path to the output STL file
	// argument 3 should be if the output should be binary or not (true or false)
	// agrument 4, dimension to scale to
	for (int i = 0; i < argc; ++i)
	{
		std::cout << "argument " << i << ":" << argv[i] << std::endl;
	}

	Model myModel = Model();
	myModel.LoadModel(argv[1]);
	myModel.ScaleToDimension(atoi(argv[4]));
	myModel.SaveModel(argv[2], atoi(argv[3]));

	return 0;
}

