#include "Model.h"
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>

void Model::UpdateBounds()
{
	for (int i = 0; i < facets.size(); i++)
	{
		// update X dimensions
		if (facets[i].a.x > max_x)
			max_x = facets[i].a.x;
		else if (facets[i].a.x < min_x)
			min_x = facets[i].a.x;

		if (facets[i].b.x > max_x)
			max_x = facets[i].b.x;
		else if (facets[i].b.x < min_x)
			min_x = facets[i].b.x;

		if (facets[i].c.x > max_x)
			max_x = facets[i].c.x;
		else if (facets[i].c.x < min_x)
			min_x = facets[i].c.x;

		// update Y dimensions
		if (facets[i].a.y > max_y)
			max_y = facets[i].a.y;
		else if (facets[i].a.y < min_y)
			min_y = facets[i].a.y;

		if (facets[i].b.y > max_y)
			max_y = facets[i].b.y;
		else if (facets[i].b.y < min_y)
			min_y = facets[i].b.y;

		if (facets[i].c.y > max_y)
			max_y = facets[i].c.y;
		else if (facets[i].c.y < min_y)
			min_y = facets[i].c.y;

		// update Z dimensions
		if (facets[i].a.z > max_z)
			max_z = facets[i].a.z;
		else if (facets[i].a.z < min_z)
			min_z = facets[i].a.z;

		if (facets[i].b.z > max_z)
			max_z = facets[i].b.z;
		else if (facets[i].b.z < min_z)
			min_z = facets[i].b.z;

		if (facets[i].c.z > max_z)
			max_z = facets[i].c.z;
		else if (facets[i].c.z < min_z)
			min_z = facets[i].c.z;
	}
	span_x = max_x - min_x;
	span_y = max_y - min_y;
	span_z = max_z - min_z;
}


void Model::LoadModel(char * pathToFile)
{
	std::ifstream fin(pathToFile, std::ifstream::binary);
	fin.seekg(80);
	char numFacets[4];
	long facetCount = 0;

	if (fin)
	{
		// grab number of facets.
		fin.read(numFacets, 4);
		memcpy(&facetCount, numFacets, sizeof(long));
	}

	// now grab file length by going to the end of the file and reading cursor position
	fin.seekg(0, std::ios_base::end);
	int filesize = fin.tellg();
	fin.close();

	// binary STL filesize is 84 bytes plus 50 * number of facets
	if (filesize == facetCount * 50 + 84)
	{
		LoadBinaryFile(pathToFile);
	}
	else
	{
		// assume ascii file
		LoadAsciiFile(pathToFile);
	}
}

void Model::LoadBinaryFile(char * pathToFile)
{
	std::ifstream fin(pathToFile, std::ios::binary);
	fin.clear();
	fin.seekg(0, std::ios::beg);
	// Binary STL has 80 byte header
	char stl_header[80];

	// Number of facets stored in 4 byte unsigned int
	char numFacets[4];
	long facetCount = 0;

	// start by getting the header out of the way
	if (fin)
	{
		fin.read(stl_header, 80);
	}
	else
	{
		// error
	}

	// now get the number of triangles in the file
	if (fin)
	{
		fin.read(numFacets, 4);
		//facetCount = numFacets;
		memcpy(&facetCount, numFacets, sizeof(long));
	}
	else
	{
		// error
	}

	Facet triangle;

	// read in facetCount number of facets
	for (int i = 0; i < facetCount; i++)
	{
		if (fin)
		{
			// each facet is represented by 50 bytes (3*4 bytes for each point and normal, and 2 useless bytes
			char facetInput[50];
			fin.read(facetInput, 50);

			//Binary stl stores numbers as floats, our software uses doubles
			// so we have to do some conversions

			float normx = 0;
			float normy = 0;
			float normz = 0;

			float p1x = 0;
			float p1y = 0;
			float p1z = 0;

			float p2x = 0;
			float p2y = 0;
			float p2z = 0;

			float p3x = 0;
			float p3y = 0;
			float p3z = 0;

			// fill normal vector
			memcpy(&normx, facetInput, sizeof(float));
			triangle.norm.x = (double)normx;

			memcpy(&normy, facetInput + 4, sizeof(float));
			triangle.norm.y = (double)normy;

			memcpy(&normz, facetInput + 8, sizeof(float));
			triangle.norm.z = (double)normz;


			// fill three points
			memcpy(&p1x, facetInput + 12, sizeof(float));
			triangle.a.x = (double)p1x;
			memcpy(&p1y, facetInput + 16, sizeof(float));
			triangle.a.y = (double)p1y;
			memcpy(&p1z, facetInput + 20, sizeof(float));
			triangle.a.z = (double)p1z;

			memcpy(&p2x, facetInput + 24, sizeof(float));
			triangle.b.x = (double)p2x;
			memcpy(&p2y, facetInput + 28, sizeof(float));
			triangle.b.y = (double)p2y;
			memcpy(&p2z, facetInput + 32, sizeof(float));
			triangle.b.z = (double)p2z;

			memcpy(&p3x, facetInput + 36, sizeof(float));
			triangle.c.x = (double)p3x;
			memcpy(&p3y, facetInput + 40, sizeof(float));
			triangle.c.y = (double)p3y;
			memcpy(&p3z, facetInput + 44, sizeof(float));
			triangle.c.z = (double)p3z;

			facets.push_back(triangle);
		}
	}

	fin.close();

	// get min/max values of model
	UpdateBounds();
}

void Model::LoadAsciiFile(char * pathToFile)
{
	std::ifstream fin(pathToFile);

	Facet		triangle;
	std::string garb = "";
	int	i;

	bool breakLoop = false;	


	while (_stricmp(garb.c_str(), "normal")) // loop till you find normal
		fin >> garb;

	while (garb.find("solid") == std::string::npos  && !fin.eof() && !breakLoop)
	{

		fin >> triangle.norm.x
			>> triangle.norm.y
			>> triangle.norm.z;

		for (i = 0; i<2; i++)
			fin >> garb;


		fin >> garb;

		fin >> triangle.a.x
			>> triangle.a.y
			>> triangle.a.z;

		fin >> garb;

		fin >> triangle.b.x
			>> triangle.b.y
			>> triangle.b.z;

		fin >> garb;

		fin >> triangle.c.x
			>> triangle.c.y
			>> triangle.c.z;

		facets.push_back(triangle);
		for (i = 0; i<4; i++){
			fin >> garb;
			if (garb.find("endsolid") != std::string::npos) breakLoop = true;
		}
	}

	fin.close();

	UpdateBounds();
}

float Model::GetMaxDim()
{
	if (span_x >= span_y & span_x >= span_z)
		return span_x;

	if (span_y >= span_z)
		return span_y;

	return span_z; 
}

void Model::ScaleByPercent(float percent)
{
	for (int i = 0; i < facets.size(); i++)
	{
		facets[i].Scale(percent);
	}

}


void Model::ScaleToDimension(float maxDim)
{
	ScaleByPercent(maxDim / GetMaxDim());
}

void Model::SaveModel(char * pathToFile, bool binary)
{
	if (!binary)
	{
		FILE * fp;
		fopen_s(&fp, pathToFile, "w");
		if (fp == NULL)
			return;

		fprintf(fp, "solid ascii\n");

		int j;

		for (unsigned int i = 0; i<facets.size(); i++)
		{
			fprintf(fp, "  facet normal %.6f %.6f %.6f\n", facets[i].norm.x,
				facets[i].norm.y,
				facets[i].norm.z);
			fprintf(fp, "    outer loop\n");


			fprintf(fp, "      vertex %.6f %.6f %.6f\n", facets[i].a.x,
			facets[i].a.y,
			facets[i].a.z);

			fprintf(fp, "      vertex %.6f %.6f %.6f\n", facets[i].b.x,
				facets[i].b.y,
				facets[i].b.z);

			fprintf(fp, "      vertex %.6f %.6f %.6f\n", facets[i].c.x,
				facets[i].c.y,
				facets[i].c.z);

			fprintf(fp, "    endloop\n");
			fprintf(fp, "  endfacet\n");
		}

		fprintf(fp, "end solid\n");

		fclose(fp);
	}
	else
	{
		FILE * fp;
		fopen_s(&fp, pathToFile, "wb");
		if (fp == NULL)
			return;

		// write 80 random bytes for the header
		char header[] = "EXPT Binary STL File. 80 byte header, 4 byte facet count, and 50 byte facets. This can be anything, but must be at least 80 bytes long in order to fill the header space";

		//write 1 byte 80 times
		fwrite(header, 1, 80, fp);

		//fprintf(fp, "solid ascii\n");

		unsigned long int numFacets[1];
		numFacets[0] = static_cast<unsigned long int>(this->facets.size());

		int size = sizeof(unsigned long int);

		//write one unsigned long int once (number of facets)
		fwrite(numFacets, sizeof(unsigned long int), 1, fp);

		for (unsigned int i = 0; i < this->facets.size(); i++)
		{
			float floatsToWrite[12];

			// normal 
			floatsToWrite[0] = facets[i].norm.x;
			floatsToWrite[1] = facets[i].norm.y;
			floatsToWrite[2] = facets[i].norm.z;

			// point 1
			floatsToWrite[3] = facets[i].a.x;
			floatsToWrite[4] = facets[i].a.y;
			floatsToWrite[5] = facets[i].a.z;

			// point 2
			floatsToWrite[6] = facets[i].b.x;
			floatsToWrite[7] = facets[i].b.y;
			floatsToWrite[8] = facets[i].b.z;

			// point 3
			floatsToWrite[9] = facets[i].c.x;
			floatsToWrite[10] = facets[i].c.y;
			floatsToWrite[11] = facets[i].c.z;

			fwrite(floatsToWrite, sizeof(float), 12, fp);

			// write the 2 byte "attribute"
			short attribute[1];

			attribute[0] = 0;

			fwrite(attribute, sizeof(short), 1, fp);
		}

		fclose(fp);
	}
}