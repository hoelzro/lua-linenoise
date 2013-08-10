package = "linenoise-windows"
version = "0.5.3-1"
source = {
	url = "https://github.com/Choonster/lua-linenoise-windows/archive/0.05.3-1.tar.gz",
	dir = "lua-linenoise-windows-0.05.3-1",
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
	platforms = {
		win32 = {
			type = "builtin",
			modules = {
				linenoise = {
					sources	 = { "linenoise.c", "linenoiselib.c", "win32fixes.c" },
					libraries = { "ws2_32" },
				},
			},
		},
		unix = {
			type = "builtin",
			modules = {
				linenoise = {
					sources = { "linenoise.c", "linenoiselib.c" },
				}
			}
		}
	}
}
