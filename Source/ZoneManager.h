#pragma once

#include <opencv2/core.hpp>

class ZoneManager
{
public:
	// Constructor & destructor
	ZoneManager();
	~ZoneManager();

	// Methods
	void calculate(cv::Mat& frame);
	void draw(cv::Mat& frame);

private:


};

