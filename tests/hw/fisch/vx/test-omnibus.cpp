#include <random>
#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/constants.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/run.h"
#include "fisch/vx/timer.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/connection_from_env.h"

using namespace halco::common;
using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

constexpr uint32_t ppu_top_subtree_address{0x0200'0000};
constexpr uint32_t top_ppu_base_address{0x0080'0000 | ppu_top_subtree_address};
constexpr uint32_t synram_synacc_top_base_address{(top_ppu_base_address | (1 << 22))};
constexpr uint32_t synram_synapse_top_base_address{synram_synacc_top_base_address + 0x000f'0000};


template <typename Connection>
size_t get_hx_revision(Connection& connection)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(true)));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(false)));
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100)));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	auto const jtag_id_ticket = builder.read(JTAGIdCodeOnDLS());
	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::jtag));

	{
		auto program = builder.done();
		run(connection, program);
	}
	auto const jtag_id = jtag_id_ticket.get().at(0).get();
	auto const hx_revision = jtag_id >> 28; // bits 28-31 represent the chip revision

	if (hx_revision > 3) {
		throw std::runtime_error("Unknown HX revision found.");
	}

	return hx_revision;
}


TEST(Omnibus, ByteEnables)
{
	auto connection = hxcomm::vx::get_connection_from_env();

	{
		auto const hx_revision = get_hx_revision(connection);
		if (hx_revision < 2) {
			GTEST_SKIP() << "Test not supported on revision " << hx_revision;
		}
	}

	PlaybackProgramBuilder builder;

	// ------ start of setup chip ------
	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(true)));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(ResetChip::Value(false)));
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100)));

	builder.write(JTAGClockScalerOnDLS(), JTAGClockScaler(JTAGClockScaler::Value(3)));
	builder.write(ResetJTAGTapOnDLS(), ResetJTAGTap());

	// configure PLL, without, events are corrupted by MADC events or broken L2
	// configure ADPLLs
	for (auto coord : iter_all<ADPLLOnDLS>()) {
		JTAGPLLRegister reg0(JTAGPLLRegister::Value(0x2a20a902));
		JTAGPLLRegister reg1(JTAGPLLRegister::Value(0xc0cbf200));

		builder.write(JTAGPLLRegisterOnDLS(coord * 2), reg0);
		builder.write(JTAGPLLRegisterOnDLS(coord * 2 + 1), reg1);
	}

	// configure PLLClockOutputBlock
	{
		JTAGPLLRegister reg(JTAGPLLRegister::Value(0x918d8181));
		builder.write(JTAGPLLRegisterOnDLS(4), reg);
	}

	// wait until PLL reconfiguration and Omnibus is up
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100 * fpga_clock_cycles_per_us)));

	// configure FPGA-side PHYs
	for (auto i : iter_all<PhyConfigFPGAOnDLS>()) {
		Omnibus config(Omnibus::Value(0x0020'4040));
		OmnibusAddress coord{phy_omnibus_mask + i};
		builder.write(coord, config);
	}

	// enable FPGA-side PHYs
	{
		Omnibus config(Omnibus::Value(0xff));
		builder.write(ut_omnibus_mask, config);
	}

	// configure Chip-side PHYs
	for (auto coord : iter_all<JTAGPhyRegisterOnDLS>()) {
		JTAGPhyRegister config(JTAGPhyRegister::Value(0x0020'4000));
		builder.write(coord, config);
	}

	// enable Chip-side PHYs
	{
		OmnibusChipOverJTAG config(OmnibusChipOverJTAG::Value(0xff));
		OmnibusChipOverJTAGAddress coord(0x0004'0000);
		builder.write(coord, config);
	}

	// wait until highspeed is up
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(80 * fpga_clock_cycles_per_us)));

	// configure synram
	builder.write(OmnibusAddress(0x0200'0000), Omnibus(Omnibus::Value(8 << 4 | 8)));
	builder.write(OmnibusAddress(0x0200'0001), Omnibus(Omnibus::Value(3 << 8 | 3)));
	builder.write(OmnibusAddress(0x0200'0002), Omnibus(Omnibus::Value(4)));
	// ------ end of setup chip ------

	// generate random address in top-PPU Synram
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<uint32_t> d(
	    0, 64 /* SynapseQuadColumnOnDLS::size */ *
	           (256 /* SynapseRowOnSynram::size */ + 2 /* switch rows */) *
	           2 /* weights + addresses */);
	OmnibusAddress address(synram_synapse_top_base_address + d(rng));

	Omnibus initial(Omnibus::Value(0x12345678));
	builder.write(address, initial);

	auto ticket_initial = builder.read(address);

	std::vector<ContainerTicket<Omnibus>> tickets;

	for (size_t i = 0; i < std::tuple_size<Omnibus::Value::ByteEnables>::value; ++i) {
		Omnibus::Value::ByteEnables byte_enables;
		byte_enables.fill(false);

		byte_enables.at(EntryOnQuad(i)) = true;

		Omnibus only_one_byte(Omnibus::Value(0x87654321, byte_enables));
		builder.write(address, only_one_byte);
		tickets.push_back(builder.read(address));
	}

	builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
	auto program = builder.done();

	run(connection, program);

	EXPECT_TRUE(ticket_initial.valid());
	EXPECT_EQ(ticket_initial.get().at(0).get(), Omnibus::Value(0x12345678));

	std::vector<Omnibus::Value> expectations;
	expectations.push_back(Omnibus::Value(0x12345621));
	expectations.push_back(Omnibus::Value(0x12344321));
	expectations.push_back(Omnibus::Value(0x12654321));
	expectations.push_back(Omnibus::Value(0x87654321));

	for (size_t i = 0; i < tickets.size(); ++i) {
		EXPECT_TRUE(tickets.at(i).valid());
		EXPECT_EQ(tickets.at(i).get().at(0).get(), expectations.at(i));
	}
}
