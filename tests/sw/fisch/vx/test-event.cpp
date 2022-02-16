#include "gtest/gtest.h"

#include "fisch/cerealization.h"
#include "fisch/vx/event.h"

template <size_t NumPack>
void test_spike_pack_to_chip_general()
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	typedef SpikePackToChip<NumPack> spike_pack_t;

	EXPECT_NO_THROW(spike_pack_t());
	EXPECT_NO_THROW(spike_pack_t(typename spike_pack_t::Value()));

	spike_pack_t config;
	spike_pack_t other_config = config;

	EXPECT_EQ(other_config, config);

	{
		typename spike_pack_t::Value labels;
		size_t i = 0;
		for (auto& label : labels) {
			label = SpikeLabel(10 + i);
			i++;
		}
		config.set(labels);
		EXPECT_EQ(config.get(), labels);
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
	typename spike_pack_t::Value labels;
	{
		size_t i = 0;
		for (auto& label : labels) {
			label = SpikeLabel(10 + i);
			i++;
		}
		obj.set(labels);
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
	typename spike_pack_t::Value labels;
	{
		size_t i = 0;
		for (auto& label : labels) {
			label = SpikeLabel(10 + i);
			i++;
		}
		obj1.set(labels);
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

TEST(MADCSampleFromChip, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(MADCSampleFromChip());
	EXPECT_NO_THROW(MADCSampleFromChip(MADCSampleFromChip::Value(), ChipTime(), FPGATime()));

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

	{
		FPGATime value(3);
		config.set_fpga_time(value);
		EXPECT_EQ(config.get_fpga_time(), value);
	}

	EXPECT_NE(config, other_config);

	MADCSampleFromChip config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(MADCSampleFromChip, Ostream)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	MADCSampleFromChip obj(MADCSampleFromChip::Value(5), ChipTime(3), FPGATime(7));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "MADCSampleFromChip(Value(5), ChipTime(3), FPGATime(7))");
}

TEST(MADCSampleFromChip, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	MADCSampleFromChip obj1, obj2;

	obj1.set_value(MADCSampleFromChip::Value(5));
	obj1.set_chip_time(ChipTime(3));
	obj1.set_fpga_time(FPGATime(7));

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

TEST(SpikeFromChip, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(SpikeFromChip());
	EXPECT_NO_THROW(SpikeFromChip(SpikeLabel(), ChipTime(), FPGATime()));

	SpikeFromChip config;
	SpikeFromChip other_config = config;

	EXPECT_EQ(other_config, config);

	{
		SpikeLabel value(12);
		config.set_label(value);
		EXPECT_EQ(config.get_label(), value);
	}

	{
		ChipTime value(3);
		config.set_chip_time(value);
		EXPECT_EQ(config.get_chip_time(), value);
	}

	{
		FPGATime value(3);
		config.set_fpga_time(value);
		EXPECT_EQ(config.get_fpga_time(), value);
	}

	EXPECT_NE(config, other_config);

	SpikeFromChip config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(SpikeFromChip, Ostream)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeFromChip obj(SpikeLabel(12), ChipTime(3), FPGATime(6));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "SpikeFromChip(SpikeLabel(12), ChipTime(3), FPGATime(6))");
}

TEST(SpikeFromChip, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	SpikeFromChip obj1, obj2;

	obj1.set_label(SpikeLabel(12));
	obj1.set_chip_time(ChipTime(4));
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

TEST(HighspeedLinkNotification, General)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	EXPECT_NO_THROW(HighspeedLinkNotification());
	EXPECT_NO_THROW(HighspeedLinkNotification(HighspeedLinkNotification::Value(), FPGATime()));

	HighspeedLinkNotification config;
	HighspeedLinkNotification other_config = config;

	EXPECT_EQ(other_config, config);

	{
		HighspeedLinkNotification::Value value(0x12);
		config.set_value(value);
		EXPECT_EQ(config.get_value(), value);
	}

	{
		FPGATime value(3);
		config.set_fpga_time(value);
		EXPECT_EQ(config.get_fpga_time(), value);
	}

	EXPECT_NE(config, other_config);

	HighspeedLinkNotification config3 = config;
	EXPECT_EQ(config, config3);
}

TEST(HighspeedLinkNotification, Ostream)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	HighspeedLinkNotification obj(HighspeedLinkNotification::Value(12), FPGATime(6));

	std::stringstream stream;
	stream << obj;

	EXPECT_EQ(stream.str(), "HighspeedLinkNotification(Value(12), FPGATime(6))");
}

TEST(HighspeedLinkNotification, CerealizeCoverage)
{
	using namespace fisch::vx;
	using namespace halco::hicann_dls::vx;

	HighspeedLinkNotification obj1, obj2;

	obj1.set_value(HighspeedLinkNotification::Value(0x12));
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
