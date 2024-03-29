# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

VERSION='0.0.1'
APPNAME='ndn-example'

def options(opt):
    opt.load('compiler_cxx default-compiler-flags')

def configure(conf):
    conf.load("compiler_cxx default-compiler-flags")

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', global_define=True, mandatory=True)

def build (bld):
    bld(target='server',
        features=['cxx', 'cxxprogram'],
        source='server.cpp',
        use='NDN_CXX',
	cxxflags=['-std=c++14'])


    bld(target='client',
        features=['cxx', 'cxxprogram'],
        source='client.cpp',
        use='NDN_CXX',
	cxxflags=['-std=c++14'])
