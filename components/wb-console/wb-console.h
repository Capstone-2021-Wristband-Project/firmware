#ifndef CONSOLE___H
#define CONSOLE___H

#include <array>
#include <cstdio>

namespace Console {

template <size_t N>
void print_array(std::array<uint8_t, N> const &data) {
    for (auto byte : data) {
        printf("%02hhx ", byte);
    }
    printf("\n");
}

}  // namespace Console

#endif