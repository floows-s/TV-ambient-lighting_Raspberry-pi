#pragma once
#include <map>

#include "Zone.h"
#include "LEDCounts.h"

#include <opencv2/core.hpp>

enum class ZoneSide {
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
	ZoneManager(LEDCounts LEDCounts, Dimensions frameDimensions = Dimensions(0,0));

	// Methods
	void calculateAverages(const cv::Mat& frame);
	void draw(const cv::Mat& frame, bool includeAverageColor = true);

	// Getters & setters
	const std::map<ZoneSide, std::vector<Zone>>& getZones() { return m_zones; }
	// TODO: change back to const
	std::vector<Zone>& getZonesBySide(ZoneSide side) { return m_zones[side]; }

	int getFrameWidth() const { return m_frameDimensions.width; }
	int getFrameHeight() const { return m_frameDimensions.height; }

private:
	// Methods
	std::map<ZoneSide, std::vector<Zone>> generateZones() const;
	void updateZoneDimension();
	Dimensions calculateVerticalZoneDimensions(int LEDCount) const;
	Dimensions calculateHorizontalZoneDimensions (int LEDCount) const;

	// Members
	Dimensions m_frameDimensions;
	LEDCounts m_LEDCounts;

	std::map<ZoneSide, std::vector<Zone>> m_zones;
};