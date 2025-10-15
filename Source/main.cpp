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

LEDCounts dummyLedCounts = { .top = 10, .bottom = 5, .left = 10, .right = 10 };
bool handleCaptureCard(cv::VideoCapture& vCap, cv::Mat& frame);

int main() {
	cv::VideoCapture vCap(0, cv::CAP_ANY);
	cv::Mat frame;

	// Init test window
	std::string windowName = "TV ambient lighting (Raspberry pi)";
	cv::namedWindow(windowName);

	// Start-up loop
	std::cout << "Entering start-up loop. Waiting for capture card signal..." << std::endl;
	while (!handleCaptureCard(vCap, frame)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50)); // <- do not overload the thread and CPU unnecessarily
	}
	std::cout << "Capture card signal recieved!" << std::endl;

	// Init manager and create zones for calculating the average color
	ZoneManager zoneManager(Dimensions(frame.cols, frame.rows), dummyLedCounts);

	// Main loop
	bool running = true;
	std::cout << "Entering main loop..." << std::endl;
	while (running) {
		// Get frame from capture card
		if (!handleCaptureCard(vCap, frame)) {
			std::cout << "Can't get frame from capture card, retrying..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(50)); // <- do not overload the thread and CPU unnecessarily
			continue;
		}

		std::cout << "FPS: " << vCap.get(cv::CAP_PROP_FPS) << std::endl;


		// Calculate averages in zones
		zoneManager.calculateAverages(frame);

		// Draw for debugging
		zoneManager.draw(frame, true);

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
		vCap.open(0, cv::CAP_ANY);
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