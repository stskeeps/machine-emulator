-- Copyright Cartesi and individual authors (see AUTHORS)
-- SPDX-License-Identifier: LGPL-3.0-or-later

local cartesi = require("cartesi")

local source_dir = assert(arg[1], "usage: trace-sret-wfi.lua <stored-machine-dir> [max-mcycle]")
local max_mcycle = math.maxinteger
if arg[2] then
    max_mcycle = assert(tonumber(arg[2]), "max-mcycle must be an integer")
    assert(max_mcycle >= 0 and max_mcycle % 1 == 0, "max-mcycle must be a non-negative integer")
end

local machine <close> = cartesi.machine(source_dir, {
    console = { output_destination = "to_stdout" },
    break_on_sret = true,
    break_on_wfi = true,
})

local function trace_instruction(name)
    io.stderr:write(string.format("TRACE %s mcycle=%d pc=0x%016x privilege=%d\n", name,
        machine:read_reg("mcycle"), machine:read_reg("pc"), machine:read_reg("iprv")))
end

local break_reason
while true do
    break_reason = machine:run(max_mcycle)
    if break_reason == cartesi.BREAK_REASON_SRET then
        trace_instruction("SRET")
    elseif break_reason == cartesi.BREAK_REASON_WFI then
        trace_instruction("WFI")
    elseif break_reason == cartesi.BREAK_REASON_FAILED then
        error("machine execution failed")
    elseif break_reason == cartesi.BREAK_REASON_HALTED or break_reason == cartesi.BREAK_REASON_YIELDED_MANUALLY
        or break_reason == cartesi.BREAK_REASON_MCYCLE_OVERFLOW
        or break_reason == cartesi.BREAK_REASON_REACHED_TARGET_MCYCLE then
        break
    end
end

io.stderr:write(string.format("Stopped: reason=%d mcycle=%d pc=0x%016x\n", break_reason,
    machine:read_reg("mcycle"), machine:read_reg("pc")))
