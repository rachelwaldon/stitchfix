#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string_view>
#include <array>

namespace SST::StitchFix::Constants {

    inline constexpr int ONE_SECOND = 1;
    inline constexpr int ONE_HOUR = 3600;
    inline constexpr int SIX_HOURS = 6*ONE_HOUR;
    inline constexpr int ONE_DAY = 86400;
    inline const std::string TICK_FREQ { "3600s" };
    inline constexpr unsigned int SEED = 4;
    inline constexpr std::array<std::string_view, 3> ENUM_STRINGS { "blazer", "pants", "dress" };
}

#endif
