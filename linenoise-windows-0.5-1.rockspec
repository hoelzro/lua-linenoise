package = "linenoise-windows"
version = "0.5-1"
source = {
	url = "https://github.com/Choonster/lua-linenoise-windows/archive/0.05.tar.gz",
	dir = "lua-linenoise-windows-0.05",
}
description = {
	summary	= "A binding for the linenoise command line library",
	homepage = "https://github.com/Choonster/lua-linenoise-windows",
	license	= "MIT/X11",
}
dependencies = {
	"lua >= 5.1"
}

build = {
	platform = {
		win32 = {
			type = "builtin",
			modules = {
				linenoise = {
					sources	 = { "lua-linenoise.c", "linenoise-windows/linenoise.c", "linenoise-windows/win32fixes.c" },
				},
			},
			libraries = {
				"ws2_32.lib"
			},
		},
		unix = {
			type = "builtin",
			modules = {
				linenoise = {
					sources = { "lua-linenoise.c", "linenoiselib.c" },
				}
			}
		}
	}
}
