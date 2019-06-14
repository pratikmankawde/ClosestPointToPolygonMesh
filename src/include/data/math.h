/*
 * Author: Pratik Mankawde
 */
#pragma once

#include <cassert>
#include <functional>

/**
 * Function to clamp value between high and low
 * Taken from std::c++17 https://en.cppreference.com/w/cpp/algorithm/clamp
 * @param value Argument value
 * @param low Lower limit
 * @param high Higher limit
 * @param comp Comparator type
 * @return Clamped value
 */
template<class T, class Compare>
constexpr const T& clamp(const T& value, const T& low, const T& high, Compare comp) {
	return assert( !comp(high, low) ),
			comp(value, low) ? low : comp(high, value) ? high : value;
}

/**
 * Function to clamp value between high and low
 * Taken from std::c++17 https://en.cppreference.com/w/cpp/algorithm/clamp
 * @param value Argument value
 * @param low Lower limit
 * @param high Higher limit
 * @return Clamped value
 */
template<class T>
constexpr const T& clamp(const T& value, const T& low, const T& high) {
	return clamp(value, low, high, std::less<T>());
}

