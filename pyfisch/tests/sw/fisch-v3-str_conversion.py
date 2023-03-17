import unittest

import pyfisch_vx_v3 as fisch

class StrConversion(unittest.TestCase):
    def test_str_conversion(self):
        self.assertEqual(str(fisch.JTAGIdCode()), "JTAGIdCode(0x0)")
        self.assertEqual(str(fisch.JTAGClockScaler(3)), "JTAGClockScaler(3)")
        self.assertEqual(str(fisch.ResetJTAGTap()), "ResetJTAGTap()")
        self.assertEqual(str(fisch.OmnibusChipOverJTAG(12)),
            "OmnibusChipOverJTAG(0d12 0xc 0b00000000000000000000000000001100)")
        self.assertEqual(str(fisch.Omnibus(fisch.Omnibus.Value(12, [True, True, False, True]))),
            "Omnibus(0d12 0xc 0b00000000000000000000000000001100, byte_enables: 1101)")
        self.assertEqual(str(fisch.JTAGPLLRegister(12)),
            "JTAGPLLRegister(0d12 0xc 0b00000000000000000000000000001100)")
        self.assertEqual(str(fisch.ResetChip(True)), "ResetChip(true)")
        self.assertEqual(str(fisch.Timer()), "Timer(0)")

if __name__ == "__main__":
    unittest.main()
