#include "Zone.h"
#include "Dimensions.h"

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

Zone::Zone(Dimensions dimensions, cv::Point origin, cv::Vec3b borderColor)
	: m_dimensions(dimensions), m_borderColor(borderColor), m_origin(origin){ };


/// <summary>
/// Draws a rectangle of the zones area on the given frame.
/// This gives you a visual representation of the zone. 
/// Could be used for debugging.
/// </summary>
/// <param name="frame">Frame drawn on</param>
/// <param name="includeAverageColor">If set to true it will fill the rectangle with the last calculated average color</param>
void Zone::draw(cv::Mat& frame, bool includeAverageColor) const {

	// Average color
	if (includeAverageColor) {
		cv::rectangle(
			frame,
			this->m_origin,
			cv::Point(
				this->m_origin.x + this->m_dimensions.width,
				this->m_origin.y + this->m_dimensions.height
			),
			m_lastCalculatedAverageColor,
			-1 // <- -1 == fill rectangle
		);
	}

	// Border
	int borderBrushThickness = 3;
	cv::rectangle(
		frame,
		this->m_origin, 
		cv::Point(
			this->m_origin.x + this->m_dimensions.width,
			this->m_origin.y + this->m_dimensions.height
		),
		m_borderColor,
		borderBrushThickness
	);
}


/// <summary>
/// Calculates and returns the average color inside the zone of the given frame.
/// It also sets the lastCalculatedArageColor inside the zone.
/// </summary>
/// <param name="frame">Frame to calculate average on</param>
/// <returns>Calculated average color</returns>
cv::Vec3b Zone::calculateAverage(cv::Mat& frame) {
	// Note: OpenCV orders color in BGR
	int blueSum = 0;
	int greenSum = 0;
	int redSum = 0;

	// Set region that needs to looped over (vars for readability)
	int beginY = this->m_origin.y;
	int endY = this->m_origin.y + this->m_dimensions.height;

	int beginX = this->m_origin.x;
	int endX = this->m_origin.x + this->m_dimensions.width;

	// Loop over region
	for (int y = beginY; y < endY; y++) {
		for (int x = beginX; x < endX; x++) {
			cv::Vec3b color = frame.at<cv::Vec3b>(cv::Point(x,y));

			// Collect BGR colors
			blueSum += color.val[0];
			greenSum += color.val[1];
			redSum += color.val[2];
		}
	}

	int totalPixelAmount = this->m_dimensions.width * this->m_dimensions.height;

	// Calculate averages
	double blueAverage = blueSum / totalPixelAmount;
	double greenAverage = greenSum / totalPixelAmount;
	double redAverage = redSum / totalPixelAmount;

	this->m_lastCalculatedAverageColor[0] = blueAverage;
	this->m_lastCalculatedAverageColor[1] = greenAverage;
	this->m_lastCalculatedAverageColor[2] = redAverage;

	return this->m_lastCalculatedAverageColor;
}





