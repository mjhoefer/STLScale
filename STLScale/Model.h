#include <vector>

class Point3d
{
public: 
	float x;
	float y;
	float z;

	Point3d()
	{

	}

	Point3d(float x_in, float y_in, float z_in)
	{
		x = x_in;
		y = y_in;
		z = z_in;
	}

	void Scale(float percent)
	{
		x = x * percent;
		y = y * percent;
		z = z * percent;
	}
};


class Facet
{
public:
	Point3d a;
	Point3d b;
	Point3d c;

	// normal vector can be represented by a point in space
	Point3d norm;

	Facet()
	{

	}

	Facet(Point3d a_in, Point3d b_in, Point3d c_in)
	{
		a = a_in;
		b = b_in;
		c = c_in;
	}

	void Scale(float percent)
	{
		a.Scale(percent);
		b.Scale(percent);
		c.Scale(percent);
	}

};


class Model
{
public:

	float min_x;
	float max_x;
	float min_y;
	float max_y;
	float min_z;
	float max_z;

	float span_x;
	float span_y;
	float span_z;

	std::vector<Facet> facets;

	Model()
	{
		facets = std::vector<Facet>();
	}
	void LoadModel(char * pathToFile);
	void LoadBinaryFile(char * pathToFile);
	void LoadAsciiFile(char * pathToFile);
	void ScaleByPercent(float percent);

	// scales the model so the largest dimension is equal to the maxDim argument
	void ScaleToDimension(float maxDim);
	void SaveModel(char * pathToFile, bool binary);
	void UpdateBounds();

	float GetMaxDim();
};