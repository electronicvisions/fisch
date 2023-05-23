#pragma once
#include "fisch/vx/chip_time.h"
#include "fisch/vx/fpga_time.h"
#include "fisch/vx/genpybind.h"
#include "fisch/vx/word_access/type/event.h"
#include "hate/join.h"
#include "hxcomm/vx/utmessage_fwd.h"

namespace cereal {
struct access;
} // namespace cereal

namespace halco::hicann_dls::vx {
struct NeuronLabel;
struct SPL1Address;
struct SpikeLabel;
struct SpikePack1ToChipOnDLS;
struct SpikePack2ToChipOnDLS;
struct SpikePack3ToChipOnDLS;
} // namespace halco::hicann_dls::vx

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

/**
 * Spike payload information.
 */
typedef halco::hicann_dls::vx::SpikeLabel SpikeLabel GENPYBIND(visible);


/**
 * Base of fixed-size spike pack to chip.
 * @tparam NumPack Number of spike labels to hold
 */
template <size_t NumPack>
class SpikePackToChip
{
public:
	typedef hate::type_list<
	    halco::hicann_dls::vx::SpikePack1ToChipOnDLS,
	    halco::hicann_dls::vx::SpikePack2ToChipOnDLS,
	    halco::hicann_dls::vx::SpikePack3ToChipOnDLS>
	    spike_pack_to_chip_coordinates;

	typedef hate::type_list<
	    word_access_type::SpikePack1ToChip,
	    word_access_type::SpikePack2ToChip,
	    word_access_type::SpikePack3ToChip>
	    spike_pack_to_chip_labels;

	typedef
	    typename hate::index_type_list_by_integer<NumPack - 1, spike_pack_to_chip_coordinates>::type
	        coordinate_type;
	typedef typename hate::index_type_list_by_integer<NumPack - 1, spike_pack_to_chip_labels>::type
	    Value;

	/** Default constructor. */
	explicit SpikePackToChip() : m_value() {}

	/**
	 * Construct spike pack with labels.
	 * @param labels Array of SpikeLabel values to use
	 */
	explicit SpikePackToChip(Value const& labels) : m_value(labels) {}

	/**
	 * Get spike labels.
	 * @return Array of SpikeLabel
	 */
	Value const& get() const;

	/**
	 * Set spike labels.
	 * @param value Array of SpikeLabel to set
	 */
	void set(Value const& value);

	// NOTE: operator needs to be inlined for python wrapping to work
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SpikePackToChip const& spike_pack)
	{
		std::stringstream ss;
		os << "SpikePack" << NumPack << "ToChip(" << hate::join_string(spike_pack.m_value, ", ")
		   << ")";
		return os;
	}

	bool operator==(SpikePackToChip const& other) const;
	bool operator!=(SpikePackToChip const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;

	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);

protected:
	Value m_value;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

/**
 * Spike pack to chip holding one spike label.
 */
class GENPYBIND(inline_base("*")) SpikePack1ToChip : public SpikePackToChip<1>
{
public:
	explicit SpikePack1ToChip() : SpikePackToChip<1>() {}
	explicit SpikePack1ToChip(Value const& labels) : SpikePackToChip<1>(labels) {}
};

/**
 * Spike pack to chip holding two spike labels.
 */
class GENPYBIND(inline_base("*")) SpikePack2ToChip : public SpikePackToChip<2>
{
public:
	explicit SpikePack2ToChip() : SpikePackToChip<2>() {}
	explicit SpikePack2ToChip(Value const& labels) : SpikePackToChip<2>(labels) {}
};

/**
 * Spike pack to chip holding three spike labels.
 */
class GENPYBIND(inline_base("*")) SpikePack3ToChip : public SpikePackToChip<3>
{
public:
	explicit SpikePack3ToChip() : SpikePackToChip<3>() {}
	explicit SpikePack3ToChip(Value const& labels) : SpikePackToChip<3>(labels) {}
};


/**
 * Spike from chip event comprised of a SpikeFromChip and FPGATime time annotation.
 * Serves as value type for spikes accessible on a PlaybackProgram.
 */
class GENPYBIND(visible) SpikeFromChip
{
public:
	/**
	 * Construct spike from chip event from a label, a Chip time and a FPGA time.
	 * @param label SpikeLabel data
	 * @param chip_time ChipTime time annotation
	 * @param fpga_time FPGATime time annotation
	 */
	explicit SpikeFromChip(
	    SpikeLabel const& label = SpikeLabel(),
	    ChipTime const& chip_time = ChipTime(),
	    FPGATime const& fpga_time = FPGATime()) :
	    label(label), chip_time(chip_time), fpga_time(fpga_time)
	{}

	/**
	 * Spike label value.
	 */
	SpikeLabel label;

	/**
	 * Chip time value.
	 */
	ChipTime chip_time;

	/**
	 * FPGA time annotation.
	 */
	FPGATime fpga_time;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SpikeFromChip const& event);

	bool operator==(SpikeFromChip const& other) const;
	bool operator!=(SpikeFromChip const& other) const;

private:
	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};


/**
 * MADC sample from chip event comprised of MADCSampleFromChip data and FPGATime time annotation.
 * Serves as value type for MADC samples accessible on a PlaybackProgram.
 */
class GENPYBIND(visible) MADCSampleFromChip
{
public:
	/** Sample value. Restricted to the MADC's 10 bit resolution. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uint16_t, 0x3ff, 0>
	{
		constexpr explicit Value(uintmax_t const val = 0) : rant_t(val) {}
	};

	/** Channel selected on active multiplexer from which the sampled value was obtained.
	 * Given as a ReadoutSourceSelection coordinate. */
	struct GENPYBIND(inline_base("*")) Channel
	    : public halco::common::detail::RantWrapper<Channel, uint8_t, 0x1, 0>
	{
		constexpr explicit Channel(uintmax_t const val = 0) : rant_t(val) {}
	};

	/**
	 * Construct MADC sample from chip event from a MADC sample from chip and a FPGA time.
	 * @param value Sample value
	 * @param chip_time ChipTime time annotation
	 * @param fpga_time FPGATime time annotation
	 */
	explicit MADCSampleFromChip(
	    Value const& value = Value(),
	    Channel const& channel = Channel(),
	    ChipTime const& chip_time = ChipTime(),
	    FPGATime const& fpga_time = FPGATime()) :
	    value(value), channel(channel), chip_time(chip_time), fpga_time(fpga_time)
	{}

	/**
	 * Sample value.
	 */
	Value value;

	/**
	 * Channel from which this sample was acquired.
	 */
	Channel channel;

	/**
	 * Chip time value.
	 */
	ChipTime chip_time;

	/**
	 * FPGA time annotation.
	 */
	FPGATime fpga_time;

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, MADCSampleFromChip const& event);

	bool operator==(MADCSampleFromChip const& other) const;
	bool operator!=(MADCSampleFromChip const& other) const;

private:
	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

/**
 * Notification from the FPGA-chip link.
 */
class GENPYBIND(visible) HighspeedLinkNotification
{
public:
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::BaseType<Value, uint8_t>
	{
		constexpr explicit Value(value_type const val = 0) : base_t(val) {}
	};

	/**
	 * Construct FPGA-chip link notification from a value and a FPGA time.
	 * @param value Value to store
	 * @param fpga_time FPGATime time annotation
	 */
	explicit HighspeedLinkNotification(
	    Value const& value = Value(), FPGATime const& fpga_time = FPGATime()) :
	    m_value(value), m_fpga_time(fpga_time)
	{}

	/**
	 * Get value data.
	 * @return Value data
	 */
	GENPYBIND(getter_for(value))
	Value get_value() const;

	/**
	 * Get value data.
	 * @param value Value data
	 */
	GENPYBIND(setter_for(value))
	void set_value(Value const& value);

	/**
	 * Get FPGA time annotation.
	 * @return FPGATime time annotation
	 */
	GENPYBIND(getter_for(fpga_time))
	FPGATime get_fpga_time() const;

	/**
	 * Set FPGA time annotation.
	 * @param value FPGATime time annotation
	 */
	GENPYBIND(setter_for(fpga_time))
	void set_fpga_time(FPGATime const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, HighspeedLinkNotification const& event);

	bool operator==(HighspeedLinkNotification const& other) const;
	bool operator!=(HighspeedLinkNotification const& other) const;

private:
	Value m_value;
	FPGATime m_fpga_time;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

/**
 * Notification from the FPGA indicating a playback instruction timeout.
 */
class GENPYBIND(visible) TimeoutNotification
{
public:
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::BaseType<Value, uint32_t>
	{
		constexpr explicit Value(value_type const val = 0) : base_t(val) {}
	};

	/**
	 * Construct Playback instruction timeout notification from a value and an
	 * FPGA time.
	 * @param value Value to store
	 * @param fpga_time FPGATime time annotation
	 */
	explicit TimeoutNotification(
	    Value const& value = Value(), FPGATime const& fpga_time = FPGATime()) :
	    m_value(value), m_fpga_time(fpga_time)
	{}

	/**
	 * Get value data.
	 * @return Value data
	 */
	GENPYBIND(getter_for(value))
	Value get_value() const;

	/**
	 * Get value data.
	 * @param value Value data
	 */
	GENPYBIND(setter_for(value))
	void set_value(Value const& value);

	/**
	 * Get FPGA time annotation.
	 * @return FPGATime time annotation
	 */
	GENPYBIND(getter_for(fpga_time))
	FPGATime get_fpga_time() const;

	/**
	 * Set FPGA time annotation.
	 * @param value FPGATime time annotation
	 */
	GENPYBIND(setter_for(fpga_time))
	void set_fpga_time(FPGATime const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, TimeoutNotification const& event);

	bool operator==(TimeoutNotification const& other) const;
	bool operator!=(TimeoutNotification const& other) const;

private:
	Value m_value;
	FPGATime m_fpga_time;

	friend struct cereal::access;
	template <class Archive>
	void serialize(Archive& ar, std::uint32_t const version);
};

} // namespace fisch::vx
