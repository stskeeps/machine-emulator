-- Copyright Cartesi and individual authors (see AUTHORS)
-- SPDX-License-Identifier: LGPL-3.0-or-later

local cartesi = require("cartesi")

local images_dir = assert(arg[1], "usage: create-test-machine.lua <tests-images-dir> <output-dir>")
local output_dir = assert(arg[2], "usage: create-test-machine.lua <tests-images-dir> <output-dir>")
local ram_mib = tonumber(arg[3]) or 64
assert(ram_mib >= 1 and ram_mib % 1 == 0, "RAM size must be a positive integer MiB")
images_dir = images_dir:gsub("/$", "")

local config = {
    ram = {
        length = ram_mib * 1024 * 1024,
        backing_store = {
            data_filename = images_dir .. "/linux.bin",
        },
    },
    dtb = {
        entrypoint = "rollup-init echo-dapp --reject=0 --verbose",
    },
    flash_drive = {
        {
            backing_store = {
                data_filename = images_dir .. "/rootfs.ext2",
            },
        },
    },
}

local machine = cartesi.machine(config)
machine:store(output_dir, cartesi.SHARING_ALL)
machine:destroy()
print(string.format("Created %d MiB initial-state test machine at %s", ram_mib, output_dir))
