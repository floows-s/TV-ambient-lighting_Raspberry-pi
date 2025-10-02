#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>


std::string windowName = "TV ambient lighting (Raspberry pi)";

bool handleCaptureCard(cv::VideoCapture& vCap, cv::Mat& frame);

int main() {
	cv::VideoCapture vCap(0, cv::CAP_ANY);
	cv::Mat frame;

	cv::namedWindow(windowName);

	// Main loop
	bool running = true;
	while (running) {
		// Get frame from capture card
		if (!handleCaptureCard(vCap, frame)) {
			std::cout << "Can't get frame from capture card, retrying..." << std::endl;
			continue;
		}



		// Give frame to zones
		// Run calculations on zones
		// maybe have a zone manager do this...
		// display zone in window??
		
		cv::imshow(windowName, frame);
		cv::waitKey(1); // <- Is needed to handle OpenCV GUI events (I know its stupid, waitKey??)
		std::cout << "FPS: " << vCap.get(cv::CAP_PROP_FPS) << std::endl;
	}

	return 0;
}


/// <summary>
/// Read a frame from the given vCap and write it to the given frame.
/// </summary>
/// <returns>If it succeeded</returns>
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