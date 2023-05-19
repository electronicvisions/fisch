from enum import Enum, auto
import os
from os.path import join
from waflib.extras.test_base import summary
from waflib.extras.symwaf2ic import get_toplevel_path
import yaml
import re
from waflib.Errors import BuildError


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
    cfg.env.CXXFLAGS_FISCH = [
        '-fvisibility=hidden',
        '-fvisibility-inlines-hidden',
    ]
    cfg.env.LINKFLAGS_FISCH = [
        '-fvisibility=hidden',
        '-fvisibility-inlines-hidden',
    ]


def build(bld):
    class TestTarget(Enum):
        SOFTWARE_ONLY = auto()
        HARDWARE = auto()
        SIMULATION = auto()

    if "FLANGE_SIMULATION_RCF_PORT" in os.environ:
        bld.env.TEST_TARGET = TestTarget.SIMULATION

        try:
            chip_revision = int(os.environ.get("SIMULATED_CHIP_REVISION"))
        except TypeError:
            raise BuildError("Environment variable 'SIMULATED_CHIP_REVISION' "
                             "not set or cannot be casted to integer.")
        bld.env.CURRENT_SETUP = dict(chip_revision=chip_revision)
    elif "SLURM_HWDB_YAML" in os.environ:
        bld.env.TEST_TARGET = TestTarget.HARDWARE
        slurm_licenses = os.environ.get("SLURM_HARDWARE_LICENSES")
        hwdb_entries = os.environ.get("SLURM_HWDB_YAML")
        fpga_id = int(re.match(r"W(?P<wafer>\d+)F(?P<fpga>\d+)",
                               slurm_licenses)["fpga"])
        fpgas = yaml.full_load(hwdb_entries)["fpgas"]
        fpga = None
        for entry in fpgas:
            if entry["fpga"] == fpga_id:
                fpga = entry
                break
        if not fpga:
            raise BuildError("FPGA not found in hwdb")
        bld.env.CURRENT_SETUP = fpga
    else:
        bld.env.TEST_TARGET = TestTarget.SOFTWARE_ONLY
        bld.env.CURRENT_SETUP = dict(chip_revision=None)

    bld.install_files(
        dest = '${PREFIX}/',
        files = bld.path.ant_glob('include/fisch/**/*.(h|tcc|def)'),
        name = 'fisch_header',
        relative_trick = True
    )

    bld(
        target          = 'fisch_inc',
        export_includes = 'include',
        depends_on = 'fisch_header'
    )

    if bld.env.have_ppu_toolchain:
        env = bld.all_envs['nux_vx'].derive()
        env.detach()

        bld.stlib(
            source = [
                'src/fisch/vx/word_access/type/omnibus.cpp',
                'src/fisch/vx/word_access/type/jtag.cpp',
            ],
            target = 'fisch_ppu_vx',
            use = ['fisch_inc', 'hate_inc', 'halco_hicann_dls_ppu_vx'],
            env = env,
            linkflags = '-Wl,-z,defs',
            uselib='FISCH',
            install_path = '${PREFIX}/lib/ppu',
        )

    for hx_version in [3]:
        bld.shlib(
            source = bld.path.ant_glob('src/fisch/vx/**/*.cpp'),
            target = f'fisch_vx_v{hx_version}',
            use = ['fisch_inc', 'hxcomm', 'halco_hicann_dls_vx', 'logger_obj'],
            defines = [f'FISCH_VX_CHIP_VERSION={hx_version}'],
            uselib='FISCH',
        )

        bld(
            features = 'cxx cxxprogram gtest',
            source = bld.path.ant_glob('tests/sw/fisch/vx/test-*.cpp'),
            target = f'fisch_swtest_vx_v{hx_version}',
            use = [f'fisch_vx_v{hx_version}', 'BOOST4FISCHTOOLS'],
            test_main = 'tests/sw/fisch/vx/main.cpp',
            defines = [f'FISCH_VX_CHIP_VERSION={hx_version}'],
            uselib='FISCH',
        )

        bld(
            features = 'cxx cxxprogram gtest',
            source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
            target = f'fisch_hwsimtest_vx_v{hx_version}',
            use = [f'fisch_vx_v{hx_version}', 'BOOST4FISCHTOOLS'],
            test_main = 'tests/hw/fisch/vx/main.cpp',
            skip_run = (bld.env.TEST_TARGET == TestTarget.SOFTWARE_ONLY or
                        not (int(bld.env.CURRENT_SETUP["chip_revision"]) == hx_version)),
            test_timeout = 3600 if bld.env.TEST_TARGET == TestTarget.SIMULATION else 30,
            uselib='FISCH',
        )

        # like fisch_hwsimtest_vx but not for sim backend (because of 2x test time
        # otherwise); the defines enforces a local quiggeldy if no remote quiggeldy
        # is available => tests quiggeldy wrapping locally
        bld(
            features = 'cxx cxxprogram gtest',
            source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
            target = f'fisch_qghwsimtest_vx_v{hx_version}',
            use = [f'fisch_vx_v{hx_version}', 'BOOST4FISCHTOOLS'],
            defines = ['FISCH_TEST_LOCAL_QUIGGELDY'],
            test_main = 'tests/hw/fisch/vx/main.cpp',
            skip_run = True,  # Unstable tests, cf. issue #3976
            depends_on = ["quiggeldy"],
            test_timeout = 30,
            uselib='FISCH',
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
