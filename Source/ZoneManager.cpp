#include <iostream>
#include "ZoneManager.h"
#include <opencv2/core.hpp>

ZoneManager::ZoneManager(unsigned int frameWidth, unsigned int frameHeight, LEDCounts LEDCounts)
	: m_frameWidth(frameWidth), m_frameHeight(frameHeight), m_LEDCounts(LEDCounts){

}

void ZoneManager::draw(cv::Mat& frame) {
	for (Zone zone : this->getZones()) {
		zone.draw(frame);
	}
}

void ZoneManager::calculate(cv::Mat& frame) {
	std::cout << "ZoneManager::calculate: In progress!" << std::endl;


}




void ZoneManager::setZoneDimension(unsigned int frameWidth, unsigned int frameHeight) {

	for (Zone zone : this->getZones()) {

	}
}
