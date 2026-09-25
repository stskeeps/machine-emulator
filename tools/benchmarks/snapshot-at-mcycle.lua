-- Copyright Cartesi and individual authors (see AUTHORS)
-- SPDX-License-Identifier: LGPL-3.0-or-later

local cartesi = require("cartesi")

local source_dir = assert(arg[1], "usage: snapshot-at-mcycle.lua <source-dir> <output-dir> <target-mcycle>")
local output_dir = assert(arg[2], "usage: snapshot-at-mcycle.lua <source-dir> <output-dir> <target-mcycle>")
local target_mcycle = assert(tonumber(arg[3]), "target mcycle must be an integer")
assert(target_mcycle >= 0 and target_mcycle % 1 == 0, "target mcycle must be a non-negative integer")

local machine = cartesi.machine(source_dir)
local initial_mcycle = machine:read_reg("mcycle")
assert(target_mcycle >= initial_mcycle, "target mcycle is before the stored machine's current mcycle")
print(string.format("Advancing machine from mcycle %d to %d", initial_mcycle, target_mcycle))

local break_reason = machine:run(target_mcycle)
local actual_mcycle = machine:read_reg("mcycle")
assert(actual_mcycle >= target_mcycle,
    string.format("machine stopped at mcycle %d before target %d (break reason %d)",
        actual_mcycle, target_mcycle, break_reason))

machine:store(output_dir, cartesi.SHARING_ALL)
machine:destroy()
print(string.format("Stored snapshot at mcycle %d: %s", actual_mcycle, output_dir))
