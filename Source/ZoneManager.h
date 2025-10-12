#pragma once
#include <map>

#include "Zone.h"
#include "LEDCounts.h"

#include <opencv2/core.hpp>

enum ZoneSide {
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};


class ZoneManager
{
public:
	// Constructor
	ZoneManager();
	ZoneManager(Dimensions frameDimensions, LEDCounts LEDCounts);

	// Methods
	void calculate(cv::Mat& frame);
	void draw(cv::Mat& frame, bool includeAverageColor = false);

	// Getters & setters
	std::map<ZoneSide, std::vector<Zone>> getZones() { return m_zones; }
	std::vector<Zone> getZonesBySide(ZoneSide side) { return m_zones[side]; }

	unsigned int getFrameWidth() { return m_frameDimensions.width; }
	unsigned int getFrameHeight() { return m_frameDimensions.height; }

private:
	std::map<ZoneSide, std::vector<Zone>> generateZones();
	void updateZoneDimension();
	Dimensions calculateVerticalZoneDimensions (int LEDCount);
	Dimensions calculateHorizontalZoneDimensions (int LEDCount);

	Dimensions m_frameDimensions;
	LEDCounts m_LEDCounts;

	std::map<ZoneSide, std::vector<Zone>> m_zones;
};