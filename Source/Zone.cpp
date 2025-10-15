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
void Zone::draw(const cv::Mat& frame, bool includeAverageColor) const {

	cv::Point endOfRect(
		this->m_origin.x + this->m_dimensions.width,
		this->m_origin.y + this->m_dimensions.height
	);

	// Average color
	if (includeAverageColor) {
		cv::rectangle(
			frame,
			this->m_origin,
			endOfRect,
			m_lastCalculatedAverageColor,
			-1 // <- -1 == fill rectangle
		);
	}

	// Border
	int borderBrushThickness = 3;
	cv::rectangle(
		frame,
		this->m_origin, 
		endOfRect,
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
cv::Vec3b Zone::calculateAverage(const cv::Mat& frame) {
	// Set region of interest (ROI)
	cv::Rect ROIRect(
		m_origin.x,
		m_origin.y,
		this->m_dimensions.width, 
		this->m_dimensions.height
	);

	// Crop frame to ROI
	cv::Mat croppedImageROI = frame(ROIRect);

	// Calculate average
	cv::Scalar averageColor = cv::mean(croppedImageROI); // Note: cv::mean is prob a highly optimized function so its beter to use that...

	// Set last calculated average
	this->m_lastCalculatedAverageColor[0] = averageColor[0]; // Blue
	this->m_lastCalculatedAverageColor[1] = averageColor[1]; // Green
	this->m_lastCalculatedAverageColor[2] = averageColor[2]; // Red

	return this->m_lastCalculatedAverageColor;
}





