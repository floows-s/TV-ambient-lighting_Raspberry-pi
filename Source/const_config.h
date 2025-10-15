#pragma once

/*
	Purpose: 
	This file holds const values that are used all over the program.
	They can be configured here.
*/
namespace Config {
	/*
		This defines the thicknes of a zone in ratio to the screen.
		If you have a verticaly oriented zone (left or right side) you multiply the ratio with the width of the screen to get the width of the zone.
		But if you have a horizontaly oriented zone (top or bottom side) you multiply the ratio with the height of the screen to get the height of a zone.
		Example: 0.10 == 10%.
	*/
	const float ZONE_THICKNES_TO_SCREEN_RATIO = 0.10f;

	/*
	* This is used in the constructor of the VideoCapture object.
	* With index at 0, it will use the first video device that it can find.
	*/
	const int VIDEO_CAPTURE_INDEX = 0;
}
