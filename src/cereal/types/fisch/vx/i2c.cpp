#include "fisch/vx/i2c.h"

#include "cereal/types/halco/common/geometry.h"
#include "fisch/cerealization.tcc"

namespace fisch::vx {

template <typename Derived, typename ValueType, typename CoordinateType>
template <class Archive>
void I2CRoRegister<Derived, ValueType, CoordinateType>::serialize(Archive& ar, std::uint32_t const)
{
	ar(CEREAL_NVP(m_value));
}

template <class Archive>
void I2CIdRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CIdRegister, Value, coordinate_type>>(this));
}

template <class Archive>
void I2CINA219RoRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CINA219RoRegister, Value, coordinate_type>>(this));
}

template <class Archive>
void I2CINA219RwRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(CEREAL_NVP(m_value));
}

template <class Archive>
void I2CTCA9554RoRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CTCA9554RoRegister, Value, coordinate_type>>(this));
}

template <class Archive>
void I2CTCA9554RwRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CTCA9554RwRegister, Value, coordinate_type>>(this));
}

template <class Archive>
void I2CAD5252RwRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(CEREAL_NVP(m_value));
}

template <class Archive>
void I2CDAC6573RwRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(CEREAL_NVP(m_value));
}

template <class Archive>
void I2CTempRegister::serialize(Archive& ar, std::uint32_t)
{
	ar(cereal::base_class<I2CRoRegister<I2CTempRegister, Value, coordinate_type>>(this));
}

EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CIdRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CINA219RoRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CINA219RwRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CTCA9554RoRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CTCA9554RwRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CAD5252RwRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CDAC6573RwRegister)
EXPLICIT_INSTANTIATE_CEREAL_SERIALIZE(I2CTempRegister)

} // namespace fisch::vx

CEREAL_CLASS_VERSION(fisch::vx::I2CIdRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CINA219RoRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CINA219RwRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CTCA9554RoRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CTCA9554RwRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CAD5252RwRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CDAC6573RwRegister, 0)
CEREAL_CLASS_VERSION(fisch::vx::I2CTempRegister, 0)
