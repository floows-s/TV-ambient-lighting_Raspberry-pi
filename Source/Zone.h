#pragma once
#include "Dimensions.h"
#include <opencv2/core.hpp>

class Zone
{

public:
	// Constructor
	Zone(Dimensions dimensions, cv::Point origin, cv::Vec3b borderColor = cv::Vec3b(0, 0, 255));

	// Methods
	cv::Vec3b calculateAverage(const cv::Mat& frame);
	void draw(const cv::Mat& frame, bool includeAverageColor = true) const;

	// Getters & setters
	const int getWidth() const { return m_dimensions.width; }
	void setWidth(unsigned int value) { m_dimensions.width = value; }

	const int getHeight() const { return m_dimensions.height; }
	void setHeight(unsigned int value) { m_dimensions.height = value; }

	const Dimensions& getDimensions() const { return m_dimensions; }
	void setDimensions(Dimensions value){ m_dimensions = value; }

	const cv::Vec3b& getLastCalculatedAverageColor() const { return m_lastCalculatedAverageColor; }
	const cv::Point& getOrigin() const { return m_origin; }
	
private:
	// Members
	Dimensions m_dimensions;
	
	cv::Vec3b m_lastCalculatedAverageColor;
	cv::Vec3b m_borderColor;
	cv::Point m_origin; 
};
