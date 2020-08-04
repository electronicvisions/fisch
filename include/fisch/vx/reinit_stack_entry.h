#pragma once
#include "fisch/vx/genpybind.h"
#include "fisch/vx/playback_program.h"
#include <memory>

#if defined(__GENPYBIND__) || defined(__GENPYBIND_GENERATED__)
#include "pyhxcomm/vx/reinit_stack_entry.h"
#endif

namespace fisch::vx GENPYBIND_TAG_FISCH_VX {

class ReinitStackEntry
{
public:
	ReinitStackEntry() = delete;
	template <typename Connection>
	ReinitStackEntry(Connection& connection);
	ReinitStackEntry(ReinitStackEntry const&) = delete;
	ReinitStackEntry(ReinitStackEntry&&) = default;
	~ReinitStackEntry();

	void set(std::shared_ptr<PlaybackProgram> const& pbmem, bool enforce = true);

	void enforce();

	void pop();

private:
	// cannot forward declare using statement from hxcomm
	class Impl;

	std::unique_ptr<Impl> m_impl;
};

GENPYBIND_MANUAL({
	using namespace ::fisch::vx;

	pybind11::class_<ReinitStackEntry> wrapped(parent, "ReinitStackEntry");

	[[maybe_unused]] ::pyhxcomm::vx::reinit_stack_entry_unroll_helper<decltype(wrapped)> helper{
	    wrapped};

	wrapped.def("pop", &ReinitStackEntry::pop);
	wrapped.def(
	    "set", &ReinitStackEntry::set, pybind11::arg("pbmem"), pybind11::arg("enforce") = true);
})

} // namespace fisch::vx

#include "fisch/vx/reinit_stack_entry.tcc"
