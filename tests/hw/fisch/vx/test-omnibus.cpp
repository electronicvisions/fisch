#include <random>
#include <gtest/gtest.h>

#include "fisch/vx/barrier.h"
#include "fisch/vx/constants.h"
#include "fisch/vx/container_ticket.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/run.h"
#include "fisch/vx/timer.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "connection.h"

using namespace halco::common;
using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

constexpr uint32_t ppu_top_subtree_address{0x0200'0000};
constexpr uint32_t top_ppu_base_address{0x0080'0000 | ppu_top_subtree_address};
constexpr uint32_t synram_synacc_top_base_address{(top_ppu_base_address | (1 << 22))};
constexpr uint32_t synram_synapse_top_base_address{synram_synacc_top_base_address + 0x000f'0000};


template <typename Connection>
bool is_HXv2(Connection& connection)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(true));
	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
	builder.write(ResetChipOnDLS(), ResetChip(false));
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
	if (jtag_id == 0x048580AF) {
		return false;
	} else if (jtag_id == 0x248580AF) {
		return true;
	} else {
		throw std::runtime_error("Unknown JTAGID found.");
	}
}


TEST(OmnibusChip, ByteEnables)
{
	auto connection = generate_test_connection();

	if (is_HXv2(connection)) {
		PlaybackProgramBuilder builder;

		// ------ start of setup chip ------
		builder.write(ResetChipOnDLS(), ResetChip(true));
		builder.write(TimerOnDLS(), Timer());
		builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
		builder.write(ResetChipOnDLS(), ResetChip(false));
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
		builder.write(
		    WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(100 * fpga_clock_cycles_per_us)));

		// configure FPGA-side PHYs
		for (auto i : iter_all<PhyConfigFPGAOnDLS>()) {
			OmnibusFPGA config(OmnibusData(0x0020'4040));
			OmnibusFPGAAddress coord(0x0600'0000 + i);
			builder.write(coord, config);
		}

		// enable FPGA-side PHYs
		{
			OmnibusFPGA config(OmnibusData(0xff));
			OmnibusFPGAAddress coord(0x0400'0000);
			builder.write(coord, config);
		}

		// configure Chip-side PHYs
		for (auto coord : iter_all<JTAGPhyRegisterOnDLS>()) {
			JTAGPhyRegister config(JTAGPhyRegister::Value(0x0020'4000));
			builder.write(coord, config);
		}

		// enable Chip-side PHYs
		{
			OmnibusChipOverJTAG config(OmnibusData(0xff));
			OmnibusChipOverJTAGAddress coord(0x0004'0000);
			builder.write(coord, config);
		}

		// wait until highspeed is up
		builder.write(TimerOnDLS(), Timer());
		builder.write(
		    WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(80 * fpga_clock_cycles_per_us)));
		// ------ end of setup chip ------

		// generate random address in top-PPU Synram
		std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<uint32_t> d(
		    0, SynapseQuadColumnOnDLS::size * (SynapseRowOnSynram::size + 2 /* switch rows */) *
		           2 /* weights + addresses */);
		OmnibusChipAddress address(synram_synapse_top_base_address + d(rng));

		OmnibusChip initial(OmnibusData(0x12345678));
		builder.write(address, initial);

		auto ticket_initial = builder.read(address);

		std::vector<ContainerTicket<OmnibusChip>> tickets;

		for (size_t i = 0; i < std::tuple_size<OmnibusChip::ByteEnables>::value; ++i) {
			OmnibusChip::ByteEnables byte_enables;
			byte_enables.fill(false);

			byte_enables.at(EntryOnQuad(i)) = true;

			OmnibusChip only_one_byte(OmnibusData(0x87654321), byte_enables);
			builder.write(address, only_one_byte);
			tickets.push_back(builder.read(address));
		}

		builder.write(BarrierOnFPGA(), Barrier(Barrier::Value::omnibus));
		auto program = builder.done();

		run(connection, program);

		EXPECT_TRUE(ticket_initial.valid());
		EXPECT_EQ(ticket_initial.get().at(0).get(), OmnibusData(0x12345678));

		std::vector<OmnibusData> expectations;
		expectations.push_back(OmnibusData(0x12345621));
		expectations.push_back(OmnibusData(0x12344321));
		expectations.push_back(OmnibusData(0x12654321));
		expectations.push_back(OmnibusData(0x87654321));

		for (size_t i = 0; i < tickets.size(); ++i) {
			EXPECT_TRUE(tickets.at(i).valid());
			EXPECT_EQ(tickets.at(i).get().at(0).get(), expectations.at(i));
		}
	}
}
