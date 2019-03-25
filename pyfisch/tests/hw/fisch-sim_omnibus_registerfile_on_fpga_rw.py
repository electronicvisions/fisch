import unittest
import random

import pyfisch_vx as fisch
import pyhalco_hicann_dls_vx as halco

random.seed(1234)

class RwOmnibus(unittest.TestCase):
    def test_write_read_write_read(self):
        builder = fisch.PlaybackProgramBuilder()

        # reset chip
        builder.write(halco.ResetChipOnDLS(), fisch.ResetChip(True))
        builder.write(halco.TimerOnDLS(), fisch.Timer())
        builder.wait_until(halco.TimerOnDLS(), 10)
        builder.write(halco.ResetChipOnDLS(), fisch.ResetChip(False))

        # write registerfile word
        address = 0x0
        data_1 = (random.randint(0, 0xff) | 0x1) # lowest bit disables write responses
        builder.write(address, fisch.Omnibus(data_1))
        # read registerfile word
        ticket_1 = builder.read(address, fisch.Omnibus())
        # write it again
        data_2 = (random.randint(0, 0xff) | 0x1) # lowest bit disables write responses
        builder.write(address, fisch.Omnibus(data_2))
        # read it again
        ticket_2 = builder.read(address, fisch.Omnibus())

        # pop playback program
        builder.wait_until(halco.TimerOnDLS(), 10000)
        builder.halt()
        program = builder.done()

        # execute playback program in simulation
        executor = fisch.PlaybackProgramSimExecutor("127.0.0.1", 50004) # ip, port
        executor.transfer(program)
        executor.execute()
        executor.fetch(program)

        self.assertEqual(ticket_1.get().get(), data_1)
        self.assertEqual(ticket_2.get().get(), data_2)

if __name__ == "__main__":
    unittest.main()
