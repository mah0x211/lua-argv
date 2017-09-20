package = "argv"
version = "scm-1"
source = {
    url = "gitrec://github.com/mah0x211/lua-argv.git"
}
description = {
    summary = "argument vector handling module",
    homepage = "https://github.com/mah0x211/lua-argv",
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1",
    "luarocks-fetch-gitrec >= 0.2",
}
build = {
    type = "builtin",
    modules = {
        argv = {
            incdirs = { "deps/lauxhlib" },
            sources = { "src/argv.c" }
        }
    }
}

