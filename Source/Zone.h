#pragma once
#include "Dimensions.h"
#include <opencv2/core.hpp>

class Zone
{

public:
	// Constructor
	Zone(Dimensions dimensions, cv::Scalar drawingColor = cv::Scalar(0, 0, 255));

	// Methods
	cv::Scalar calculate(cv::Mat& frame);
	void draw(cv::Mat& frame);

	// Getters & setters
	unsigned int getWidth() { return m_dimensions.width; }
	void setWidth(unsigned int value) { m_dimensions.width = value; }

	unsigned int getHeight() { return m_dimensions.height; }
	void setHeight(unsigned int value) { m_dimensions.height = value; }

	Dimensions getDimensions(){ return m_dimensions; }
	Dimensions setDimensions(Dimensions value){ m_dimensions = value; }

	cv::Scalar getLastCalculatedAverageColor() { return m_lastCalculatedAverageColor; }
	cv::Point getOrigin() { return m_origin; }
	
private:
	Dimensions m_dimensions;
	
	cv::Scalar m_lastCalculatedAverageColor;
	cv::Scalar m_drawingColor;
	cv::Point m_origin; 
};

