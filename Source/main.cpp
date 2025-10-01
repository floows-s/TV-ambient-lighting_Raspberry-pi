#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include<windows.h>

std::string windowName = "TV ambient lighting (Raspberry pi)";

int main() {
	cv::VideoCapture vCap(0, cv::CAP_ANY);

	cv::namedWindow(windowName);

	cv::Mat frame;
	vCap.read(frame);

	// Main loop
	while (true) {
		// Check vCap
		// Note: vCap.isOpened() doenst check if the capture card is still connected...
		if (!vCap.isOpened()) {
			std::cout << "VideoCapture is not opened... Trying to reopen!" << std::endl;
			vCap.open(0, cv::CAP_ANY);
			continue;
		}

		// Read frame
		bool hasReadFrame = vCap.read(frame);
		if (!hasReadFrame) {
			std::cout << "Can't read frame... Skipping loop cycle!" << std::endl;
			vCap.release(); // <- Release so next cycle it will try to reconnect the vCap.
			continue;
		}

		if (frame.empty()) {
			std::cout << "Frame is empty... Skipping loop cycle!" << std::endl;
			continue;
		}



		// Give frame to zones
		// Run calculations on zones
		// maybe have a zone manager do this...
		// display zone in window??
		
		cv::imshow(windowName, frame);
		cv::waitKey(1); // TODO << look at this, maybe make loop interval
		std::cout << "FPS: " << vCap.get(cv::CAP_PROP_FPS) << std::endl;
	}
	
	
	//std::cout << "Hoi\n";
	//cv::namedWindow("Test");
	//cv::imshow("Test", image);
	//cv::waitKey(0);
	//cv::destroyAllWindows();

	return 0;
}