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
 * @param programs List of playback-programs with one program for each connection in the
 * multiconnection.
 * @return Run time information
 */
template <typename Connection>
RunTimeInfo run(
    Connection& connection, std::vector<std::shared_ptr<PlaybackProgram>> const& programs);

} // namespace fisch::vx

#include "fisch/vx/run.tcc"
