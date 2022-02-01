#pragma once
#include <memory>

namespace fisch::vx {

class PlaybackProgram;
struct RunTimeInfo;

/**
 * Transfer and execute the given playback program and fetch results.
 *
 * @tparam Connection Connection type to be used for running the program
 * @param connection Connection instance to be used for running the program
 * @param program PlaybackProgram to run
 * @return Run time information
 */
template <typename Connection>
RunTimeInfo run(Connection& connection, std::shared_ptr<PlaybackProgram> const& program);

} // namespace fisch::vx

#include "fisch/vx/run.tcc"
