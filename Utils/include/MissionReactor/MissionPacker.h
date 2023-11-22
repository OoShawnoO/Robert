/**
  ******************************************************************************
  * @file           : MissionPacker.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#ifndef ROBERT_MISSIONPACKER_H
#define ROBERT_MISSIONPACKER_H

#include <fstream>                              /* ofstream */

#include "MissionReactor/MissionInterface.h"    /* MissionInterface Settlement */

namespace hzd {
#define TYPE_EQUAL(T,type) (typeid(T) == typeid(type))
    static const char* _func = "func";
    static const char* _item = "item";
    static const char* _sett = "sett";

    /**
     *  涉及到一些流程控制可能需要防止人为修改，故而采用二进制方式保存流程控制文件。
     *  \n
     *  When it comes to process control, it may be necessary to prevent human modification, so binary storage of process control files is adopted.
     *  \n
     *  规则: \n
     *  1. 写入事件：\n
     *      1) 写入 4 字节 'func' \n
     *      2) 写入 64 字节 事件名称 \n
     *      3) 写入 8 字节 参数个数 \n
     *      4) 写入 n X 16 字节 参数 \n
     *      5) 写入 动态数量 X 4 字节 的两种信号 \n
     *      _____________________________________________________________ \n
     *      | 4 Byte | 64 Byte | 8 Byte | 16 Byte x n |  16 Byte x ? | \n
     *     ______________________________________________________________ \n
     *      |  type  |  name   |  size  | parameter x n | 16 Byte x ? |\n
     *      _____________________________________________________________ \n
     *  2. 写入物体：\n
     *      1) 写入 4 字节 'item' \n
     *      2) 写入 64 字节 Item名称 \n
     *      3） 写入 4 字节 ItemID \n
     *      _____________________________ \n
     *      | 4 Byte | 64 Byte | 4 Byte | \n
     *     ______________________________ \n
     *      |  type  |  name   |   id   | \n
     *      _____________________________ \n
     *  3. 写入结算机制: \n
     *      1) 写入 4 字节 'sett'  \n
     *      2) 写入 4 字节 开始结算类型  \n
     *      3) 写入 8 字节 Item 个数 \n
     *      4) 写入 4 X Item个数 字节 ItemID \n
     *      5) [optional] 写入边界 4 X 4 字节 (x,y,w,h)   \n
     *      6) [optional] 写入时间间隔 4 字节  (精度为ms)   \n
     *
     *      7) 写入 4 字节 结束结算类型 \n
     *      8) 写入 4 字节 Item 个数 \n
     *      9) 写入 4 X Item个数 字节 ItemID \n
     *      10) [optional] 写入边界 4 X 4 字节 (x,y,w,h)   \n
     *      11) [optional] 写入时间间隔 4 字节  (精度为ms)   \n
     */

    class MissionPacker {
        int order{0};
        std::ofstream out;

    public:
        explicit MissionPacker(const std::string& fileName);
        ~MissionPacker();
        void Save();

        bool WriteFunction(
            const std::string&                                functionName,
            const std::vector<MissionInterface::Parameter>&   parameters
        );

        bool WriteItem(
            const std::string&                                 itemName,
            ItemID                                             itemID
        );

        bool WriteSettlement(
            Settlement::StartType                               startType,
            Settlement::EndType                                 endType,
            const std::vector<ItemID>&                          startItemsID,
            const std::vector<ItemID>&                          endItemsID,
            const Item&                                         startBound = {},
            const Item&                                         endBound = {},
            int                                                 startDuration = 0,
            int                                                 endDuration = 0
        );
    };

} // hzd

#endif //ROBERT_MISSIONPACKER_H
