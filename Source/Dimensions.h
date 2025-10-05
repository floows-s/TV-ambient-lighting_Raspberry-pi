#pragma once
struct Dimensions {
	unsigned int width;
	unsigned int height;
	
	bool equals(Dimensions value) {
		return this->width == value.width && this->height == value.height;
	}

	bool equals(cv::Mat value) {
		return this->width == value.rows && this->height == value.cols;
	}
};