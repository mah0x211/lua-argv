lua-argv
===

argument vector handling module.

***


## Installation

```sh
luarocks install argv --from=http://mah0x211.github.io/rocks/
```

---


## Create an Argv Object.

### args = argv.new()

returns a new `Argv` object.

**Returns**

- `args:argv`: argv object.

**Example**

```lua
local args = require('argv').new();
```


---

## Argv Properties


### __len

```
local len = #args;
```


## Argv Methods


### ... = argv:set( n, ... )

remove all saved arguments and save passed variable arguments (`...`) then return the `n` saved arguments from a head.

**Parameters**

- `n:number`: number of arguments.
- `...`: any arguments.

**Returns**

- `...`: saved arguments.


### ... = argv:select( [n] )

return the `n` saved arguments from a head.

**Parameters**

- `n:number`: number of arguments. (default: `#argv`)

**Returns**

- `...`: saved arguments.

