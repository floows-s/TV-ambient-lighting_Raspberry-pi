#pragma once
/// <summary>
/// A struct that stores the amount of LEDs on each side of the screen.
/// </summary>
struct LEDCounts {
	unsigned int top;
	unsigned int bottom;
	unsigned int left;
	unsigned int right;

	/// <summary>
	/// Adds up all the counts and returns the sum.
	/// </summary>
	/// <returns>The sum of all leds</returns>
	unsigned int all() const {
		return top + bottom + left + right;
	}
};

