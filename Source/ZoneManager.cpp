#include <iostream>
#include <cmath>
#include <vector>

#include "ZoneManager.h"
#include "const_config.h"
#include "Dimensions.h"
#include "LEDCounts.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

ZoneManager::ZoneManager(LEDCounts LEDCounts, Dimensions frameDimensions)
	: m_LEDCounts(LEDCounts), m_frameDimensions(frameDimensions), m_zones(this->generateZones()){ }

/// <summary>
/// Generate zones based on the m_LEDCounts and puts it in a map with the associated ZoneSide.
/// </summary>
/// <returns>A map of ZoneSides with the associated generated zones.</returns>
std::map<ZoneSide, std::vector<Zone>> ZoneManager::generateZones() const {
	std::cout << "Generating zones..." << std::endl;
	std::map<ZoneSide, std::vector<Zone>> zoneMap;

	ZoneSide sidesToInclude[] = { 
		ZoneSide::TOP, ZoneSide::BOTTOM, 
		ZoneSide::LEFT, ZoneSide::RIGHT 
	};

	for (ZoneSide side : sidesToInclude) {
		const cv::Vec3b redColor(0, 0, 255);
		const cv::Vec3b blueColor(255, 0, 0);
		const cv::Vec3b greenColor(0, 255, 0);
		const cv::Vec3b yellowColor(0, 255, 255);

		// Set LEDCount and borderColor and calculate dimension based on ZoneSide
		int LEDCount;
		Dimensions dimensions;
		cv::Vec3b borderColor;

		switch (side) {
		case ZoneSide::TOP:
			borderColor = redColor;
			LEDCount = this->m_LEDCounts.top;
			dimensions = this->calculateHorizontalZoneDimensions(LEDCount);
			break;
			
		case ZoneSide::BOTTOM:
			borderColor = blueColor;
			LEDCount = this->m_LEDCounts.bottom;
			dimensions = this->calculateHorizontalZoneDimensions(LEDCount);
			break;

		case ZoneSide::LEFT:
			borderColor = greenColor;
			LEDCount = this->m_LEDCounts.left;
			dimensions = this->calculateVerticalZoneDimensions(LEDCount);
			break;

		case ZoneSide::RIGHT:
			borderColor = yellowColor;
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
				originPoint = cv::Point(0, dimensions.height * i);
				break;

			case ZoneSide::RIGHT:
				originPoint = cv::Point(m_frameDimensions.width - dimensions.width, dimensions.height * i);
				break;
				
			default:
				std::cout << "Error: a unknown ZoneSide is given while calculating the zone origin point." << std::endl;
				continue;
			}

			// Create zone and add to array
			Zone zone(dimensions, originPoint, borderColor);
			zones.push_back(zone);
		}

		// Add generated zones of a side to the zoneMap
		zoneMap.insert({ side, zones });
	}

	return zoneMap;
}

/// <summary>
/// Runs the draw method on all zones.
/// </summary>
/// <param name="frame">Frame drawn on</param>
/// <param name="includeAverageColor">If set to true it will fill the rectangle with the last calculated average color of the zone</param>
void ZoneManager::draw(const cv::Mat& frame, bool includeAverageColor) {
	if (!m_frameDimensions.equals(frame)) {
		m_frameDimensions.width = frame.cols;
		m_frameDimensions.height = frame.rows;

		this->updateZoneDimension();
	}

	for (const auto& [side, zones] : this->m_zones) {
		for (const Zone& zone : zones) {
			zone.draw(frame, includeAverageColor);
		}
	}
}

/// <summary>
/// Runs the calculateAverage method on all zones.
/// </summary>
/// <param name="frame">Frame to calculate averages on</param>
void ZoneManager::calculateAverages(const cv::Mat& frame) {
	if (!m_frameDimensions.equals(frame)) {
		m_frameDimensions.width = frame.cols;
		m_frameDimensions.height = frame.rows;

		this->updateZoneDimension();
	}

	for (auto& [side, zones] : this->m_zones) {
		for (Zone& zone : zones) {
			zone.calculateAverage(frame);
		}
	}
}

void ZoneManager::updateZoneDimension() {
	std::cout << "Updating zones dimensions..." << std::endl;

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
			std::cout << "Error: a unknown ZoneSide is given while updating the zone dimensions." << std::endl;
			continue;
		}


		// Double check! A assert can never hurt
		assert(dimensions.width > 0 || dimensions.height > 0);
		
		// Update the zone dimensions
		for (Zone& zone : zones) {
			zone.setDimensions(dimensions);
		}
	}
}

/// <summary>
/// Calculates the dimensions of a zone based on the frame dimensions and given LEDCount.
/// Should be used to calculate the zone dimensions of the right and left side.
/// </summary>
/// <param name="LEDCount"></param>
/// <returns></returns>
Dimensions ZoneManager::calculateVerticalZoneDimensions(int LEDCount) const {
	int frameWidth = m_frameDimensions.width;
	int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = (int)std::ceil(frameWidth * Config::ZONE_THICKNES_TO_SCREEN_RATIO),
		.height = (int)std::ceil(frameHeight / LEDCount)
	};

	return zoneDimensions;
}

/// <summary>
/// Calculates the dimensions of a zone based on the frame dimensions and given LEDCount.
/// Should be used to calculate the zone dimensions of the top and bottom side.
/// </summary>
/// <param name="LEDCount"></param>
/// <returns></returns>
Dimensions ZoneManager::calculateHorizontalZoneDimensions(int LEDCount) const {
	int frameWidth = m_frameDimensions.width;
	int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = (int)std::ceil(frameWidth / LEDCount),
		.height = (int)std::ceil(frameWidth * Config::ZONE_THICKNES_TO_SCREEN_RATIO)
	};

	return zoneDimensions;
}
