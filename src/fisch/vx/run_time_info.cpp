#include "fisch/vx/run_time_info.h"

#include "hate/timer.h"

namespace fisch::vx {

std::ostream& operator<<(std::ostream& os, RunTimeInfo const& data)
{
	os << "RunTimeInfo(" << std::endl;
	os << data.connection;
	os << ", playback_decode_time: " << hate::to_string(data.playback_decode_time) << ")";
	return os;
}

} // namespace fisch::vx
