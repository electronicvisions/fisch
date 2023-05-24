#pragma once
#include "fisch/vx/constants.h"
#include "fisch/vx/genpybind.h"
#include "halco/common/geometry.h"

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {
namespace word_access_type GENPYBIND_MODULE {

struct GENPYBIND(inline_base("*")) I2CIdRegister
    : public halco::common::detail::RantWrapper<
          I2CIdRegister,
          uint_fast32_t,
          std::numeric_limits<uint32_t>::max(),
          std::numeric_limits<uint32_t>::min()>
{
	constexpr explicit I2CIdRegister(uintmax_t const val = 0) GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CTempRegister
    : public halco::common::detail::RantWrapper<
          I2CTempRegister,
          uint_fast16_t,
          std::numeric_limits<uint16_t>::max(),
          std::numeric_limits<uint16_t>::min()>
{
	constexpr explicit I2CTempRegister(uintmax_t const val = 0) GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CINA219RoRegister
    : public halco::common::detail::RantWrapper<
          I2CINA219RoRegister,
          uint_fast16_t,
          std::numeric_limits<uint16_t>::max(),
          std::numeric_limits<uint16_t>::min()>
{
	constexpr explicit I2CINA219RoRegister(uintmax_t const val = 0) GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CINA219RwRegister
    : public halco::common::detail::RantWrapper<
          I2CINA219RwRegister,
          uint_fast16_t,
          std::numeric_limits<uint16_t>::max(),
          std::numeric_limits<uint16_t>::min()>
{
	constexpr explicit I2CINA219RwRegister(uintmax_t const val = 0) GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CTCA9554RoRegister
    : public halco::common::detail::RantWrapper<
          I2CTCA9554RoRegister,
          uint_fast16_t,
          std::numeric_limits<uint8_t>::max(),
          std::numeric_limits<uint8_t>::min()>
{
	constexpr explicit I2CTCA9554RoRegister(uintmax_t const val = 0)
	    GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CTCA9554RwRegister
    : public halco::common::detail::RantWrapper<
          I2CTCA9554RwRegister,
          uint_fast16_t,
          std::numeric_limits<uint8_t>::max(),
          std::numeric_limits<uint8_t>::min()>
{
	constexpr explicit I2CTCA9554RwRegister(uintmax_t const val = 0)
	    GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CAD5252RwRegister
    : public halco::common::detail::RantWrapper<
          I2CAD5252RwRegister,
          uint_fast16_t,
          std::numeric_limits<uint8_t>::max(),
          std::numeric_limits<uint8_t>::min()>
{
	constexpr explicit I2CAD5252RwRegister(uintmax_t const val = 0) GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

struct GENPYBIND(inline_base("*")) I2CDAC6573RwRegister
    : public halco::common::detail::RantWrapper<
          I2CDAC6573RwRegister,
          uint_fast16_t,
          fisch::vx::dac6573_value_max,
          fisch::vx::dac_value_min>
{
	constexpr explicit I2CDAC6573RwRegister(uintmax_t const val = 0)
	    GENPYBIND(implicit_conversion) :
	    rant_t(val)
	{}
};

} // namespace word_access_type
} // namespace fisch::vx

namespace std {

HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CIdRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CTempRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CINA219RoRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CINA219RwRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CTCA9554RoRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CTCA9554RwRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CAD5252RwRegister)
HALCO_GEOMETRY_HASH_CLASS(fisch::vx::word_access_type::I2CDAC6573RwRegister)

} // namespace std
