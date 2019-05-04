import unittest
import random

import pyfisch_vx as fisch
import pyhalco_hicann_dls_vx as halco

random.seed(1234)

class PPUWordWriteRead(unittest.TestCase):
    def test_read_write_read(self):
        builder = fisch.PlaybackProgramBuilder()

        # reset chip
        builder.write(halco.ResetChipOnDLS(), fisch.ResetChip(True))
        builder.write(halco.TimerOnDLS(), fisch.Timer())
        builder.wait_until(halco.TimerOnDLS(), 10)
        builder.write(halco.ResetChipOnDLS(), fisch.ResetChip(False))

        # wait until omnibus is up (22 us)
        builder.wait_until(halco.TimerOnDLS(), 22 * fisch.fpga_clock_cycles_per_us)

        # JTAG init
        builder.write(halco.JTAGClockScalerOnDLS(), fisch.JTAGClockScaler(3))
        builder.write(halco.ResetJTAGTapOnDLS(), fisch.ResetJTAGTap())

        # write PPU word
        ppu_top_base = 0x02000000 + (1 << 23)
        address = ppu_top_base + random.randint(0, 1023)
        data = random.randint(0, 0xffffffff)
        builder.write(address, fisch.OmnibusOnChipOverJTAG(data))
        # read it
        ticket = builder.read(address, fisch.OmnibusOnChipOverJTAG())

        # pop playback program
        builder.wait_until(halco.TimerOnDLS(), 10000)
        builder.halt()
        program = builder.done()

        # ticket data not yet available
        self.assertFalse(ticket.valid())

        # execute playback program in simulation
        executor = fisch.PlaybackProgramSimExecutor("127.0.0.1", 50008) # ip, port
        executor.transfer(program)
        executor.execute()
        executor.fetch(program)

        # ticket data available
        self.assertTrue(ticket.valid())
        response = ticket.get()
        self.assertEqual(response.get(), data)

if __name__ == "__main__":
    unittest.main()
