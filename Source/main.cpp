#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include <ws2811.h>

#include "ZoneManager.h"
#include "LEDCounts.h"
#include "const_config.h"

ws2811_t ledStrip =
{
	.freq = Config::TARGET_FREQ,
	.dmanum = Config::DMA,
	.channel =
	{
		{
			.gpionum = Config::DATA_OUT_GPIO_PIN,
			.invert = 0,
			.count = Config::LED_COUNTS.all(),
			.strip_type = Config::STRIP_TYPE,
			.brightness = 128,
		}
	}
};


bool handleCaptureCard(cv::VideoCapture& vCap, cv::Mat& frame);
bool handleRenderLedStrip(ws2811_t& ledStrip, ZoneManager& zoneManager);
void setColorsOnLedStrip(ws2811_t& ledStrip, ZoneManager& zoneManager);
int BGRToWRGBHex(cv::Vec3b color);

int main() {
	ws2811_init(&ledStrip);

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
	ZoneManager zoneManager(Config::LED_COUNTS, Dimensions(frame.cols, frame.rows));

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

		// Set calculated colors and render led-strip
		handleRenderLedStrip(ledStrip, zoneManager);

		// Calculate incremental average loop time
		auto endTime = std::chrono::high_resolution_clock::now();
		float deltaTimeInMS = std::chrono::duration<float, std::milli>(endTime - startTime).count();
		averageLoopTimeMS += (deltaTimeInMS - averageLoopTimeMS) / loopCounter;
		std::cout << "Average loop time: " << averageLoopTimeMS << "MS" << std::endl;

		cv::imshow(windowName, frame);
		char pressedKey = cv::waitKey(1); // <- Is needed to handle OpenCV GUI events (like imshow) (I know its stupid, waitKey??)
		if (pressedKey == 'q' || pressedKey == 'Q') running = false;
	}

	std::cout << "Out of main loop" << std::endl;
	std::cout << "Releasing VideoCapture..." << std::endl;
	vCap.release();

	// TODO hook into shutdown or SIGKILL and make led strip off
	std::cout << "Releasing led-strip..." << std::endl;
	ws2811_fini(&ledStrip);

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

/// <summary>
/// Sets the colors on the LED strip based on the ZoneManager's zones last calculated average's and then renders the changes to the physical strip.
/// </summary>
/// <param name="ledStrip">A reference to the ws2811_t ledStrip to be updated and rendered.</param>
/// <param name="zoneManager">A reference to the ZoneManager.</param>
bool handleRenderLedStrip(ws2811_t& ledStrip, ZoneManager& zoneManager) {
	setColorsOnLedStrip(ledStrip, zoneManager);
	ws2811_return_t result = ws2811_render(&ledStrip);
	if (result != ws2811_return_t::WS2811_SUCCESS) {
		std::cout << "Can't render led-strip! Eror code: " << result << std::endl;
		return false;
	}

	return true;
}

/// <summary>
/// Sets the colors in the ledStrip.
/// </summary>
/// <param name="ledStrip">A reference to the ws2811_t ledStrip.</param>
/// <param name="zoneManager">A refrence of the zoneManager's.</param>
void setColorsOnLedStrip(ws2811_t& ledStrip, ZoneManager& zoneManager) {
	/*
	* Note: 
	* due to lack of motivation to finish this project properly
	* because i have bigger projects i want to work, the flow direction is static...
	* 
	* If anyone looking at this and has nothing to do, and wants to implement this i would thank you!
	* I would suggest doing something with the ZoneSide enum in a array as a var in the const_config file.
	* 
	* For now the flow is (looking infront of the screen): 
	*   v ---------- <
    *   |            |
    *   |            | START (Right bottom)
    *   > ----------
	*			 END (Right bottom)
	*/

	ZoneSide zoneOrder[] = { ZoneSide::LEFT, ZoneSide::TOP, ZoneSide::RIGHT, ZoneSide::BOTTOM };

	int ledsStartPosition = 0;
	for (ZoneSide zoneSide : zoneOrder) {
		const std::vector<Zone>& zones = zoneManager.getZonesBySide(zoneSide);

		// If the current ZoneSide is top of right -> loop through zones in reverse
		bool reverseLoop = (zoneSide == ZoneSide::TOP || zoneSide == ZoneSide::RIGHT); 
		int start = (reverseLoop ? zones.size() - 1 : 0);
		int end = (reverseLoop ? 0 : zones.size() - 1);
		int step = (reverseLoop ? -1 : 1);

		for (int i = start; i != end; i += step) {
			ledStrip.channel[0].leds[ledsStartPosition + i] = BGRToWRGBHex(
				zones[i].getLastCalculatedAverageColor()
			);
		}

		int ledsStartPosition = (zones.size() - 1);
	}
}

// TODO UPDATE SUMARRY!
/// <summary>
/// Converts a BGR value to a RGB hex value.
/// Note: White is also included but set to 0.
/// </summary>
/// <param name="color">The color to be converted.</param>
/// <returns>The WRGB hex value.</returns>
int BGRToWRGBHex(cv::Vec3b color) {
	//		  White			Blue 			   Green		   Red
	return ((0 << 24) | (color[0] << 16) | (color[1] << 8) | color[2]);
}