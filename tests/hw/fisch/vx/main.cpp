#include <string>

#include <boost/program_options.hpp>
#include <gtest/gtest.h>
#ifdef FISCH_TEST_LOCAL_QUIGGELDY
#include "hxcomm/vx/ensure_local_quiggeldy.h"
#include <chrono>
#include <thread>
#endif

// logger include directory structure omits prefix
#include "logging_ctrl.h"

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	std::string loglevel;
	namespace bpo = boost::program_options;
	bpo::options_description desc("Options");
	// clang-format off
    desc.add_options()("loglevel", bpo::value<std::string>(&loglevel)->default_value("trace"));
	// clang-format on

	bpo::variables_map vm;
	bpo::store(
	    bpo::basic_command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
	bpo::notify(vm);

	if (loglevel == "trace") {
		logger_default_config(log4cxx::Level::getTrace());
	} else if (loglevel == "debug") {
		logger_default_config(log4cxx::Level::getDebug());
	} else if (loglevel == "info") {
		logger_default_config(log4cxx::Level::getInfo());
	} else if (loglevel == "warning") {
		logger_default_config(log4cxx::Level::getWarn());
	} else if (loglevel == "error") {
		logger_default_config(log4cxx::Level::getError());
	} else if (loglevel == "fatal") {
		logger_default_config(log4cxx::Level::getFatal());
	} else {
		std::cout << "loglevel option has to be one of {trace,debug,info,warning,error,fatal"
		          << std::endl;
		exit(EXIT_FAILURE);
	}

#ifdef FISCH_TEST_LOCAL_QUIGGELDY
	// ensure that a local quiggeldy is running; this is a noop if we run
	// already in quiggeldy environment
	[[maybe_unused]] hxcomm::vx::EnsureLocalQuiggeldy context;
	using namespace std::literals::chrono_literals;
	std::this_thread::sleep_for(1s);
#endif

	return RUN_ALL_TESTS();
}
