#include <iostream>
#include <cmath>
#include <vector>

#include "ZoneManager.h"
#include "const_config.h"
#include "Dimensions.h"
#include "LEDCounts.h"

#include <opencv2/core.hpp>

ZoneManager::ZoneManager() = default;
ZoneManager::ZoneManager(Dimensions frameDimensions, LEDCounts LEDCounts)
	: m_frameDimensions(frameDimensions), m_LEDCounts(LEDCounts), m_zones(this->generateZones()){
	

}

/// <summary>
/// Generate zones based on the m_LEDCounts and puts it in a map with the associated ZoneSide.
/// </summary>
/// <returns>A map of ZoneSides with the associated generated zones.</returns>
std::map<ZoneSide, std::vector<Zone>> ZoneManager::generateZones() {
	std::map<ZoneSide, std::vector<Zone>> zoneMap;

	ZoneSide sidesToInclude[] = { 
		ZoneSide::TOP, ZoneSide::BOTTOM, 
		ZoneSide::LEFT, ZoneSide::RIGHT 
	};

	for (ZoneSide side : sidesToInclude) {

		// Set LEDCount and calculate dimension based on ZoneSide
		int LEDCount;
		Dimensions dimensions;
		switch (side) {
		case ZoneSide::TOP:
			LEDCount = this->m_LEDCounts.top;
			dimensions = this->calculateHorizontalZoneDimensions(LEDCount);
			break;
		case ZoneSide::BOTTOM:
			LEDCount = this->m_LEDCounts.bottom;
			dimensions = this->calculateHorizontalZoneDimensions(LEDCount);
			break;
		case ZoneSide::LEFT:
			LEDCount = this->m_LEDCounts.left;
			dimensions = this->calculateVerticalZoneDimensions(LEDCount);
		case ZoneSide::RIGHT:
			LEDCount = this->m_LEDCounts.right;
			dimensions = this->calculateVerticalZoneDimensions(LEDCount);
			break;
		}

		// Create zones
		std::vector<Zone> zones;
		for (int i = 0; i < LEDCount; i++) {
			Zone zone(dimensions);
			zones.push_back(
				zone
			);
		}

		zoneMap.insert({ side, zones });
	}

	return zoneMap;
}

void ZoneManager::draw(cv::Mat& frame, bool includeAverageColor) {
	if (!m_frameDimensions.equals(frame)) {
		m_frameDimensions.width = frame.cols;
		m_frameDimensions.height = frame.rows;

		this->updateZoneDimension();
	}

	for (auto & [side, zones] : this->m_zones) {
		for (Zone zone : zones) {
			this->draw(frame);
		}
	}
}

void ZoneManager::calculate(cv::Mat& frame) {
	std::cout << "ZoneManager::calculate: In progress!" << std::endl;

	//if (!m_frameDimensions.equals(frame)) {
	//	m_frameDimensions.width = frame.cols;
	//	m_frameDimensions.height = frame.rows;

	//	this->updateZoneDimension();
	//}

	//for (auto& [side, zones] : this->m_zones) {
	//	for (Zone zone : zones) {
	//		this->calculate(frame);
	//	}
	//}
}

void ZoneManager::updateZoneDimension() {
	for (auto& [side, zones] : this->m_zones) {
		// Calculate dimensions based on ZoneSide
		Dimensions dimensions;
		switch (side) {
		case ZoneSide::TOP:
			dimensions = this->calculateHorizontalZoneDimensions(
				this->m_LEDCounts.top
			);
			break;
		case ZoneSide::BOTTOM:
			dimensions = this->calculateHorizontalZoneDimensions(
				this->m_LEDCounts.bottom
			);
			break;
		case ZoneSide::LEFT:
			dimensions = this->calculateVerticalZoneDimensions(
				this->m_LEDCounts.left
			);
		case ZoneSide::RIGHT:
			dimensions = this->calculateVerticalZoneDimensions(
				this->m_LEDCounts.right
			);
			break;
		}


		// Double check! A assert can never hurt
		assert(dimensions.width > 0 || dimensions.height > 0);
		
		// Update the zone dimensions
		for (Zone zone : zones) {
			zone.setDimensions(dimensions);
		}
	}
}


Dimensions ZoneManager::calculateVerticalZoneDimensions(int LEDCount) {
	unsigned int frameWidth = m_frameDimensions.width;
	unsigned int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = (unsigned int) std::floor(frameWidth * ZONE_THICKNES_TO_SCREEN_RATIO),
		.height = (unsigned int) std::floor(frameHeight / LEDCount)
	};

	return zoneDimensions;
}

Dimensions ZoneManager::calculateHorizontalZoneDimensions(int LEDCount) {
	unsigned int frameWidth = m_frameDimensions.width;
	unsigned int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = (unsigned int) std::floor(frameHeight / LEDCount),
		.height = (unsigned int) std::floor(frameWidth * ZONE_THICKNES_TO_SCREEN_RATIO)
	};

	return zoneDimensions;
}
