#pragma once

#include "fisch/vx/genpybind.h"

GENPYBIND_TAG_FISCH_VX
#if (FISCH_VX_CHIP_VERSION == 2)
GENPYBIND_MANUAL({
	parent.attr("__variant__") = "pybind11";
	parent->py::module::import("pyhalco_hicann_dls_vx_v2");
});
#elif (FISCH_VX_CHIP_VERSION == 3)
GENPYBIND_MANUAL({
	parent.attr("__variant__") = "pybind11";
	parent->py::module::import("pyhalco_hicann_dls_vx_v3");
});
#elif (FISCH_VX_CHIP_VERSION == 4)
GENPYBIND_MANUAL({
	parent.attr("__variant__") = "pybind11";
	parent->py::module::import("pyhalco_hicann_dls_vx_v4");
});
#else
#error "Unknown FISCH_VX_CHIP_VERSION value."
#endif

#include "fisch/vx/fisch.h"

#include "halco/hicann-dls/vx/coordinates.h"
