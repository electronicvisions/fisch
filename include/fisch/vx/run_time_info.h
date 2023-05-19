#pragma once
#include "fisch/vx/genpybind.h"
#include "hate/visibility.h"
#include "hxcomm/common/connection_time_info.h"
#include <chrono>
#include <ostream>

#if defined(__GENPYBIND__) || defined(__GENPYBIND_GENERATED__)
#include <pybind11/chrono.h>
#endif

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Time information of a run() invokation.
 */
struct GENPYBIND(visible) RunTimeInfo
{
	/**
	 * Time information of the connection usage during the run() invokation.
	 */
	hxcomm::ConnectionTimeInfo connection;

	/**
	 * Time spent decoding response messages into the playback program response queues.
	 */
	std::chrono::nanoseconds playback_decode_time;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, RunTimeInfo const& data) SYMBOL_VISIBLE;
};

} // namespace fisch::vx
