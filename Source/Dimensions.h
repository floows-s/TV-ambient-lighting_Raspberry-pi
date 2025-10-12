#pragma once

#include <opencv2/core.hpp>

struct Dimensions {
	int width;
	int height;
	
	bool equals(Dimensions value) {
		return this->width == value.width && this->height == value.height;
	}

	bool equals(cv::Mat value) {
		return this->width == value.cols && this->height == value.rows;
	}
};