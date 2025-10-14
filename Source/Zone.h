#pragma once
#include "Dimensions.h"
#include <opencv2/core.hpp>

class Zone
{

public:
	// Constructor
	Zone() = default;
	Zone(Dimensions dimensions, cv::Point origin, cv::Vec3b borderColor = cv::Vec3b(0, 0, 255));

	// Methods
	cv::Vec3b calculateAverage(cv::Mat& frame);
	void draw(cv::Mat& frame, bool includeAverageColor = true) const;

	// Getters & setters
	unsigned int getWidth() { return m_dimensions.width; }
	void setWidth(unsigned int value) { m_dimensions.width = value; }

	unsigned int getHeight() { return m_dimensions.height; }
	void setHeight(unsigned int value) { m_dimensions.height = value; }

	Dimensions getDimensions(){ return m_dimensions; }
	void setDimensions(Dimensions value){ m_dimensions = value; }

	cv::Vec3b getLastCalculatedAverageColor() { return m_lastCalculatedAverageColor; }
	cv::Point getOrigin() { return m_origin; }
	
private:
	// Members
	Dimensions m_dimensions;
	
	cv::Vec3b m_lastCalculatedAverageColor;
	cv::Vec3b m_borderColor;
	cv::Point m_origin; 
};
