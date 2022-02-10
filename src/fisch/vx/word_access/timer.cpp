#include "fisch/vx/word_access/type/timer.h"

namespace fisch::vx::word_access_type {

bool Timer::operator==(Timer const& /* other */) const
{
	return true;
}

bool Timer::operator!=(Timer const& /* other */) const
{
	return false;
}

} // namespace fisch::vx::word_access_type
