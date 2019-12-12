import os
from os.path import join
from waflib.extras.test_base import summary
from waflib.extras.symwaf2ic import get_toplevel_path


def depends(dep):
    dep('halco')
    dep('hxcomm')
    dep('code-format')
    dep.recurse('pyfisch')


def options(opt):
    opt.load('compiler_cxx')
    opt.load('gtest')
    opt.load('test_base')
    opt.load("doxygen")
    opt.recurse('pyfisch')


def configure(cfg):
    cfg.load('compiler_cxx')
    cfg.load('gtest')
    cfg.load('test_base')
    cfg.load("doxygen")
    cfg.recurse('pyfisch')


def build(bld):
    bld.env.DLSvx_HARDWARE_AVAILABLE = "cube" == os.environ.get("SLURM_JOB_PARTITION")
    bld.env.DLSvx_SIM_AVAILABLE = "FLANGE_SIMULATION_RCF_PORT" in os.environ

    bld(
        target          = 'fisch_inc',
        export_includes = 'include',
    )

    bld.shlib(
        source = bld.path.ant_glob('src/fisch/vx/*.cpp'),
        target = 'fisch_vx',
        use = ['fisch_inc', 'hxcomm', 'halco_hicann_dls_vx'],
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/sw/fisch/vx/*.cpp'),
        target = 'fisch_swtests_vx',
        use = ['fisch_vx'],
    )

    bld(
        target = 'fisch_hwtest_vx_inc',
        export_includes = 'tests/hw/fisch/vx/executor_hw/',
    )

    bld(
        target = 'fisch_simtest_vx_inc',
        export_includes = 'tests/hw/fisch/vx/executor_sim/',
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
        target = 'fisch_hwtests_vx',
        use = ['fisch_vx', 'fisch_hwtest_vx_inc'],
        test_main = 'tests/hw/fisch/vx/main.cpp',
        skip_run = not bld.env.DLSvx_HARDWARE_AVAILABLE
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
        target = 'fisch_simtests_vx',
        use = ['fisch_vx', 'fisch_simtest_vx_inc'],
        test_main = 'tests/hw/fisch/vx/main.cpp',
        skip_run = not bld.env.DLSvx_SIM_AVAILABLE,
    )

    bld.recurse('pyfisch')

    bld(
        features = 'doxygen',
        name = 'fisch_documentation',
        doxyfile = bld.root.make_node(join(get_toplevel_path(), "code-format" ,"doxyfile")),
        install_path = 'doc/fisch',
        pars = {
            "PROJECT_NAME": "\"FPGA Instruction Set Compiler for HICANN\"",
            "INPUT": join(get_toplevel_path(), "fisch", "include"),
            "PREDEFINED": "GENPYBIND()= GENPYBIND_TAG_FISCH_VX=",
            "INCLUDE_PATH": join(get_toplevel_path(), "fisch", "include"),
            "OUTPUT_DIRECTORY": join(get_toplevel_path(), "build", "fisch", "doc")
        },
    )

    # Create test summary (to stdout and XML file)
    bld.add_post_fun(summary)
