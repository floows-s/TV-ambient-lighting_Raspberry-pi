#pragma once

#include <Zone.h>
#include <opencv2/core.hpp>


class ZoneManager
{
public:
	// Constructor & destructor
	ZoneManager(unsigned int frameWidth, unsigned int frameHeight, LEDCounts LEDCounts);

	// Methods
	void calculate(cv::Mat& frame);
	void draw(cv::Mat& frame);

	// Getters & setters
	std::vector<Zone> getZones() { return m_zones; }
	unsigned int getFrameWidth() { return m_frameWidth; }
	unsigned int getFrameHeight() { return m_frameHeight; }

private:
	void updateZoneDimension(unsigned int frameWidth, unsigned int frameHeight);
	void calculateVerticalZone (unsigned int frameWidth, unsigned int frameHeight);

	unsigned int m_frameWidth;
	unsigned int m_frameHeight;

	LEDCounts m_LEDCounts;

	std::vector<Zone> m_zones;

};

struct LEDCounts {
	unsigned int top;
	unsigned int bottom;
	unsigned int left;
	unsigned int right;
};

