#pragma once
#include <map>

#include "Zone.h"
#include "LEDCounts.h"

#include <opencv2/core.hpp>


class ZoneManager
{
public:
	// Constructor
	ZoneManager(Dimensions frameDimensions, LEDCounts LEDCounts);

	// Methods
	void calculate(cv::Mat& frame);
	void draw(cv::Mat& frame, bool includeAverageColor = false);

	// Getters & setters
	std::map<ZoneSide, std::vector<Zone>> getZones(ZoneSide side) { return m_zones; }
	std::vector<Zone> getZonesBySide(ZoneSide side) { return m_zones[side]; }

	unsigned int getFrameWidth() { return m_frameDimensions.width; }
	unsigned int getFrameHeight() { return m_frameDimensions.height; }

private:
	void updateZoneDimension();
	Dimensions calculateVerticalZoneDimensions (int LEDCount);
	Dimensions calculateHorizontalZoneDimensions (int LEDCount);

	Dimensions m_frameDimensions;
	LEDCounts m_LEDCounts;

	std::map<ZoneSide, std::vector<Zone>> m_zones = { // Init with empty values
		{ ZoneSide::TOP, {}},
		{ ZoneSide::BOTTOM, {}},
		{ ZoneSide::LEFT, {}},
		{ ZoneSide::RIGHT, {}}
	};
};

enum ZoneSide {
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};

