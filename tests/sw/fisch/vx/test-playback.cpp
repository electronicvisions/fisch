#include <gtest/gtest.h>

#include "fisch/vx/container_ticket.h"
#include "fisch/vx/encode.h"
#include "fisch/vx/fill.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"

#include "fisch/cerealization.tcc"
#include "fisch/common/logger.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/traits.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hate/timer.h"
#include "hxcomm/vx/utmessage.h"

#include <cereal/types/memory.hpp>

using namespace fisch::vx;
using namespace halco::hicann_dls::vx;

template <typename ContainerT>
void test_playback_program_builder_read_api()
{
	PlaybackProgramBuilder builder;

	typename ContainerT::coordinate_type coord;
	std::vector<typename ContainerT::coordinate_type> vector_coord{coord};

	// single coordinate read
	builder.read(coord);

	// vector coordinate read
	builder.read(vector_coord);
}

TEST(PlaybackProgramBuilder, ReadAPI)
{
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	if constexpr (IsReadable<Type>::value) {                                                       \
		EXPECT_NO_THROW(test_playback_program_builder_read_api<Type>());                           \
	} else {                                                                                       \
		EXPECT_THROW(test_playback_program_builder_read_api<Type>(), std::logic_error);            \
	}

#include "fisch/vx/container.def"
}

template <typename ContainerT>
void test_playback_program_builder_write_api()
{
	PlaybackProgramBuilder builder;

	ContainerT config;
	std::vector<ContainerT> vector_config{config};

	typename ContainerT::coordinate_type coord;
	std::vector<typename ContainerT::coordinate_type> vector_coord{coord};

	// single write
	builder.write(coord, config);

	// vector write
	builder.write(vector_coord, vector_config);
}

TEST(PlaybackProgramBuilder, WriteAPI)
{
#define PLAYBACK_CONTAINER(Name, Type)                                                             \
	if constexpr (IsWritable<Type>::value) {                                                       \
		EXPECT_NO_THROW(test_playback_program_builder_write_api<Type>());                          \
	} else {                                                                                       \
		EXPECT_THROW(test_playback_program_builder_write_api<Type>(), std::logic_error);           \
	}

#include "fisch/vx/container.def"
}

TEST(PlaybackProgram, General)
{
	EXPECT_NO_THROW(PlaybackProgram());

	PlaybackProgram program;
	EXPECT_TRUE(program.empty());

	PlaybackProgramBuilder builder;
	builder.write(TimerOnDLS(), Timer());
	EXPECT_FALSE(builder.empty());
	auto const nonempty_program = builder.done();
	EXPECT_FALSE(nonempty_program->empty());
}

TEST(PlaybackProgramBuilder, General)
{
	EXPECT_NO_THROW(PlaybackProgramBuilder());

	PlaybackProgramBuilder builder;
	EXPECT_TRUE(builder.empty());

	builder.write(TimerOnDLS(), Timer());
	EXPECT_FALSE(builder.empty());
	std::stringstream ss_builder;
	ss_builder << builder;

	std::shared_ptr<PlaybackProgram> small_program = builder.done();

	EXPECT_EQ(small_program->get_to_fpga_messages().size(), 1);

	// printable
	std::stringstream ss;
	ss << *small_program;
	EXPECT_GT(ss.str().size(), 0);
	std::stringstream ss_expected;
	ss_expected << hxcomm::vx::UTMessageToFPGA<hxcomm::vx::instruction::timing::Setup>();
	EXPECT_NE(ss_builder.str().find(ss_expected.str()), std::string::npos);
	EXPECT_NE(ss.str().find(ss_expected.str()), std::string::npos);

	// new program started on last done() call
	auto empty_program = builder.done();
	EXPECT_NE(empty_program, small_program);
	EXPECT_NE(*empty_program, *small_program);
}

TEST(PlaybackProgramBuilder, WriteSingle)
{
	PlaybackProgramBuilder builder;
	Omnibus config;
	OmnibusAddress coord;

	EXPECT_NO_THROW(builder.write(coord, config));

	auto program = builder.done();
	auto program_to_fpga_messages = program->get_to_fpga_messages();
	std::vector<hxcomm::vx::UTMessageToFPGAVariant> expected_to_fpga_messages;
	UTMessageToFPGABackEmplacer emplacer(expected_to_fpga_messages);
	config.encode_write(coord, emplacer);
	EXPECT_EQ(expected_to_fpga_messages, program_to_fpga_messages);
}

TEST(PlaybackProgramBuilder, WriteMultiple)
{
	PlaybackProgramBuilder builder;
	std::vector<OmnibusAddress> addresses{OmnibusAddress(0), OmnibusAddress(1)};
	std::vector<Omnibus> words{Omnibus(Omnibus::Value(0)), Omnibus(Omnibus::Value(1))};

	auto const ref_addresses = addresses;
	auto const ref_words = words;

	EXPECT_NO_THROW(builder.write(addresses, words));

	// too much addresses
	addresses.push_back(OmnibusAddress(2));
	EXPECT_THROW(builder.write(addresses, words), std::runtime_error);

	// too much words
	words.push_back(Omnibus(Omnibus::Value(2)));
	words.push_back(Omnibus(Omnibus::Value(3)));
	EXPECT_THROW(builder.write(addresses, words), std::runtime_error);

	auto program = builder.done();
	auto program_to_fpga_messages = program->get_to_fpga_messages();

	std::vector<hxcomm::vx::UTMessageToFPGAVariant> expected_to_fpga_messages;
	UTMessageToFPGABackEmplacer emplacer(expected_to_fpga_messages);
	for (size_t i = 0; i < ref_addresses.size(); ++i) {
		ref_words.at(i).encode_write(ref_addresses.at(i), emplacer);
	}
	EXPECT_EQ(expected_to_fpga_messages, program_to_fpga_messages);
}

TEST(PlaybackProgramBuilder, ReadSingle)
{
	PlaybackProgramBuilder builder;

	auto coord = OmnibusAddress(13);
	EXPECT_NO_THROW(builder.read(coord));
	builder.done(); // reset

	ContainerTicket<Omnibus> ticket = builder.read(coord);

	EXPECT_FALSE(ticket.valid());
	EXPECT_THROW(ticket.get(), std::runtime_error);

	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x12345678));
	auto program = builder.done();

	auto program_to_fpga_messages = program->get_to_fpga_messages();

	std::vector<hxcomm::vx::UTMessageToFPGAVariant> expected_to_fpga_messages;
	UTMessageToFPGABackEmplacer emplacer(expected_to_fpga_messages);
	Omnibus::encode_read(coord, emplacer);
	EXPECT_EQ(expected_to_fpga_messages, program_to_fpga_messages);

	program->push_from_fpga_message(from_fpga_message);

	EXPECT_TRUE(program->tickets_valid());
	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	auto result = ticket.get();
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(
	    result.at(0).get(), Omnibus::Value(static_cast<uint32_t>(from_fpga_message.decode())));
}

TEST(PlaybackProgramBuilder, ReadMultiple)
{
	PlaybackProgramBuilder builder;
	std::vector<OmnibusAddress> addresses{OmnibusAddress(0), OmnibusAddress(1)};

	EXPECT_NO_THROW(builder.read(addresses));
	builder.done(); // reset

	ContainerTicket<Omnibus> ticket = builder.read(addresses);

	EXPECT_FALSE(ticket.valid());
	EXPECT_THROW(ticket.get(), std::runtime_error);

	auto program = builder.done();

	auto program_to_fpga_messages = program->get_to_fpga_messages();

	std::vector<hxcomm::vx::UTMessageToFPGAVariant> expected_to_fpga_messages;
	UTMessageToFPGABackEmplacer emplacer(expected_to_fpga_messages);
	for (size_t i = 0; i < addresses.size(); ++i) {
		Omnibus::encode_read(addresses.at(i), emplacer);
	}
	EXPECT_EQ(expected_to_fpga_messages, program_to_fpga_messages);

	// too little messages
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_1(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x12345678));
	program->push_from_fpga_message(from_fpga_message_1);
	EXPECT_FALSE(ticket.valid());
	EXPECT_THROW(ticket.get(), std::runtime_error);
	EXPECT_FALSE(program->tickets_valid());

	// enough messages
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_2(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x87654321));
	program->push_from_fpga_message(from_fpga_message_2);
	EXPECT_TRUE(program->tickets_valid());
	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());
	auto result = ticket.get();
	EXPECT_EQ(result.size(), addresses.size());
	EXPECT_EQ(
	    result.at(0).get(), Omnibus::Value(static_cast<uint32_t>(from_fpga_message_1.decode())));
	EXPECT_EQ(
	    result.at(1).get(), Omnibus::Value(static_cast<uint32_t>(from_fpga_message_2.decode())));
}

TEST(PlaybackProgramBuilder, ReadMultipleTickets)
{
	PlaybackProgramBuilder builder;
	std::vector<OmnibusAddress> addresses{OmnibusAddress(0), OmnibusAddress(1)};

	EXPECT_NO_THROW(builder.read(addresses));
	builder.done(); // reset

	std::vector<ContainerTicket<Omnibus>> tickets;
	for (auto address : addresses) {
		tickets.push_back(builder.read(address));
	}

	for (auto ticket : tickets) {
		EXPECT_FALSE(ticket.valid());
		EXPECT_THROW(ticket.get(), std::runtime_error);
	}

	auto program = builder.done();

	// only first ticket valid
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_1(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x12345678));
	program->push_from_fpga_message(from_fpga_message_1);
	EXPECT_TRUE(tickets.at(0).valid());
	EXPECT_FALSE(tickets.at(1).valid());
	EXPECT_NO_THROW(tickets.at(0).get());
	EXPECT_THROW(tickets.at(1).get(), std::runtime_error);
	EXPECT_FALSE(program->tickets_valid());
	{
		auto result_1 = tickets.at(0).get();
		EXPECT_EQ(result_1.size(), 1);
		EXPECT_EQ(
		    result_1.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_1.decode())));
	}

	// both tickets valid
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_2(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x87654321));
	program->push_from_fpga_message(from_fpga_message_2);
	for (auto ticket : tickets) {
		EXPECT_TRUE(ticket.valid());
		EXPECT_NO_THROW(ticket.get());
	}
	EXPECT_TRUE(program->tickets_valid());
	{
		auto result_1 = tickets.at(0).get();
		auto result_2 = tickets.at(1).get();
		EXPECT_EQ(result_1.size(), 1);
		EXPECT_EQ(result_2.size(), 1);
		EXPECT_EQ(
		    result_1.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_1.decode())));
		EXPECT_EQ(
		    result_2.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_2.decode())));
	}
}

TEST(PlaybackProgramBuilder, ReadMultipleVectorTickets)
{
	PlaybackProgramBuilder builder;
	std::vector<std::vector<OmnibusAddress>> addresses{{OmnibusAddress(0), OmnibusAddress(1)},
	                                                   {OmnibusAddress(2), OmnibusAddress(3)}};

	std::vector<ContainerTicket<Omnibus>> tickets;
	for (auto address : addresses) {
		tickets.push_back(builder.read(address));
	}

	for (auto ticket : tickets) {
		EXPECT_FALSE(ticket.valid());
		EXPECT_THROW(ticket.get(), std::runtime_error);
	}

	auto program = builder.done();

	// not enough messages
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_1(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x12345678));
	program->push_from_fpga_message(from_fpga_message_1);
	for (auto ticket : tickets) {
		EXPECT_FALSE(ticket.valid());
		EXPECT_THROW(ticket.get(), std::runtime_error);
	}

	// first ticket valid
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_2(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x87654321));
	program->push_from_fpga_message(from_fpga_message_2);
	EXPECT_TRUE(tickets.at(0).valid());
	EXPECT_NO_THROW(tickets.at(0).get());
	EXPECT_FALSE(tickets.at(1).valid());
	EXPECT_THROW(tickets.at(1).get(), std::runtime_error);
	EXPECT_FALSE(program->tickets_valid());
	{
		auto result_1 = tickets.at(0).get();
		EXPECT_EQ(
		    result_1.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_1.decode())));
		EXPECT_EQ(
		    result_1.at(1).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_2.decode())));
	}

	// second ticket still not valid
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_3(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x43218765));
	program->push_from_fpga_message(from_fpga_message_3);
	EXPECT_TRUE(tickets.at(0).valid());
	EXPECT_NO_THROW(tickets.at(0).get());
	EXPECT_FALSE(tickets.at(1).valid());
	EXPECT_THROW(tickets.at(1).get(), std::runtime_error);
	EXPECT_FALSE(program->tickets_valid());
	{
		auto result_1 = tickets.at(0).get();
		EXPECT_EQ(
		    result_1.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_1.decode())));
		EXPECT_EQ(
		    result_1.at(1).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_2.decode())));
	}

	// both tickets valid
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message_4(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x56781234));
	program->push_from_fpga_message(from_fpga_message_4);
	for (auto ticket : tickets) {
		EXPECT_TRUE(ticket.valid());
		EXPECT_NO_THROW(ticket.get());
	}
	EXPECT_TRUE(program->tickets_valid());
	{
		auto result_1 = tickets.at(0).get();
		auto result_2 = tickets.at(1).get();
		EXPECT_EQ(
		    result_1.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_1.decode())));
		EXPECT_EQ(
		    result_1.at(1).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_2.decode())));
		EXPECT_EQ(
		    result_2.at(0).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_3.decode())));
		EXPECT_EQ(
		    result_2.at(1).get(),
		    Omnibus::Value(static_cast<uint32_t>(from_fpga_message_4.decode())));
	}
}

TEST(PlaybackProgramBuilder, ClearFromFPGAMessages)
{
	PlaybackProgramBuilder builder;
	OmnibusAddress address;

	EXPECT_NO_THROW(builder.read(address));
	builder.done(); // reset

	auto ticket = builder.read(address);

	EXPECT_FALSE(ticket.valid());
	EXPECT_THROW(ticket.get(), std::runtime_error);

	auto program = builder.done();

	// ticket valid
	hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>
	    from_fpga_message(hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(0x12345678));
	program->push_from_fpga_message(from_fpga_message);
	EXPECT_TRUE(ticket.valid());
	EXPECT_NO_THROW(ticket.get());

	// after clear not valid
	program->clear_from_fpga_messages();
	EXPECT_FALSE(ticket.valid());
	EXPECT_THROW(ticket.get(), std::runtime_error);
}

TEST(PlaybackProgramBuilder, WaitUntil)
{
	using namespace hxcomm::vx;
	using namespace hxcomm::vx::instruction;

	PlaybackProgramBuilder builder;

	EXPECT_NO_THROW(builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(0x123))));

	auto program = builder.done();
	auto program_to_fpga_messages = program->get_to_fpga_messages();
	EXPECT_EQ(program_to_fpga_messages.size(), 1);

	auto expected_to_fpga_message =
	    UTMessageToFPGA<timing::WaitUntil>(timing::WaitUntil::Payload(0x123));
	EXPECT_EQ(
	    expected_to_fpga_message,
	    std::get<decltype(expected_to_fpga_message)>(program_to_fpga_messages.at(0)));
}

TEST(PlaybackProgram, CerealizeCoverage)
{
	using namespace fisch::vx;

	PlaybackProgramBuilder builder;
	builder.read(Omnibus::coordinate_type());             // add omnibus instructions
	builder.read(OmnibusChipOverJTAG::coordinate_type()); // add jtag instructions

	auto obj1 = builder.done();
	obj1->push_from_fpga_message(
	    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>());
	obj1->push_from_fpga_message(
	    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::jtag_from_hicann::Data>());

	std::shared_ptr<PlaybackProgram> obj2; // empty program pointer
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
	EXPECT_TRUE(obj1);
	EXPECT_TRUE(obj2);
	ASSERT_EQ(*obj1, *obj2);
}

TEST(PlaybackProgramBuilder, MergeBack)
{
	PlaybackProgramBuilder builder;
	PlaybackProgramBuilder other;

	EXPECT_NO_THROW(builder.merge_back(other));

	other.read(OmnibusAddress());
	EXPECT_NO_THROW(builder.merge_back(other));
	EXPECT_FALSE(builder.empty());
	EXPECT_TRUE(other.empty());

	other.write(OmnibusAddress(), Omnibus());
	EXPECT_NO_THROW(builder.merge_back(other));
	EXPECT_FALSE(builder.empty());
	EXPECT_TRUE(other.empty());

	PlaybackProgramBuilder reference;
	reference.read(OmnibusAddress());
	reference.write(OmnibusAddress(), Omnibus());

	EXPECT_EQ(*(builder.done()), *(reference.done()));
}

TEST(PlaybackProgramBuilder, MergeFront)
{
	PlaybackProgramBuilder builder;
	PlaybackProgramBuilder other;

	EXPECT_NO_THROW(builder.merge_front(other));

	other.read(OmnibusAddress());
	EXPECT_NO_THROW(builder.merge_front(other));
	EXPECT_FALSE(builder.empty());
	EXPECT_TRUE(other.empty());

	other.write(OmnibusAddress(), Omnibus());
	EXPECT_NO_THROW(builder.merge_front(other));
	EXPECT_FALSE(builder.empty());
	EXPECT_TRUE(other.empty());

	PlaybackProgramBuilder reference;
	reference.write(OmnibusAddress(), Omnibus());
	reference.read(OmnibusAddress());

	EXPECT_EQ(*(builder.done()), *(reference.done()));
}

TEST(PlaybackProgramBuilder, CopyBack)
{
	PlaybackProgramBuilder builder;
	PlaybackProgramBuilder other;

	EXPECT_NO_THROW(builder.copy_back(other));

	other.read(OmnibusAddress());
	EXPECT_THROW(builder.copy_back(other), std::runtime_error);
	other.done();
	EXPECT_TRUE(builder.empty());

	other.write(OmnibusAddress(), Omnibus());
	EXPECT_NO_THROW(builder.copy_back(other));
	EXPECT_FALSE(builder.empty());
	EXPECT_EQ(*(builder.done()), *(other.done()));
}

TEST(PlaybackProgramBuilder, PerformanceOmnibusRead)
{
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		std::vector<halco::hicann_dls::vx::OmnibusAddress> addresses;
		size_t const num = hate::math::pow(2, p);
		for (size_t i = 0; i < num; ++i) {
			addresses.push_back(halco::hicann_dls::vx::OmnibusAddress(i));
		}

		PlaybackProgramBuilder builder;
		hate::Timer timer;
		[[maybe_unused]] auto ticket = builder.read(addresses);
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());
		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 30.);
}

TEST(PlaybackProgramBuilder, PerformanceOmnibusWrite)
{
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		std::vector<halco::hicann_dls::vx::OmnibusAddress> addresses;
		std::vector<Omnibus> container;
		size_t const num = hate::math::pow(2, p);
		for (size_t i = 0; i < num; ++i) {
			addresses.push_back(halco::hicann_dls::vx::OmnibusAddress(i));
			container.push_back(Omnibus(Omnibus::Value(i)));
		}

		PlaybackProgramBuilder builder;
		hate::Timer timer;
		builder.write(addresses, container);
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());
		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 15.);
}

TEST(PlaybackProgramBuilder, PerformanceWaitUntilWrite)
{
	std::mt19937 rng(std::random_device{}());
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		std::vector<halco::hicann_dls::vx::WaitUntilOnFPGA> addresses;
		std::vector<WaitUntil> container;
		size_t const num = hate::math::pow(2, p);
		for (size_t i = 0; i < num; ++i) {
			addresses.push_back(halco::hicann_dls::vx::WaitUntilOnFPGA());
			container.push_back(fill_random<WaitUntil>(rng));
		}

		PlaybackProgramBuilder builder;
		hate::Timer timer;
		builder.write(addresses, container);
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());
		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 15.);
}

#define TEST_PlaybackProgramBuilder_PerformanceSpikePackNToChipWrite(N)                            \
	TEST(PlaybackProgramBuilder, PerformanceSpikePack##N##ToChipWrite)                             \
	{                                                                                              \
		std::mt19937 rng(std::random_device{}());                                                  \
		auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");             \
		constexpr size_t max_pow = 25;                                                             \
                                                                                                   \
		double rate_mhz = 0.;                                                                      \
		for (size_t p = 0; p < max_pow; ++p) {                                                     \
			std::vector<halco::hicann_dls::vx::SpikePack##N##ToChipOnDLS> addresses;               \
			std::vector<SpikePack##N##ToChip> container;                                           \
			size_t const num = hate::math::pow(2, p);                                              \
			for (size_t i = 0; i < num; ++i) {                                                     \
				addresses.push_back(halco::hicann_dls::vx::SpikePack##N##ToChipOnDLS());           \
				container.push_back(fill_random<SpikePack##N##ToChip>(rng));                       \
			}                                                                                      \
                                                                                                   \
			PlaybackProgramBuilder builder;                                                        \
			hate::Timer timer;                                                                     \
			builder.write(addresses, container);                                                   \
			rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());             \
			LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");      \
		}                                                                                          \
		EXPECT_GE(rate_mhz, 15.);                                                                  \
	}
// clang-format off
TEST_PlaybackProgramBuilder_PerformanceSpikePackNToChipWrite(1)
TEST_PlaybackProgramBuilder_PerformanceSpikePackNToChipWrite(2)
TEST_PlaybackProgramBuilder_PerformanceSpikePackNToChipWrite(3)
// clang-format on
#undef TEST_PlaybackProgramBuilder_PerformanceSpikePackNToChipWrite

            TEST(PlaybackProgram, PerformanceOmnibusReadProcessMessages)
{
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		std::vector<halco::hicann_dls::vx::OmnibusAddress> addresses;
		std::vector<hxcomm::vx::UTMessageFromFPGAVariant> from_fpga_messages;
		static_assert(Omnibus::decode_ut_message_count == 1);
		size_t const num = hate::math::pow(2, p);
		for (size_t i = 0; i < num; ++i) {
			addresses.push_back(halco::hicann_dls::vx::OmnibusAddress(i));
			from_fpga_messages.push_back(
			    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>(
			        hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(i)));
		}

		PlaybackProgramBuilder builder;
		[[maybe_unused]] auto ticket = builder.read(addresses);
		auto program = builder.done();
		hate::Timer timer;
		for (auto const& message : from_fpga_messages) {
			program->push_from_fpga_message(message);
		}
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());
		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 30.);
}

TEST(PlaybackProgram, PerformanceOmnibusReadDecode)
{
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		std::vector<halco::hicann_dls::vx::OmnibusAddress> addresses;
		std::vector<hxcomm::vx::UTMessageFromFPGAVariant> from_fpga_messages;
		static_assert(Omnibus::decode_ut_message_count == 1);
		size_t const num = hate::math::pow(2, p);
		for (size_t i = 0; i < num; ++i) {
			addresses.push_back(halco::hicann_dls::vx::OmnibusAddress(i));
			from_fpga_messages.push_back(
			    hxcomm::vx::UTMessageFromFPGA<hxcomm::vx::instruction::omnibus_from_fpga::Data>(
			        hxcomm::vx::instruction::omnibus_from_fpga::Data::Payload(i)));
		}

		PlaybackProgramBuilder builder;
		auto ticket = builder.read(addresses);
		auto program = builder.done();
		for (auto const& message : from_fpga_messages) {
			program->push_from_fpga_message(message);
		}
		hate::Timer timer;
		[[maybe_unused]] auto container = ticket.get();
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());
		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 30.);
}

TEST(PlaybackProgramBuilder, PerformanceSpiketrainGeneratedWrite)
{
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		SpikePack1ToChipOnDLS spike_coordinate;
		WaitUntilOnFPGA wait_until_coordinate;
		size_t const num = hate::math::pow(2, p);

		hate::Timer timer;
		PlaybackProgramBuilder builder;
		// regular spike-train with 10 cycles ISI and SpikeLabel()
		builder.write(TimerOnDLS(), Timer());
		for (size_t i = 0; i < num; ++i) {
			builder.write(wait_until_coordinate, WaitUntil(WaitUntil::Value(i * 10)));
			builder.write(spike_coordinate, SpikePack1ToChip());
		}
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());

		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 8.);
}

TEST(PlaybackProgramBuilder, PerformanceSpiketrainStoredWrite)
{
	auto logger = log4cxx::Logger::getLogger("fisch.Test_PlaybackProgramBuilder");
	constexpr size_t max_pow = 25;

	double rate_mhz = 0.;
	for (size_t p = 0; p < max_pow; ++p) {
		size_t const num = hate::math::pow(2, p);
		std::vector<SpikePack1ToChip> spikes;
		std::vector<WaitUntil> times;
		SpikePack1ToChipOnDLS spike_coordinate;
		WaitUntilOnFPGA wait_until_coordinate;
		// regular spike-train with 10 cycles ISI and random SpikeLabel
		for (size_t i = 0; i < num; ++i) {
			spikes.push_back(SpikePack1ToChip());
			times.push_back(WaitUntil(WaitUntil::Value(i * 10)));
		}

		hate::Timer timer;
		PlaybackProgramBuilder builder;
		builder.write(TimerOnDLS(), Timer());
		for (size_t i = 0; i < num; ++i) {
			builder.write(wait_until_coordinate, times[i]);
			builder.write(spike_coordinate, spikes[i]);
		}
		rate_mhz = static_cast<double>(num) / static_cast<double>(timer.get_us());

		LOG4CXX_INFO(logger, num << ": " << timer.print() << ", " << rate_mhz << " MHz");
	}
	EXPECT_GE(rate_mhz, 8.);
}
