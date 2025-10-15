#pragma once

#include <opencv2/core.hpp>

/// <summary>
/// A struct to store width and height.
/// </summary>
struct Dimensions {
	int width;
	int height;

	bool operator==(const Dimensions& value) {
		return width == value.width && height == value.height;
	}

	bool equals(cv::Mat value) {
		return width == value.cols && height == value.rows;
	}
};