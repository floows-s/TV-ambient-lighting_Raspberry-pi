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
			.brightness = 50,
		}
	}
};


bool handleCaptureCard(cv::VideoCapture& vCap, cv::Mat& frame);
bool handleRenderLedStrip(ws2811_t& ledStrip, ZoneManager& zoneManager);
void setColorsOnLedStrip(ws2811_t& ledStrip, ZoneManager& zoneManager);
int BGRToWRGBHex(cv::Vec3b color);

// TODO: remove this!
std::vector<cv::Vec3b> DEBUG_generateRainbowColors(int numColors) {
	std::vector<cv::Vec3b> colors;
	colors.reserve(numColors);

	for (int i = 0; i < numColors; ++i) {
		// Hue ranges from 0-240 (red -> violet) in OpenCV HSV
		float hue = (float)i / (numColors - 1) * 150.0f;
		cv::Mat hsv(1, 1, CV_8UC3, cv::Vec3b((uchar)hue, 255, 255));
		cv::Mat bgr;
		cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);
		colors.push_back(bgr.at<cv::Vec3b>(0, 0));
	}

	return colors;
}

// TODO: remove this!
void DEBUG_setRainbowColors(ZoneManager& zoneManager) {
	ZoneSide zoneSides[] = { ZoneSide::LEFT, ZoneSide::TOP, ZoneSide::RIGHT, ZoneSide::BOTTOM };


	int startPos = 0;
	for (ZoneSide zoneSide : zoneSides) {
		std::vector<Zone>& zones = zoneManager.getZonesBySide(zoneSide);

		std::vector<cv::Vec3b> colors = DEBUG_generateRainbowColors(zones.size());

		for (int i = 0; i < zones.size(); i++) {
			zones[i].DEBUG_SETLASTCALCULATEDAVERAGECOLOR(colors[i]);
		}

	}
}



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

	// TODO: remove this!
	DEBUG_setRainbowColors(zoneManager);

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
		//zoneManager.calculateAverages(frame);

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
	ledStrip.channel[0].brightness = 0; // Easy way to turn off all leds
	ws2811_render(&ledStrip);
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
	const cv::Vec3b redColor(0, 0, 255);
	const cv::Vec3b blueColor(255, 0, 0);
	const cv::Vec3b greenColor(0, 255, 0);
	const cv::Vec3b yellowColor(0, 255, 255);

	ZoneSide zoneOrder[] = { ZoneSide::RIGHT, ZoneSide::TOP, ZoneSide::LEFT, ZoneSide::BOTTOM };

	int ledIndex = 0;
	for (ZoneSide zoneSide : zoneOrder) {
		// TODO: change this back to const
		std::vector<Zone>& zones = zoneManager.getZonesBySide(zoneSide);

		// If the current ZoneSide is top of right -> loop through zones in reverse
		bool reverseLoop = (zoneSide == ZoneSide::TOP);
		int start = (reverseLoop ? zones.size() - 1 : 0);
		int end = (reverseLoop ? -1 : zones.size());// Note: past the last-index
		int step = (reverseLoop ? -1 : 1);

		cv::Vec3b color;
		if (zoneSide == ZoneSide::RIGHT) {
			color = redColor;
		}
		if (zoneSide == ZoneSide::LEFT) {
			color = blueColor;
		}
		if (zoneSide == ZoneSide::TOP) {
			color = greenColor;
		}
		if (zoneSide == ZoneSide::BOTTOM) {
			color = yellowColor;
		}

		for (int i = start; i != end; i += step) {
			//zones[i].DEBUG_SETLASTCALCULATEDAVERAGECOLOR(color);
			ledStrip.channel[0].leds[ledIndex] = BGRToWRGBHex(
				zones[i].getLastCalculatedAverageColor()
			);
			ledIndex++;
		}
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