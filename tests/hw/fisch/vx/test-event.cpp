#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/omnibus_constants.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/run.h"
#include "fisch/vx/systime.h"
#include "fisch/vx/timer.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"
#include "hxcomm/vx/connection_from_env.h"

using namespace halco::common;
using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

// TODO: move to haldls in order to get sane init routine (Issue #3220)
TEST(SpikePack1ToChip, Loopback)
{
	PlaybackProgramBuilder builder;

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
		JTAGPLLRegister reg(JTAGPLLRegister::Value(0x118d8181));
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

	builder.write(SystimeSyncOnFPGA(), SystimeSync(SystimeSync::Value(true)));

	// wait until systime init is finished
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(85 * fpga_clock_cycles_per_us)));

	// enable all counters
	{
		Omnibus config(Omnibus::Value(0xfff0));
		OmnibusAddress coord(0x0013'0000);
		builder.write(coord, config);
	}

	// enable loopback on all l2in channels
	for (size_t i = 0; i < 4; ++i) {
		Omnibus config(Omnibus::Value(0x8000'0000));
		OmnibusAddress coord(0x0013'0039 + i * 9);
		builder.write(coord, config);
	}

	constexpr size_t num_spikes = 1000;

	std::vector<SpikeLabel> to_fpga_spike_labels;
	for (size_t i = 0; i < num_spikes; ++i) {
		SpikePack1ToChip spike({SpikeLabel(i)});
		builder.write(SpikePack1ToChipOnDLS(), spike);
		builder.write(TimerOnDLS(), Timer());
		builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(10)));
		to_fpga_spike_labels.push_back(spike.get().at(0));
	}

	builder.write(TimerOnDLS(), Timer());
	builder.write(WaitUntilOnFPGA(), WaitUntil(WaitUntil::Value(1000)));
	auto program = builder.done();

	auto connection = hxcomm::vx::get_connection_from_env();
	run(connection, program);

	auto spikes = program->get_spikes();

	EXPECT_LE(spikes.size(), num_spikes * 1.2);
	EXPECT_GT(spikes.size(), num_spikes * 0.5); // issue #3959

	unsigned num_unexpected_spikes = 0;
	for (auto spike : spikes) {
		auto it =
		    std::find(to_fpga_spike_labels.cbegin(), to_fpga_spike_labels.cend(), spike.label);

		if (it == to_fpga_spike_labels.cend()) {
			num_unexpected_spikes++;
		}
	}
	EXPECT_LE(num_unexpected_spikes, num_spikes * 0.2);
}
