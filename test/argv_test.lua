local unpack = unpack or table.unpack
local argv = require('argv')
local testcase = require('testcase')

function testcase.new()
    -- test that returns instance of argv
    local v = assert(argv.new())
    assert.match(tostring(v), '^argv: ', false)
end

function testcase.set()
    local v = assert(argv.new())

    -- test that set all arguments
    local remains = {v:set(0, 'a', 'b', 'c', 'd')}
    assert.equal(remains, {true})
    assert.equal({v:select()}, {'a', 'b', 'c', 'd'})

    -- test that set the arguments exclude the first three arguments
    remains = {v:set(3, 'a', 'b', 'c', 'd')}
    assert.equal(remains, {true, 'a', 'b', 'c'})
    assert.equal({v:select()}, {'d'})

    -- test that set the arguments exclude the last two arguments
    remains = {v:set(-2, 'a', 'b', 'c', 'd')}
    assert.equal(remains, {true, 'c', 'd'})
    assert.equal({v:select()}, {'a', 'b'})

    -- test that reset arguments with no arguments
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal(#v, 4)
    v:set()
    assert.equal(#v, 0)

    -- test that reset arguments without value arguments
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal(#v, 4)
    v:set(100)
    assert.equal(#v, 0)

    -- test that throw an error with invalid argument
    local err = assert.throws(function()
        v:set('foo')
    end)
    assert.match(err, '#1 .+ [(]number expected, got string', false)
end

function testcase.add()
    local v = assert(argv.new())

    -- test that add arguments to index
    for _, arg in ipairs({
        {add = {-5, 'x', 'y', 'z'}, eq = {'x', 'y', 'z', 'a', 'b', 'c', 'd'}},
        {add = {-4, 'x', 'y', 'z'}, eq = {'x', 'y', 'z', 'a', 'b', 'c', 'd'}},
        {add = {-3, 'x', 'y', 'z'}, eq = {'a', 'x', 'y', 'z', 'b', 'c', 'd'}},
        {add = {-2, 'x', 'y', 'z'}, eq = {'a', 'b', 'x', 'y', 'z', 'c', 'd'}},
        {add = {-1, 'x', 'y', 'z'}, eq = {'a', 'b', 'c', 'x', 'y', 'z', 'd'}},
        {add = {0, 'x', 'y', 'z'}, eq = {'a', 'b', 'c', 'd', 'x', 'y', 'z'}},
        {add = {1, 'x', 'y', 'z'}, eq = {'x', 'y', 'z', 'a', 'b', 'c', 'd'}},
        {add = {2, 'x', 'y', 'z'}, eq = {'a', 'x', 'y', 'z', 'b', 'c', 'd'}},
        {add = {3, 'x', 'y', 'z'}, eq = {'a', 'b', 'x', 'y', 'z', 'c', 'd'}},
        {add = {4, 'x', 'y', 'z'}, eq = {'a', 'b', 'c', 'x', 'y', 'z', 'd'}},
        {add = {5, 'x', 'y', 'z'}, eq = {'a', 'b', 'c', 'd', 'x', 'y', 'z'}},
        {add = {6, 'x', 'y', 'z'}, eq = {'a', 'b', 'c', 'd', 'x', 'y', 'z'}},
    }) do
        v:set(0, 'a', 'b', 'c', 'd')
        assert(v:add(unpack(arg.add)))
        assert.equal({v:select()}, arg.eq)
    end
end

function testcase.select()
    local v = assert(argv.new())

    -- test that returns all stored arguments
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal({v:select()}, {'a', 'b', 'c', 'd'})

    -- test that returns two stored arguments from the head
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal({v:select(2)}, {'a', 'b'})

    -- test that returns all stored arguments if n is greater than length
    assert.equal({v:select(20)}, {'c', 'd'})

    -- test that returns three stored arguments from the tail
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal({v:select(-3)}, {'b', 'c', 'd'})

    -- test that returns all stored arguments if n is greater than length
    assert.equal({v:select(-20)}, {'a'})

    -- test that returns no arguments
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal({v:select(0)}, {})

    -- test that returns passed arguments
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal({v:select(0, 'foo', 'bar')}, {'foo', 'bar'})

    -- test that returns one stored argument from the head and passed arguments
    v:set(0, 'a', 'b', 'c', 'd')
    assert.equal({v:select(1, 'hello', 'world')}, {'a', 'hello', 'world'})
end
