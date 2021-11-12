lua-argv
===

[![test](https://github.com/mah0x211/lua-argv/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-argv/actions/workflows/test.yml)

argument vector handling module.

***


## Installation

```sh
luarocks install argv
```

---

## args = argv.new()

create instance of `argv`.

**Returns**

- `args:argv`: argv object.

**Example**

```lua
local args = require('argv').new()
```

that instance of `argv` has a '__len' metamethod.

```lua
local len = #args;
```


## ... = argv:set( n, ... )

store arguments (`...`) except `n` arguments and returns remaining arguments.
if `n` is positive number then it exclude the first `n` arguments, otherwise exclude the last `n` arguments.

**Parameters**

- `n:integer`: number of arguments.
- `...`: any arguments.

**Returns**

- `...`: excluded arguments.

**Example**

```lua
local dump = require('dump')
local argv = require('argv')
local args = argv.new()

local excluded = {
    args:set(3, 'a', 'b', 'c', 'd'),
}
print(dump(excluded))
-- {
--     [1] = "a",
--     [2] = "b",
--     [3] = "c"
-- }
print(dump({args:select()}))
-- {
--     [1] = "d"
-- }


excluded = {
    args:set(-2, 'a', 'b', 'c', 'd'),
}
print(dump(excluded))
-- {
--     [1] = "c",
--     [2] = "d",
-- }
print(dump({args:select()}))
-- {
--     [1] = "a"
--     [2] = "b"
-- }
```


## ... = argv:select( [n [, ...]] )

returns `n` stored arguments and the passed arguments.
if `n` is positive number then it returns the first `n` arguments, otherwise returns the last `n` arguments.

**Parameters**

- `n:number`: number of arguments. (default: `#argv`)
- `...`: any arguments.

**Returns**

- `...`: saved arguments.

