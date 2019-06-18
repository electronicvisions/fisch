#pragma once
#include "fisch/vx/genpybind.h"
#include "fisch/vx/systime.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "hxcomm/vx/utmessage.h"

namespace cereal {
class access;
} // namespace cereal

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

class GENPYBIND(visible) SpikeLabel
{
public:
	typedef halco::hicann_dls::vx::NeuronLabel neuron_label_type;
	typedef halco::hicann_dls::vx::SPL1Address spl1_address_type;

	/** Default constructor. */
	SpikeLabel();

	/**
	 * Construct spike label with neuron and spl1 address.
	 * @param neuron NeuronLabel value to use
	 * @param spl1 SPL1Address value to use
	 */
	SpikeLabel(neuron_label_type const& neuron, spl1_address_type const& spl1);

	/**
	 * Get neuron address value.
	 * @return NeuronLabel
	 */
	GENPYBIND(getter_for(neuron_label))
	neuron_label_type get_neuron_label() const;

	/**
	 * Set neuron address value.
	 * @param value NeuronLabel to set
	 */
	GENPYBIND(setter_for(neuron_label))
	void set_neuron_label(neuron_label_type const& value);

	/**
	 * Get spl1 address value.
	 * @return SPL1Address
	 */
	GENPYBIND(getter_for(spl1_address))
	spl1_address_type get_spl1_address() const;

	/**
	 * Set spl1 address value.
	 * @param value SPL1Address to set
	 */
	GENPYBIND(setter_for(spl1_address))
	void set_spl1_address(spl1_address_type const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SpikeLabel const& spike_label);

	bool operator==(SpikeLabel const& other) const;
	bool operator!=(SpikeLabel const& other) const;

private:
	neuron_label_type m_neuron_label;
	spl1_address_type m_spl1_address;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};


template <size_t NumPack>
class SpikePackToChip
{
public:
	typedef hate::type_list<
	    halco::hicann_dls::vx::SpikePack1ToChipOnDLS,
	    halco::hicann_dls::vx::SpikePack2ToChipOnDLS,
	    halco::hicann_dls::vx::SpikePack3ToChipOnDLS>
	    spike_pack_to_chip_coordinates;

	typedef
	    typename hate::index_type_list_by_integer<NumPack - 1, spike_pack_to_chip_coordinates>::type
	        coordinate_type;
	typedef std::array<SpikeLabel, NumPack> labels_type;

	/** Default constructor. */
	SpikePackToChip();

	/**
	 * Construct spike pack with labels.
	 * @param labels Array of SpikeLabel values to use
	 */
	SpikePackToChip(labels_type const& labels);

	/**
	 * Get spike labels.
	 * @return Array of SpikeLabel
	 */
	// NOTE: property python wrapping not possible due to inheritance from a templated class.
	labels_type get_labels() const;

	/**
	 * Set spike labels.
	 * @param value Array of SpikeLabel to set
	 */
	// NOTE: property python wrapping not possible due to inheritance from a templated class.
	void set_labels(labels_type const& value);

	// NOTE: operator needs to be inlined for python wrapping to work
	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SpikePackToChip const& spike_pack)
	{
		std::stringstream ss;
		os << "SpikePack" << NumPack << "ToChip(";
		for (size_t i = 0; i < spike_pack.m_labels.size() - 1; ++i) {
			os << spike_pack.m_labels[i] << ", ";
		}
		os << spike_pack.m_labels[spike_pack.m_labels.size() - 1] << ")";
		return os;
	}

	bool operator==(SpikePackToChip const& other) const;
	bool operator!=(SpikePackToChip const& other) const;

	constexpr static size_t GENPYBIND(hidden) encode_read_ut_message_count = 0;
	constexpr static size_t GENPYBIND(hidden) encode_write_ut_message_count = 1;
	constexpr static size_t GENPYBIND(hidden) decode_ut_message_count = 0;

	static std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_read_ut_message_count> encode_read(
	    coordinate_type const& coord) GENPYBIND(hidden);
	std::array<hxcomm::vx::UTMessageToFPGAVariant, encode_write_ut_message_count> encode_write(
	    coordinate_type const& coord) const GENPYBIND(hidden);
	void decode(
	    std::array<hxcomm::vx::UTMessageFromFPGAVariant, decode_ut_message_count> const& messages)
	    GENPYBIND(hidden);

protected:
	labels_type m_labels;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};

typedef SpikePackToChip<1> SpikePack1ToChip GENPYBIND(opaque);
typedef SpikePackToChip<2> SpikePack2ToChip GENPYBIND(opaque);
typedef SpikePackToChip<3> SpikePack3ToChip GENPYBIND(opaque);


class GENPYBIND(visible) SpikeFromChip
{
public:
	/** Default constructor. */
	SpikeFromChip();

	/**
	 * Construct spike event with label and time.
	 * @param label SpikeLabel value to use
	 * @param time ChipTime value to use
	 */
	SpikeFromChip(SpikeLabel const& label, ChipTime const& time);

	/**
	 * Get spike label value.
	 * @return SpikeLabel
	 */
	GENPYBIND(getter_for(label))
	SpikeLabel get_label() const;

	/**
	 * Set spike label value.
	 * @param value SpikeLabel to set
	 */
	GENPYBIND(setter_for(label))
	void set_label(SpikeLabel const& value);

	/**
	 * Get chip time value.
	 * @return ChipTime
	 */
	GENPYBIND(getter_for(chip_time))
	ChipTime get_chip_time() const;

	/**
	 * Set chip time value.
	 * @param value ChipTime to set
	 */
	GENPYBIND(setter_for(chip_time))
	void set_chip_time(ChipTime const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SpikeFromChip const& spike_label);

	bool operator==(SpikeFromChip const& other) const;
	bool operator!=(SpikeFromChip const& other) const;

private:
	SpikeLabel m_label;
	ChipTime m_chip_time;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};


class GENPYBIND(visible) MADCSampleFromChip
{
public:
	/** Default constructor. */
	MADCSampleFromChip();

	/** Sample value. */
	struct GENPYBIND(inline_base("*")) Value
	    : public halco::common::detail::RantWrapper<Value, uintmax_t, 0x3fff, 0>
	{
		constexpr explicit Value(uintmax_t const val = 0) : rant_t(val) {}
	};

	/**
	 * Construct MADC sample event with value and time.
	 * @param value Value value to use
	 * @param time ChipTime value to use
	 */
	MADCSampleFromChip(Value const& value, ChipTime const& time);

	/**
	 * Get sample value.
	 * @return Value
	 */
	GENPYBIND(getter_for(value))
	Value get_value() const;

	/**
	 * Set sample value.
	 * @param value Value to set
	 */
	GENPYBIND(setter_for(value))
	void set_value(Value const& value);

	/**
	 * Get chip time value.
	 * @return ChipTime
	 */
	GENPYBIND(getter_for(chip_time))
	ChipTime get_chip_time() const;

	/**
	 * Set chip time value.
	 * @param value ChipTime to set
	 */
	GENPYBIND(setter_for(chip_time))
	void set_chip_time(ChipTime const& value);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, MADCSampleFromChip const& spike_label);

	bool operator==(MADCSampleFromChip const& other) const;
	bool operator!=(MADCSampleFromChip const& other) const;

private:
	Value m_value;
	ChipTime m_chip_time;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};


class GENPYBIND(visible) SpikeFromChipEvent
{
public:
	SpikeFromChipEvent(
	    SpikeFromChip const& spike = SpikeFromChip(), FPGATime const& fpga_time = FPGATime()) :
	    m_spike(spike),
	    m_fpga_time(fpga_time)
	{}

	GENPYBIND(getter_for(spike))
	SpikeFromChip get_spike() const;
	GENPYBIND(setter_for(spike))
	void set_spike(SpikeFromChip const& spike);

	GENPYBIND(getter_for(fpga_time))
	FPGATime get_fpga_time() const;
	GENPYBIND(setter_for(fpga_time))
	void set_fpga_time(FPGATime const& fpga_time);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, SpikeFromChipEvent const& event);

	bool operator==(SpikeFromChipEvent const& other) const;
	bool operator!=(SpikeFromChipEvent const& other) const;

private:
	SpikeFromChip m_spike;
	FPGATime m_fpga_time;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};


class GENPYBIND(visible) MADCSampleFromChipEvent
{
public:
	MADCSampleFromChipEvent(
	    MADCSampleFromChip const& sample = MADCSampleFromChip(),
	    FPGATime const& fpga_time = FPGATime()) :
	    m_sample(sample),
	    m_fpga_time(fpga_time)
	{}

	GENPYBIND(getter_for(sample))
	MADCSampleFromChip get_sample() const;
	GENPYBIND(setter_for(sample))
	void set_sample(MADCSampleFromChip const& spike);

	GENPYBIND(getter_for(fpga_time))
	FPGATime get_fpga_time() const;
	GENPYBIND(setter_for(fpga_time))
	void set_fpga_time(FPGATime const& fpga_time);

	GENPYBIND(stringstream)
	friend std::ostream& operator<<(std::ostream& os, MADCSampleFromChipEvent const& event);

	bool operator==(MADCSampleFromChipEvent const& other) const;
	bool operator!=(MADCSampleFromChipEvent const& other) const;

private:
	MADCSampleFromChip m_sample;
	FPGATime m_fpga_time;

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& ar);
};

} // namespace fisch::vx
