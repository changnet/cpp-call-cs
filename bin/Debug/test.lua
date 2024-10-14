
print("start lua")

local cs = require "cs"

local e = cs.test();
print("cs test result", e)
assert(e)