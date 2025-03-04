// Copyright 2020 Cartesi Pte. Ltd.
//
// This file is part of the machine-emulator. The machine-emulator is free
// software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// The machine-emulator is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the machine-emulator. If not, see http://www.gnu.org/licenses/.
//

#include "clua-htif.h"
#include "clua.h"
#include "htif.h"

namespace cartesi {

int clua_htif_export(lua_State *L, int ctxidx) {
    (void) ctxidx;
    struct named_constant {
        uint64_t value;
        const char *name;
    };
    std::array constants{
        named_constant{HTIF_DEVICE_HALT, "HTIF_DEVICE_HALT"},
        named_constant{HTIF_DEVICE_CONSOLE, "HTIF_DEVICE_CONSOLE"},
        named_constant{HTIF_DEVICE_YIELD, "HTIF_DEVICE_YIELD"},
        named_constant{HTIF_HALT_HALT, "HTIF_HALT_HALT"},
        named_constant{HTIF_YIELD_AUTOMATIC, "HTIF_YIELD_AUTOMATIC"},
        named_constant{HTIF_YIELD_MANUAL, "HTIF_YIELD_MANUAL"},
        named_constant{HTIF_YIELD_REASON_PROGRESS, "HTIF_YIELD_REASON_PROGRESS"},
        named_constant{HTIF_YIELD_REASON_RX_ACCEPTED, "HTIF_YIELD_REASON_RX_ACCEPTED"},
        named_constant{HTIF_YIELD_REASON_RX_REJECTED, "HTIF_YIELD_REASON_RX_REJECTED"},
        named_constant{HTIF_YIELD_REASON_TX_VOUCHER, "HTIF_YIELD_REASON_TX_VOUCHER"},
        named_constant{HTIF_YIELD_REASON_TX_NOTICE, "HTIF_YIELD_REASON_TX_NOTICE"},
        named_constant{HTIF_YIELD_REASON_TX_REPORT, "HTIF_YIELD_REASON_TX_REPORT"},
        named_constant{HTIF_YIELD_REASON_TX_EXCEPTION, "HTIF_YIELD_REASON_TX_EXCEPTION"},
        named_constant{HTIF_CONSOLE_GETCHAR, "HTIF_CONSOLE_GETCHAR"},
        named_constant{HTIF_CONSOLE_PUTCHAR, "HTIF_CONSOLE_PUTCHAR"},
    };
    for (const auto &c : constants) {
        lua_pushinteger(L, static_cast<lua_Integer>(c.value));
        lua_setfield(L, -2, c.name);
    }
    return 0;
}

} // namespace cartesi
