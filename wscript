from waflib.extras.test_base import summary

def depends(dep):
    dep('halco')
    dep('hxcomm')
    dep.recurse('pyfisch')


def options(opt):
    opt.load('compiler_cxx')
    opt.load('gtest')
    opt.load('test_base')
    opt.recurse('pyfisch')


def configure(cfg):
    cfg.load('compiler_cxx')
    cfg.load('gtest')
    cfg.load('test_base')
    cfg.recurse('pyfisch')


def build(bld):
    bld(
        target          = 'fisch_inc',
        export_includes = 'include',
    )

    bld.shlib(
        source = bld.path.ant_glob('src/fisch/vx/*.cpp'),
        target = 'fisch_vx',
        use = ['fisch_inc', 'hx_comm_inc', 'hx_comm', 'halco_hicann_dls_vx'],
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
        skip_run = True,
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/hw/fisch/vx/test-*.cpp'),
        target = 'fisch_simtests_vx',
        use = ['fisch_vx', 'fisch_simtest_vx_inc'],
        test_main = 'tests/hw/fisch/vx/main.cpp',
        skip_run = True,
    )

    bld.recurse('pyfisch')

    # Create test summary (to stdout and XML file)
    bld.add_post_fun(summary)


def doc(dox):
    pass
