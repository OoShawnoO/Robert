/**
  ******************************************************************************
  * @file           : IPFilter.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/24
  ******************************************************************************
  */

#include "IPFilter.h"

namespace hzd {
    IPFilter::IPFilter(bool _isAllowedAll):isAllowedAll(_isAllowedAll) {}

    bool IPFilter::AddBan(in_addr_t inAddr) {
        return bannedIPs.insert(inAddr).second;
    }

    bool IPFilter::AddAllow(in_addr_t inAddr){
        return allowedIPs.insert(inAddr).second;
    }

    bool IPFilter::isBanned(in_addr_t inAddr) {
        return bannedIPs.find(inAddr) != bannedIPs.end();
    }

    bool IPFilter::isAllowed(in_addr_t inAddr) {
        if(isAllowedAll) return true;
        return allowedIPs.find(inAddr) != allowedIPs.end();
    }
} // hzd