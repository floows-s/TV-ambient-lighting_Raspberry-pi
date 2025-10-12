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
	// TODO: put the m_zones = this->generateZones();

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
		cv::Scalar redColor(0, 0, 255);
		cv::Scalar blueColor(255, 0, 0);
		cv::Scalar greenColor(0, 255, 0);
		cv::Scalar yellowColor(0, 255, 255);

		// Set LEDCount and calculate dimension based on ZoneSide
		int LEDCount;
		Dimensions dimensions;
		cv::Scalar drawingColor;
		switch (side) {
		case ZoneSide::TOP:
			drawingColor = redColor;
			LEDCount = this->m_LEDCounts.top;
			dimensions = this->calculateHorizontalZoneDimensions(LEDCount);
			break;
		case ZoneSide::BOTTOM:
			drawingColor = blueColor;
			LEDCount = this->m_LEDCounts.bottom;
			dimensions = this->calculateHorizontalZoneDimensions(LEDCount);
			break;
		case ZoneSide::LEFT:
			drawingColor = greenColor;
			LEDCount = this->m_LEDCounts.left;
			dimensions = this->calculateVerticalZoneDimensions(LEDCount);
			break;
		case ZoneSide::RIGHT:
			drawingColor = yellowColor;
			LEDCount = this->m_LEDCounts.right;
			dimensions = this->calculateVerticalZoneDimensions(LEDCount);
			break;
		default:
			std::cout << "Error: a unknown ZoneSide is given while calculating the zone dimensions." << std::endl;
			continue;
		}

		// Create zones
		std::vector<Zone> zones;
		for (int i = 0; i < LEDCount; i++) {
			
			// Calculate origin point
			cv::Point originPoint;
			switch (side) {
			case ZoneSide::TOP:
				originPoint = cv::Point(dimensions.width * i, 0);
				break;
			case ZoneSide::BOTTOM:
				originPoint = cv::Point(dimensions.width * i, m_frameDimensions.height - dimensions.height);
				break;
			case ZoneSide::LEFT:
				originPoint = cv::Point(m_frameDimensions.width - dimensions.width, dimensions.height * i);
				break;
			case ZoneSide::RIGHT:
				originPoint = cv::Point(0, dimensions.height * i);
				break;
			default:
				std::cout << "Error: a unknown ZoneSide is given while calculating the zone origin point." << std::endl;
				continue;
			}

			Zone zone(dimensions, originPoint, drawingColor);
			zones.push_back(zone);
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
			zone.draw(frame);
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
	std::cout << "Update zones!" << std::endl;

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
			break;
		case ZoneSide::RIGHT:
			dimensions = this->calculateVerticalZoneDimensions(
				this->m_LEDCounts.right
			);
			break;
		default:
			dimensions = Dimensions{ .width = -1, .height = -1 };
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
	int frameWidth = m_frameDimensions.width;
	int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = (int)std::ceil(frameWidth * ZONE_THICKNES_TO_SCREEN_RATIO),
		.height = (int)std::ceil(frameHeight / LEDCount)
	};

	return zoneDimensions;
}

Dimensions ZoneManager::calculateHorizontalZoneDimensions(int LEDCount) {
	int frameWidth = m_frameDimensions.width;
	int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = (int)std::ceil(frameWidth / LEDCount),
		.height = (int)std::ceil(frameWidth * ZONE_THICKNES_TO_SCREEN_RATIO)
	};

	return zoneDimensions;
}
