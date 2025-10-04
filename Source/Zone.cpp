#include "Zone.h"
#include "Dimensions.h"

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

Zone::Zone(Dimensions dimensions, cv::Scalar drawingColor)
	: m_dimensions(dimensions), m_drawingColor(drawingColor){ }

void Zone::draw(cv::Mat& frame) {
	cv::rectangle(
		frame,
		this->m_origin,
		cv::Point(
			this->m_origin.x + this->m_dimensions.width,
			this->m_origin.y + this->m_dimensions.height
		),
		m_drawingColor,
		3
	);
}

cv::Scalar Zone::calculate(cv::Mat& frame) {
	std::cout << "Zone::calculate: In progress!" << std::endl;

	return cv::Scalar();
}





