#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "ZoneManager.h"
#include "LEDCounts.h"
#include "const_config.h"

double clock_tToMilliseconds(clock_t ticks) {
	return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

// TODO: edge case, when there are less pixels then LEDS, what to do...
const LEDCounts dummyLedCounts = { .top = 10, .bottom = 10, .left = 10, .right = 10 };
bool handleCaptureCard(cv::VideoCapture& vCap, cv::Mat& frame);

int main() {
	cv::VideoCapture vCap(Config::VIDEO_CAPTURE_INDEX, cv::CAP_ANY);
	cv::Mat frame;

	// Init test window
	const std::string windowName = "TV ambient lighting (Raspberry pi)";
	cv::namedWindow(windowName);

	// Start-up loop
	std::cout << "Entering start-up loop. Waiting for capture card signal..." << std::endl;
	while (!handleCaptureCard(vCap, frame)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50)); // <- do not overload the thread and CPU unnecessarily
	}
	std::cout << "Capture card signal recieved!" << std::endl;

	// Init manager and create zones for calculating the average color
	ZoneManager zoneManager(dummyLedCounts, Dimensions(frame.cols, frame.rows));

	// Main loop
	std::cout << "Entering main loop..." << std::endl;
	uint64_t loopCounter = 0; // Dont worry this will only overflow in about 51 milion years ;)
	double averageLoopTimeMS = 0.0;

	bool running = true;
	while (running) {
		loopCounter++;
		auto startTime = std::chrono::high_resolution_clock::now();

		// Get frame from capture card
		if (!handleCaptureCard(vCap, frame)) {
			std::cout << "Can't get frame from capture card, retrying..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(50)); // <- do not overload the thread and CPU unnecessarily
			continue;
		}

		// Calculate averages in zones
		zoneManager.calculateAverages(frame);

		// Draw for debugging
		zoneManager.draw(frame, true);

		// Calculate incremental average
		auto endTime = std::chrono::high_resolution_clock::now();
		float deltaTimeInMS = std::chrono::duration<float, std::milli>(endTime - startTime).count();
		averageLoopTimeMS += (deltaTimeInMS - averageLoopTimeMS) / loopCounter;
		std::cout << "Average loop time: " << averageLoopTimeMS << "MS" << std::endl;

		cv::imshow(windowName, frame);
		char pressedKey = cv::waitKey(1); // <- Is needed to handle OpenCV GUI events (I know its stupid, waitKey??)
		if (pressedKey == 'q' || pressedKey == 'Q') running = false;
	}

	std::cout << "Out of main loop" << std::endl;
	std::cout << "Releasing VideoCapture..." << std::endl;
	vCap.release();
	return 0;
}


/// <summary>
/// Read a frame from the given vCap and write it to the given frame.
/// </summary>
/// <returns>If it could succesfully get a non-empty frame from the vCap</returns>
bool handleCaptureCard(cv::VideoCapture& vCap, cv::Mat& frame) {
	// Check vCap
	// Note: vCap.isOpened() doenst check if the capture card is still connected...
	if (!vCap.isOpened()) {
		std::cout << "VideoCapture is not opened... Trying to reopen!" << std::endl;
		vCap.open(Config::VIDEO_CAPTURE_INDEX, cv::CAP_ANY);
		return false;
	}

	// Read frame
	bool hasReadFrame = vCap.read(frame);
	if (!hasReadFrame) {
		std::cout << "Can't read frame... Releasing VideoCapture so it will fully reconnect!" << std::endl;
		vCap.release(); // <- Release so next cycle it will try to reconnect the vCap.
		return false;
	}

	if (frame.empty()) {
		std::cout << "Frame is empty... Skipping loop cycle!" << std::endl;
		return false;
	}

	return true;
}