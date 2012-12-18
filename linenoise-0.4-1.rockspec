package = 'linenoise'
version = '0.4-1'
source = {
  url = 'https://github.com/hoelzro/lua-linenoise/archive/0.04.tar.gz',
  dir = 'lua-linenoise-0.04',
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
}
