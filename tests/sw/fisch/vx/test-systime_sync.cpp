#include "gtest/gtest.h"

#include "fisch/cerealization.h"
#include "fisch/vx/systime.h"

#include "test-macros.h"

using namespace fisch::vx;

FISCH_TEST_BOOLEAN_REGISTER_GENERAL(SystimeSync)

FISCH_TEST_BOOLEAN_REGISTER_OSTREAM(SystimeSync, SystimeSync)

FISCH_TEST_BOOLEAN_REGISTER_CEREAL(SystimeSync)
