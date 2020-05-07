#include <gtest/gtest.h>

#include "fisch/vx/constants.h"
#include "fisch/vx/jtag.h"
#include "fisch/vx/omnibus.h"
#include "fisch/vx/playback_program.h"
#include "fisch/vx/playback_program_builder.h"
#include "fisch/vx/reset.h"
#include "fisch/vx/systime.h"
#include "fisch/vx/timer.h"
#include "halco/common/iter_all.h"
#include "halco/hicann-dls/vx/coordinates.h"

#include "executor.h"

using namespace halco::common;
using namespace halco::hicann_dls::vx;
using namespace fisch::vx;

// TODO: move to haldls in order to get sane init routine (Issue #3220)
TEST(SpikePack1ToChip, Loopback)
{
	PlaybackProgramBuilder builder;

	builder.write(ResetChipOnDLS(), ResetChip(true));
	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(10));
	builder.write(ResetChipOnDLS(), ResetChip(false));
	builder.wait_until(TimerOnDLS(), Timer::Value(100));

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
	builder.wait_until(TimerOnDLS(), Timer::Value(100 * fpga_clock_cycles_per_us));

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
	builder.wait_until(TimerOnDLS(), Timer::Value(80 * fpga_clock_cycles_per_us));

	builder.write(SystimeSyncOnFPGA(), SystimeSync(true));

	// wait until systime init is finished
	builder.wait_until(TimerOnDLS(), Timer::Value(85 * fpga_clock_cycles_per_us));

	// enable all counters
	{
		OmnibusChip config(OmnibusData(0xfff0));
		OmnibusChipAddress coord(0x0013'0000);
		builder.write(coord, config);
	}

	// enable loopback on all l2in channels
	for (size_t i = 0; i < 4; ++i) {
		OmnibusChip config(OmnibusData(0x8000'0000));
		OmnibusChipAddress coord(0x0013'0039 + i * 9);
		builder.write(coord, config);
	}

	constexpr size_t num_spikes = 1000;

	std::vector<SpikeLabel> to_fpga_spike_labels;
	for (size_t i = 0; i < num_spikes; ++i) {
		SpikePack1ToChip spike({SpikeLabel(i)});
		builder.write(SpikePack1ToChipOnDLS(), spike);
		builder.write(TimerOnDLS(), Timer());
		builder.wait_until(TimerOnDLS(), Timer::Value(10));
		to_fpga_spike_labels.push_back(spike.get_labels().at(0));
	}

	builder.write(TimerOnDLS(), Timer());
	builder.wait_until(TimerOnDLS(), Timer::Value(1000));
	auto program = builder.done();

	auto executor = generate_playback_program_test_executor();
	executor.run(program);

	auto spikes = program->get_spikes();

	EXPECT_LE(spikes.size(), num_spikes);
	EXPECT_GT(spikes.size(), 0);

	for (auto spike : spikes) {
		auto it = std::find(
		    to_fpga_spike_labels.cbegin(), to_fpga_spike_labels.cend(),
		    spike.get_spike().get_label());
		EXPECT_TRUE(it != to_fpga_spike_labels.cend()) << "Received spike not sent.";
	}
}
