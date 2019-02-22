from waflib.extras.test_base import summary

def depends(dep):
    dep('halco')


def options(opt):
    opt.load('test_base')


def configure(cfg):
    cfg.load('test_base')


def build(bld):
    bld(target          = 'fisch_inc',
        export_includes = 'include'
    )

    # Create test summary (to stdout and XML file)
    bld.add_post_fun(summary)


def doc(dox):
    pass
