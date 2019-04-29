#include "gtest/gtest.h"

#include "fisch/cerealization.h"
#include "fisch/vx/event.h"

TEST(SpikeLabel, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(SpikeLabel());
	EXPECT_NO_THROW(SpikeLabel(NeuronLabel(), SPL1Address()));

	SpikeLabel config;
	SpikeLabel other_config = config;

	EXPECT_EQ(other_config, config);

	{
		NeuronLabel value(12);
		config.set_neuron_label(value);
		EXPECT_EQ(config.get_neuron_label(), value);
	}

	{
		SPL1Address value(3);
		config.set_spl1_address(value);
		EXPECT_EQ(config.get_spl1_address(), value);
	}

	EXPECT_NE(config, other_config);

	SpikeLabel config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(SpikeLabel, Ostream)
{
	using namespace fisch::vx;

	SpikeLabel obj;

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SpikeLabel(NeuronLabel(0), SPL1Address(0))");
}

TEST(SpikeLabel, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeLabel obj1, obj2;

	obj1.set_neuron_label(NeuronLabel(12));
	obj1.set_spl1_address(SPL1Address(2));

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}

template <size_t NumPack>
void test_spike_pack_to_chip_general()
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	typedef SpikePackToChip<NumPack> spike_pack_t;

	EXPECT_NO_THROW(spike_pack_t());
	EXPECT_NO_THROW(spike_pack_t(typename spike_pack_t::labels_type()));

	spike_pack_t config;
	spike_pack_t other_config = config;

	EXPECT_EQ(other_config, config);

	{
		typename spike_pack_t::labels_type labels;
		size_t i = 0;
		for (auto& label : labels) {
			label = SpikeLabel(NeuronLabel(10 + i), SPL1Address(i));
			i++;
		}
		config.set_labels(labels);
		EXPECT_EQ(config.get_labels(), labels);
	}

	EXPECT_NE(config, other_config);

	spike_pack_t config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(SpikePackToChip, General)
{
	test_spike_pack_to_chip_general<1>();
	test_spike_pack_to_chip_general<2>();
	test_spike_pack_to_chip_general<3>();
}

template <size_t NumPack>
void test_spike_pack_to_chip_ostream()
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	typedef SpikePackToChip<NumPack> spike_pack_t;

	spike_pack_t obj;
	typename spike_pack_t::labels_type labels;
	{
		size_t i = 0;
		for (auto& label : labels) {
			label = SpikeLabel(NeuronLabel(10 + i), SPL1Address(i));
			i++;
		}
		obj.set_labels(labels);
	}

	std::stringstream stream;
	stream << obj;

	std::stringstream expected;
	expected << "SpikePack" << NumPack << "ToChip(";
	size_t i = 0;
	for (auto label : labels) {
		expected << label;
		if (i != labels.size() - 1) {
			expected << ", ";
		}
		i++;
	}
	expected << ")";
	EXPECT_EQ(stream.str(), expected.str());
}

TEST(SpikePackToChip, Ostream)
{
	test_spike_pack_to_chip_ostream<1>();
	test_spike_pack_to_chip_ostream<2>();
	test_spike_pack_to_chip_ostream<3>();
}

template <size_t NumPack>
void test_spike_pack_to_chip_cereal()
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	typedef SpikePackToChip<NumPack> spike_pack_t;

	spike_pack_t obj1, obj2;
	typename spike_pack_t::labels_type labels;
	{
		size_t i = 0;
		for (auto& label : labels) {
			label = SpikeLabel(NeuronLabel(10 + i), SPL1Address(i));
			i++;
		}
		obj1.set_labels(labels);
	}

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}

TEST(SpikePackToChip, CerealizeCoverage)
{
	test_spike_pack_to_chip_cereal<1>();
	test_spike_pack_to_chip_cereal<2>();
	test_spike_pack_to_chip_cereal<3>();
}

TEST(SpikeFromChip, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(SpikeFromChip());
	EXPECT_NO_THROW(SpikeFromChip(SpikeLabel(NeuronLabel(), SPL1Address()), ChipTime()));

	SpikeFromChip config;
	SpikeFromChip other_config = config;

	EXPECT_EQ(other_config, config);

	{
		SpikeLabel value(NeuronLabel(12), SPL1Address(2));
		config.set_label(value);
		EXPECT_EQ(config.get_label(), value);
	}

	{
		ChipTime value(3);
		config.set_chip_time(value);
		EXPECT_EQ(config.get_chip_time(), value);
	}

	EXPECT_NE(config, other_config);

	SpikeFromChip config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(SpikeFromChip, Ostream)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeFromChip obj(SpikeLabel(NeuronLabel(12), SPL1Address(2)), ChipTime(3));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(
	    stream.str(), "SpikeFromChip(SpikeLabel(NeuronLabel(12), SPL1Address(2)), ChipTime(3))");
}

TEST(SpikeFromChip, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeFromChip obj1, obj2;

	obj1.set_label(SpikeLabel(NeuronLabel(12), SPL1Address(2)));
	obj1.set_chip_time(ChipTime(3));

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}

TEST(MADCSampleFromChip, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(MADCSampleFromChip());
	EXPECT_NO_THROW(MADCSampleFromChip(MADCSampleFromChip::Value(), ChipTime()));

	MADCSampleFromChip config;
	MADCSampleFromChip other_config = config;

	EXPECT_EQ(other_config, config);

	{
		MADCSampleFromChip::Value value(5);
		config.set_value(value);
		EXPECT_EQ(config.get_value(), value);
	}

	{
		ChipTime value(3);
		config.set_chip_time(value);
		EXPECT_EQ(config.get_chip_time(), value);
	}

	EXPECT_NE(config, other_config);

	MADCSampleFromChip config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(MADCSampleFromChip, Ostream)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	MADCSampleFromChip obj(MADCSampleFromChip::Value(5), ChipTime(3));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "MADCSampleFromChip(Value(5), ChipTime(3))");
}

TEST(MADCSampleFromChip, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	MADCSampleFromChip obj1, obj2;

	obj1.set_value(MADCSampleFromChip::Value(5));
	obj1.set_chip_time(ChipTime(3));

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}

TEST(SpikeFromChipEvent, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(SpikeFromChipEvent());
	EXPECT_NO_THROW(SpikeFromChipEvent(
	    SpikeFromChip(SpikeLabel(NeuronLabel(), SPL1Address()), ChipTime()), FPGATime()));

	SpikeFromChipEvent config;
	SpikeFromChipEvent other_config = config;

	EXPECT_EQ(other_config, config);

	{
		SpikeFromChip value(SpikeLabel(NeuronLabel(12), SPL1Address(2)), ChipTime(5));
		config.set_spike(value);
		EXPECT_EQ(config.get_spike(), value);
	}

	{
		FPGATime value(3);
		config.set_fpga_time(value);
		EXPECT_EQ(config.get_fpga_time(), value);
	}

	EXPECT_NE(config, other_config);

	SpikeFromChipEvent config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(SpikeFromChipEvent, Ostream)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeFromChipEvent obj(
	    SpikeFromChip(SpikeLabel(NeuronLabel(12), SPL1Address(2)), ChipTime(3)), FPGATime(6));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(
	    stream.str(), "SpikeFromChipEvent(SpikeFromChip(SpikeLabel(NeuronLabel(12), "
	                  "SPL1Address(2)), ChipTime(3)), FPGATime(6))");
}

TEST(SpikeFromChipEvent, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeFromChipEvent obj1, obj2;

	obj1.set_spike(SpikeFromChip(SpikeLabel(NeuronLabel(12), SPL1Address(2)), ChipTime(6)));
	obj1.set_fpga_time(FPGATime(3));

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_EQ(obj1, obj2);
}
