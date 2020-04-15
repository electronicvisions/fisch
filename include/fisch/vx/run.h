#pragma once

#include <memory>

namespace fisch::vx {

class PlaybackProgram;

/**
 * Transfer and execute the given playback program and fetch results.
 *
 * @tparam Connection Connection type to be used for running the program
 * @param connection Connection instance to be used for running the program
 * @param program PlaybackProgram to run
 */
template <typename Connection>
void run(Connection& connection, std::shared_ptr<PlaybackProgram> const& program);

} // namespace fisch::vx

#include "fisch/vx/run.tcc"
