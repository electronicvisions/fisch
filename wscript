import os
from os.path import join
from waflib.extras.test_base import summary
from waflib.extras.symwaf2ic import get_toplevel_path


def depends(dep):
    dep('hate')
    dep('halco')
    dep('hxcomm')
    dep('code-format')
    dep('logger')

    if getattr(dep.options, 'with_fisch_python_bindings', True):
        dep.recurse('pyfisch')

    dep('libnux')


def options(opt):
    opt.load('compiler_cxx')
    opt.load('gtest')
    opt.load('test_base')
    opt.load("doxygen")
    opt.recurse('pyfisch')

    hopts = opt.add_option_group('fisch options')
    hopts.add_option("--fisch-loglevel",
                     choices=["trace", "debug", "info",
                              "warning", "error", "fatal"],
                     default="warning",
                     help="Maximal loglevel to compile in.")
    hopts.add_withoption('fisch-python-bindings', default=True,
                         help='Toggle the generation and build of fisch python bindings')

def configure(cfg):
    cfg.load('compiler_cxx')
    cfg.load('gtest')
    cfg.load('test_base')
    cfg.load("doxygen")
    cfg.check_boost(lib='program_options', uselib_store='BOOST4FISCHTOOLS')

    if getattr(cfg.options, 'with_fisch_python_bindings', True):
        cfg.recurse('pyfisch')

    cfg.define(
        "FISCH_LOG_THRESHOLD",
        {'trace':   0,
         'debug':   1,
         'info':    2,
         'warning': 3,
         'error':   4,
         'fatal':   5}[cfg.options.fisch_loglevel]
    )


def build(bld):
    bld.env.DLSvx_HARDWARE_AVAILABLE = "cube" == os.environ.get("SLURM_JOB_PARTITION")
    bld.env.DLSvx_SIM_AVAILABLE = "FLANGE_SIMULATION_RCF_PORT" in os.environ

    bld(
        target          = 'fisch_inc',
        export_includes = 'include',
    )

    if bld.env.have_ppu_toolchain:
        bld.stlib(
            source = [
                'src/fisch/vx/word_access/type/omnibus.cpp',
                'src/fisch/vx/word_access/type/jtag.cpp',
            ],
            target = 'fisch_ppu_vx',
            use = ['fisch_inc', 'hate_inc', 'halco_hicann_dls_ppu_vx'],
            env = bld.all_envs['nux_vx'],
            linkflags = '-Wl,-z,defs',
            install_path = '${PREFIX}/lib/ppu',
        )

    bld.shlib(
        source = bld.path.ant_glob('src/fisch/vx/**/*.cpp'),
        target = 'fisch_vx',
        use = ['fisch_inc', 'hxcomm', 'halco_hicann_dls_vx', 'logger_obj'],
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/sw/fisch/vx/*.cpp'),
        target = 'fisch_swtest_vx',
        use = ['fisch_vx'],
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
        target = 'fisch_hwsimtest_vx',
        use = ['fisch_vx', 'BOOST4FISCHTOOLS'],
        test_main = 'tests/hw/fisch/vx/main.cpp',
        skip_run = not (bld.env.DLSvx_HARDWARE_AVAILABLE or bld.env.DLSvx_SIM_AVAILABLE),
        test_timeout = 3600 if bld.env.DLSvx_SIM_AVAILABLE else 30
    )

    # like fisch_hwsimtest_vx but not for sim backend (because of 2x test time
    # otherwise); the defines enforces a local quiggeldy if no remote quiggeldy
    # is available => tests quiggeldy wrapping locally
    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
        target = 'fisch_qghwsimtest_vx',
        use = ['fisch_vx', 'BOOST4FISCHTOOLS'],
        defines = ['FISCH_TEST_LOCAL_QUIGGELDY'],
        test_main = 'tests/hw/fisch/vx/main.cpp',
        skip_run = True,  # Unstable tests, cf. issue #3976
        depends_on = ["quiggeldy"],
        test_timeout = 30
    )

    if getattr(bld.options, 'with_fisch_python_bindings', True):
        bld.recurse('pyfisch')

    if bld.env.DOXYGEN:
        bld(
            target = 'doxygen_fisch',
            features = 'doxygen',
            doxyfile = bld.root.make_node(join(get_toplevel_path(), "code-format" ,"doxyfile")),
            doxy_inputs = 'include/fisch',
            install_path = 'doc/fisch',
            pars = {
                "PROJECT_NAME": "\"FPGA Instruction Set Compiler for HICANN\"",
                "PREDEFINED": "GENPYBIND()= GENPYBIND_TAG_FISCH_VX=",
                "INCLUDE_PATH": join(get_toplevel_path(), "fisch", "include"),
                "OUTPUT_DIRECTORY": join(get_toplevel_path(), "build", "fisch", "doc")
            },
        )

    # Create test summary (to stdout and XML file)
    bld.add_post_fun(summary)
