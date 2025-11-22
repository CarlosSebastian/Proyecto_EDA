#ifndef BOUNDSID_H
#define BOUNDSID_H

#include <string>

enum class BoundsID {
    None,
    Keogh,
    Improved,
    Enhanced,
    Petitjean,
    Webb,
    EnhancedWebb
};

inline std::string boundsIDToString(BoundsID id) {
    switch(id) {
        case BoundsID::None: return "None";
        case BoundsID::Keogh: return "Keogh";
        case BoundsID::Improved: return "Improved";
        case BoundsID::Enhanced: return "Enhanced";
        case BoundsID::Petitjean: return "Petitjean";
        case BoundsID::Webb: return "Webb";
        case BoundsID::EnhancedWebb: return "EnhancedWebb";
        default: return "Unknown";
    }
}

inline BoundsID stringToBoundsID(const std::string& str) {
    if (str == "none" || str == "None") return BoundsID::None;
    if (str == "keogh" || str == "Keogh") return BoundsID::Keogh;
    if (str == "improved" || str == "Improved") return BoundsID::Improved;
    if (str == "enhanced" || str == "Enhanced") return BoundsID::Enhanced;
    if (str == "petitjean" || str == "Petitjean") return BoundsID::Petitjean;
    if (str == "webb" || str == "Webb") return BoundsID::Webb;
    if (str == "enhancedwebb" || str == "EnhancedWebb") return BoundsID::EnhancedWebb;
    return BoundsID::None;
}

#endif // BOUNDSID_H