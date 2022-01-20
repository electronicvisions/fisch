#include "fisch/vx/word_access/type/jtag.h"

namespace fisch::vx::word_access_type {

bool ResetJTAGTap::operator==(ResetJTAGTap const& /* other */) const
{
	return true;
}

bool ResetJTAGTap::operator!=(ResetJTAGTap const& /* other */) const
{
	return false;
}

} // namespace fisch::vx::word_access_type
