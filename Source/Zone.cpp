#include "Zone.h"
#include "Dimensions.h"

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

Zone::Zone(Dimensions dimensions, cv::Point origin, cv::Scalar borderColor)
	: m_dimensions(dimensions), m_borderColor(borderColor), m_origin(origin){ }

void Zone::draw(cv::Mat& frame, bool includeAverageColor) {

	// Average color
	if (true) {
		cv::rectangle(
			frame,
			this->m_origin,
			cv::Point(
				this->m_origin.x + this->m_dimensions.width,
				this->m_origin.y + this->m_dimensions.height
			),
			m_lastCalculatedAverageColor,
			-1
		);
	}

	// Border
	cv::rectangle(
		frame,
		this->m_origin,
		cv::Point(
			this->m_origin.x + this->m_dimensions.width,
			this->m_origin.y + this->m_dimensions.height
		),
		m_borderColor,
		3
	);
}

cv::Scalar Zone::calculate(cv::Mat& frame) {
	// Init sum and count
	// Note: OpenCV orders in BGR
	int blueSum = 0;
	int greenSum = 0;
	int redSum = 0;

	// Set region that needs to looped over
	int beginY = this->m_origin.y;
	int endY = this->m_origin.y + this->m_dimensions.height;

	int beginX = this->m_origin.x;
	int endX = this->m_origin.x + this->m_dimensions.width;

	// Loop over region
	for (int y = beginY; y < endY; y++) {
		for (int x = beginX; x < endX; x++) {
			cv::Scalar color = frame.at<cv::Vec3b>(cv::Point(x,y));

			blueSum += color.val[0];
			greenSum += color.val[1];
			redSum += color.val[2];
		}
	}

	int pixelAmount = this->m_dimensions.width * this->m_dimensions.height;

	double blueAverage = blueSum / pixelAmount;
	double greenAverage = greenSum / pixelAmount;
	double redAverage = redSum / pixelAmount;

	blueAverage += 30;
	greenAverage += 30;
	redAverage += 30;

	std::cout << "blue sum: " << blueAverage << std::endl;
	std::cout << "green sum: " << greenAverage << std::endl;
	std::cout << "red sum: " << redAverage << std::endl;
	std::cout << "pixelAmount: " << pixelAmount << std::endl;


	this->m_lastCalculatedAverageColor = cv::Scalar(blueAverage, greenAverage, redAverage);

	return this->m_lastCalculatedAverageColor;
}





