from waflib.extras.test_base import summary

def depends(dep):
    dep('halco')
    dep('hxcomm')


def options(opt):
    opt.load('compiler_cxx')
    opt.load('gtest')
    opt.load('test_base')


def configure(cfg):
    cfg.load('compiler_cxx')
    cfg.load('gtest')
    cfg.load('test_base')


def build(bld):
    bld(
        target          = 'fisch_inc',
        export_includes = 'include',
    )

    bld.shlib(
        source = bld.path.ant_glob('src/fisch/vx/*.cpp'),
        target = 'fisch_vx',
        use = ['fisch_inc', 'hx_comm_inc', 'hx_comm'],
    )

    bld(
        features = 'cxx cxxprogram gtest',
        source = bld.path.ant_glob('tests/sw/fisch/vx/*.cpp'),
        target = 'fisch_swtests_vx',
        use = ['fisch_vx'],
    )

    # Create test summary (to stdout and XML file)
    bld.add_post_fun(summary)


def doc(dox):
    pass
