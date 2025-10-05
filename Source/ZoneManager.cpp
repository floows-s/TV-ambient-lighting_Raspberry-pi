#include <iostream>
#include <cmath>

#include "ZoneManager.h"
#include "const_config.h"
#include "Dimensions.h"
#include "LEDCounts.h"

#include <opencv2/core.hpp>

ZoneManager::ZoneManager(Dimensions frameDimensions, LEDCounts LEDCounts)
	: m_frameDimensions(frameDimensions), m_LEDCounts(LEDCounts) {
	// TODO: init m_zones

}

void ZoneManager::draw(cv::Mat& frame, bool includeAverageColor = false) {
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
		Dimensions zoneDimensions;

		if (side == ZoneSide::TOP || side == ZoneSide::BOTTOM) {
			zoneDimensions = this->calculateVerticalZoneDimensions(
				(side == ZoneSide::TOP ? this->m_LEDCounts.top : this->m_LEDCounts.bottom)
			);
		}

		if (side == ZoneSide::LEFT || side == ZoneSide::RIGHT) {
			zoneDimensions = this->calculateVerticalZoneDimensions(
				(side == ZoneSide::TOP ? this->m_LEDCounts.top : this->m_LEDCounts.bottom)
			);
		}

		assert(zoneDimensions.width > 0 || zoneDimensions.height > 0);
		

		for (Zone zone : zones) {
			zone.setDimensions(zoneDimensions);
		}
	}
}


Dimensions ZoneManager::calculateVerticalZoneDimensions(int LEDCount) {
	unsigned int frameWidth = m_frameDimensions.width;
	unsigned int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = std::floor(frameWidth * ZONE_THICKNES_TO_SCREEN_RATIO),
		.height = std::floor(frameHeight / LEDCount)
	};

	return zoneDimensions;
}

Dimensions ZoneManager::calculateHorizontalZoneDimensions(int LEDCount) {
	unsigned int frameWidth = m_frameDimensions.width;
	unsigned int frameHeight = m_frameDimensions.height;

	Dimensions zoneDimensions = {
		.width = std::floor(frameHeight / LEDCount),
		.height = std::floor(frameWidth * ZONE_THICKNES_TO_SCREEN_RATIO)
	};

	return zoneDimensions;
}


