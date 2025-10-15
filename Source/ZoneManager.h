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

/// <summary>
/// This class generates and manages a set of zones.
/// The zones are generated based on the given frameDimensions and LEDCounts.
/// When the sizes of a given frame changes the zones will also change size.
/// </summary>
class ZoneManager
{
public:
	// Constructor
	ZoneManager(Dimensions frameDimensions, LEDCounts LEDCounts);

	// Methods
	void calculateAverages(cv::Mat& frame);
	void draw(cv::Mat& frame, bool includeAverageColor = true);

	// Getters & setters
	std::map<ZoneSide, std::vector<Zone>> getZones() { return m_zones; }
	std::vector<Zone> getZonesBySide(ZoneSide side) { return m_zones[side]; }

	unsigned int getFrameWidth() { return m_frameDimensions.width; }
	unsigned int getFrameHeight() { return m_frameDimensions.height; }

private:
	// Methods
	std::map<ZoneSide, std::vector<Zone>> generateZones();
	void updateZoneDimension();
	Dimensions calculateVerticalZoneDimensions(int LEDCount) const;
	Dimensions calculateHorizontalZoneDimensions (int LEDCount) const;

	// Members
	Dimensions m_frameDimensions;
	LEDCounts m_LEDCounts;

	std::map<ZoneSide, std::vector<Zone>> m_zones;
};