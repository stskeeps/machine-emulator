-- Copyright Cartesi and individual authors (see AUTHORS)
-- SPDX-License-Identifier: LGPL-3.0-or-later

local cartesi = require("cartesi")

local images_dir = assert(arg[1], "usage: create-test-machine.lua <tests-images-dir> <output-dir>")
local output_dir = assert(arg[2], "usage: create-test-machine.lua <tests-images-dir> <output-dir>")
images_dir = images_dir:gsub("/$", "")

local config = {
    ram = {
        length = 0x4000000,
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
print("Created initial-state test machine at " .. output_dir)
