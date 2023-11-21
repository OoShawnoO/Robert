/**
  ******************************************************************************
  * @file           : MissionFactory.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#ifndef ROBERT_MISSIONFACTORY_H
#define ROBERT_MISSIONFACTORY_H

#include "MissionReactor/MissionInterface.h"        /* MissionInterface */

namespace hzd {
    template<typename T>
    using Ptr = std::shared_ptr<T>;

    class MissionFactory {
        MissionFactory() = default;
    public:
        static Ptr<MissionInterface> Create(const std::string& missionName);
    };

} // hzd

#endif //ROBERT_MISSIONFACTORY_H
