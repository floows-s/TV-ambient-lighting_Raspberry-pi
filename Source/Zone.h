#pragma once

#include <opencv2/core.hpp>
#include <Color.h>

class Zone
{

public:
	// Constructor & destructor
	Zone();
	~Zone();

	// Methods
	Color calculate(cv::Mat& frame);
	void draw(cv::Mat& frame);

	// Getters & setters
	uint getWidth() { return width; }
	void setWidth(uint value) { width = value; }

	uint getHeight() { return height; }
	void setHeight(uint value) { height = value; }
	
private:
	uint width;
	uint height;
	Color lastCalculatedAverageColor;
};

