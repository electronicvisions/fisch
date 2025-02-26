#pragma once
#include "fisch/vx/genpybind.h"
#include "fisch/vx/playback_program.h"
#include "hate/visibility.h"
#include <memory>

#if defined(__GENPYBIND__) || defined(__GENPYBIND_GENERATED__)
#include "pyhxcomm/vx/reinit_stack_entry.h"
#endif

namespace fisch { namespace vx GENPYBIND_TAG_FISCH_VX {

class ReinitStackEntry
{
public:
	ReinitStackEntry() = delete;
	template <typename Connection>
	ReinitStackEntry(Connection& connection);
	ReinitStackEntry(ReinitStackEntry const&) = delete;
	ReinitStackEntry(ReinitStackEntry&&) = default;
	~ReinitStackEntry() SYMBOL_VISIBLE;

	/**
	 * Set and maybe enforce reinit stack entry value.
	 * @param pbmem_request Playback program to be executed once a reinit is required.
	 * @param pbmem_snapshot Playback program to be executed once the exclusive access to the
	 * hardware is relinquished. All read commands within this program are translated to writes and
	 * replace the pbmem_request for future reinit operations. This is to be used to snapshot (parts
	 * of) the current state of the hardware prior to releasing the exclusive access to other users
	 * and be able to reapply this state at the next reinit operation without need for client-side
	 * synchronisation and transform of the read-out data. Currently only Omnibus read -> write
	 * operations are supported.
	 * @param enforce Whether to directly apply the pbmem_request or only apply it during the next
	 * reinit.
	 */
	void set(
	    std::shared_ptr<PlaybackProgram> const& pbmem_request,
	    std::optional<std::shared_ptr<PlaybackProgram>> const& pbmem_snapshot = std::nullopt,
	    bool enforce = true) SYMBOL_VISIBLE;

	void enforce() SYMBOL_VISIBLE;

	void pop() SYMBOL_VISIBLE;

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
	    "set", &ReinitStackEntry::set, pybind11::arg("pbmem_request"),
	    pybind11::arg("pbmem_snapshot") = std::nullopt, pybind11::arg("enforce") = true);
})

} // namespace vx
} // namespace fisch

#include "fisch/vx/reinit_stack_entry.tcc"
