package = 'linenoise'
version = '0.8-1'
source = {
  url = 'https://github.com/hoelzro/lua-linenoise/archive/0.8.tar.gz',
  dir = 'lua-linenoise-0.8',
}
description = {
  summary  = 'A binding for the linenoise command line library',
  homepage = 'https://github.com/hoelzro/lua-linenoise',
  license  = 'MIT/X11',
}
dependencies = {
  'lua >= 5.1'
}

build = {
  type    = 'builtin',
  modules = {
    linenoise = {
      sources   = { 'linenoise.c', 'linenoiselib.c' },
    },
  },
  platforms = {
    win32 = {
      modules = {
        linenoise = {
          sources   = { 'linenoise.c', 'linenoiselib.c', 'win32fixes.c' },
          libraries = { 'ws2_32' },
        },
      },
    },
  },
}
