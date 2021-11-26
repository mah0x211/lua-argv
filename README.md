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


## ok, ... = argv:set( n, ... )

store arguments (`...`) except `n` arguments and returns remaining arguments.
if `n` is positive number then it exclude the first `n` arguments, otherwise exclude the last `n` arguments.

**Parameters**

- `n:integer`: number of arguments.
- `...`: any arguments.

**Returns**

- `ok:boolean`: `false` if the stack cannot be allocated.
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
--     [1] = true,
--     [2] = "a",
--     [3] = "b",
--     [4] = "c"
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
--     [1] = true,
--     [2] = "c",
--     [3] = "d",
-- }
print(dump({args:select()}))
-- {
--     [1] = "a"
--     [2] = "b"
-- }
```


## ok = argv:add( pos, ... )

insert arguments (`...`) at position `pos`. 'pos' can be a negative number. The last argument is position `0` and `-1`.

**Parameters**

- `pos:integer`: insert position.
- `...`: any arguments.

**Returns**

- `ok:boolean`: `false` if the stack cannot be allocated.

**Example**

```lua
local dump = require('dump')
local argv = require('argv')
local v = argv.new()

v:set(0, 'a', 'b', 'c', 'd')
v:add(0, '1', '2', '3')
v:add(1, 'X', 'Y', 'Z')
v:add(3, 'foo', 'bar')
print(dump({v:select()}))
-- {
--     [1] = "X",
--     [2] = "Y",
--     [3] = "foo",
--     [4] = "bar",
--     [5] = "Z",
--     [6] = "a",
--     [7] = "b",
--     [8] = "c",
--     [9] = "d",
--     [10] = "1",
--     [11] = "2",
--     [12] = "3"
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

