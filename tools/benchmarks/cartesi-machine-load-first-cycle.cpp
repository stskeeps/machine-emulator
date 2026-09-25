// Copyright Cartesi and individual authors (see AUTHORS)
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cm.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

using clock_type = std::chrono::steady_clock;

struct sample {
    double load_ms;
    double run_ms;
    double first_cycle_ms;
    double destroy_ms;
};

double milliseconds(clock_type::duration duration) {
    return std::chrono::duration<double, std::milli>(duration).count();
}

bool parse_count(const char *text, uint64_t &value, bool allow_zero = false) {
    char *end = nullptr;
    const auto parsed = std::strtoull(text, &end, 10);
    if (end == text || *end != '\0' || (!allow_zero && parsed == 0)) {
        return false;
    }
    value = parsed;
    return true;
}

bool parse_sharing(const std::string &text, cm_sharing_mode &sharing) {
    if (text == "none") {
        sharing = CM_SHARING_NONE;
    } else if (text == "config") {
        sharing = CM_SHARING_CONFIG;
    } else if (text == "all") {
        sharing = CM_SHARING_ALL;
    } else {
        return false;
    }
    return true;
}

int fail(const char *operation, cm_error error) {
    std::cerr << operation << " failed (" << error << "): " << cm_get_last_error_message() << '\n';
    return 1;
}

int run_one(const std::string &directory, cm_sharing_mode sharing, sample &result) {
    const auto begin = clock_type::now();
    cm_machine *machine = nullptr;
    auto error = cm_load_new(directory.c_str(), nullptr, sharing, &machine);
    const auto loaded = clock_type::now();
    if (error != CM_ERROR_OK) {
        return fail("cm_load_new", error);
    }

    uint64_t initial_mcycle = 0;
    error = cm_read_reg(machine, CM_REG_MCYCLE, &initial_mcycle);
    if (error != CM_ERROR_OK) {
        cm_destroy(machine);
        cm_delete(machine);
        return fail("cm_read_reg", error);
    }
    if (initial_mcycle == UINT64_MAX) {
        cm_destroy(machine);
        cm_delete(machine);
        std::cerr << "cannot advance first cycle: mcycle is already UINT64_MAX\n";
        return 1;
    }

    cm_break_reason reason = CM_BREAK_REASON_FAILED;
    error = cm_run(machine, initial_mcycle + 1, &reason);
    const auto cycled = clock_type::now();
    uint64_t final_mcycle = initial_mcycle;
    if (error == CM_ERROR_OK) {
        error = cm_read_reg(machine, CM_REG_MCYCLE, &final_mcycle);
    }

    const auto destroy_error = cm_destroy(machine);
    cm_delete(machine);
    const auto destroyed = clock_type::now();

    if (error != CM_ERROR_OK) {
        return fail("cm_run/cm_read_reg", error);
    }
    if (destroy_error != CM_ERROR_OK) {
        return fail("cm_destroy", destroy_error);
    }
    if (final_mcycle < initial_mcycle + 1) {
        std::cerr << "machine stopped before advancing mcycle (reason " << reason << ")\n";
        return 1;
    }

    result.load_ms = milliseconds(loaded - begin);
    result.run_ms = milliseconds(cycled - loaded);
    result.first_cycle_ms = milliseconds(cycled - begin);
    result.destroy_ms = milliseconds(destroyed - cycled);
    return 0;
}

double percentile(std::vector<double> values, double p) {
    std::sort(values.begin(), values.end());
    const auto index = static_cast<size_t>((p * static_cast<double>(values.size() - 1)) + 0.5);
    return values[index];
}

void summarize(const char *name, const std::vector<sample> &samples, double sample::*field) {
    std::vector<double> values;
    values.reserve(samples.size());
    for (const auto &entry : samples) {
        values.push_back(entry.*field);
    }
    double total = 0;
    for (const auto value : values) {
        total += value;
    }
    std::cout << name << "_ms mean=" << total / static_cast<double>(values.size())
              << " median=" << percentile(values, 0.50) << " p95=" << percentile(values, 0.95)
              << " min=" << *std::min_element(values.begin(), values.end())
              << " max=" << *std::max_element(values.begin(), values.end()) << '\n';
}

} // namespace

int main(int argc, char **argv) {
    if (argc < 2 || argc > 5) {
        std::cerr << "usage: " << argv[0]
                  << " <stored-machine-dir> [iterations=10] [warmups=0] [sharing=none|config|all]\n";
        return 2;
    }

    uint64_t iterations = 10;
    uint64_t warmups = 0;
    cm_sharing_mode sharing = CM_SHARING_NONE;
    if ((argc >= 3 && !parse_count(argv[2], iterations)) || (argc >= 4 && !parse_count(argv[3], warmups, true)) ||
        (argc >= 5 && !parse_sharing(argv[4], sharing))) {
        std::cerr
            << "iterations must be positive, warmups must be non-negative, and sharing must be none, config, or all\n";
        return 2;
    }

    if (iterations > static_cast<uint64_t>(SIZE_MAX) || warmups > static_cast<uint64_t>(SIZE_MAX) ||
        iterations + warmups < iterations) {
        std::cerr << "iteration count is too large\n";
        return 2;
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "iteration,load_ms,run_to_first_cycle_ms,time_to_first_cycle_ms,destroy_ms\n";
    std::vector<sample> measured;
    measured.reserve(static_cast<size_t>(iterations));
    for (uint64_t i = 0; i < iterations + warmups; ++i) {
        sample current{};
        if (const int status = run_one(argv[1], sharing, current); status != 0) {
            return status;
        }
        if (i >= warmups) {
            const auto iteration = i - warmups;
            std::cout << iteration << ',' << current.load_ms << ',' << current.run_ms << ',' << current.first_cycle_ms
                      << ',' << current.destroy_ms << '\n';
            measured.push_back(current);
        }
    }

    summarize("load", measured, &sample::load_ms);
    summarize("run_to_first_cycle", measured, &sample::run_ms);
    summarize("time_to_first_cycle", measured, &sample::first_cycle_ms);
    summarize("destroy", measured, &sample::destroy_ms);
    return 0;
}
