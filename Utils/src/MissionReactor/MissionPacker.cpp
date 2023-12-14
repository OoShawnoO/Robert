/**
  ******************************************************************************
  * @file           : MissionPacker.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#include <iostream>                         /* cerr */

#include "MissionReactor/MissionPacker.h"   /* MissionPacker */

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
     *      ______________________________________ \n
     *      | 4 Byte | 64 Byte | 4 Byte | 16 Byte | \n
     *     _______________________________________ \n
     *      |  type  |  name   |  flag  | parameter | \n
     *      ______________________________________ \n
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
     *      8) 写入 8 字节 Item 个数 \n
     *      9) 写入 4 X Item个数 字节 ItemID \n
     *      10) [optional] 写入边界 4 X 4 字节 (x,y,w,h)   \n
     *      11) [optional] 写入时间间隔 4 字节  (精度为ms)   \n
     */


namespace hzd {

    MissionPacker::MissionPacker(const std::string &fileName) {
        out.open(fileName,std::ios::out);
        if(!out.is_open()){
            std::cerr << "failed to creator or open file:" << fileName << std::endl;
            exit(-1);
        }
    }

    MissionPacker::~MissionPacker() {
        Save();
    }

    void MissionPacker::Save() {
        out.close();
    }

    bool MissionPacker::WriteFunction(
            const std::string                               &functionName,
            const std::vector<MissionInterface::Parameter>  &parameters
    )
    {
        out.write(_func,4);
        if(functionName.size() >= 64) {
            std::cerr << "function:" << functionName << "'s name size >= 64 Byte";
            return false;
        }
        char buffer[64] = {0};
        strcpy(buffer,functionName.c_str());
        out.write(buffer,64);
        auto size = parameters.size();
        out.write((const char*)&size,sizeof(size_t));
        for(const auto& param : parameters) {
            out.write((const char*)&param,PARAMETER_SIZE);
        }
        return true;
    }

    bool MissionPacker::WriteItem(
            const std::string                               &itemName,
            ItemID                                          itemID
    )
    {
        out.write(_item,4);
        if(itemName.size() >= 64) {
            std::cerr << "item:" << itemName << "'s name size >= 64 Byte";
            return false;
        }
        char buffer[64] = {0};
        strcpy(buffer,itemName.c_str());
        out.write(buffer,64);
        out.write((const char*)&itemID,sizeof(ItemID));
        return true;
    }

    bool MissionPacker::WriteSettlement(
            Settlement::StartType                           startType,
            Settlement::EndType                             endType,
            const std::vector<ItemID>&                      startItemsID,
            const std::vector<ItemID>&                      endItemsID,
            const Item                                      &startBound,
            const Item                                      &endBound,
            int                                             startDuration,
            int                                             endDuration
    )
    {
        out.write(_sett,4);

        out.write((const char*)&startType,SETTLEMENT_STARTTYPE_SIZE);
        auto startSize = startItemsID.size();
        out.write((const char*)&startSize,sizeof(size_t));
        for(const auto& itemID : startItemsID) {
            out.write((const char*)&itemID,sizeof(ItemID));
        }
        switch (startType) {
            case Settlement::StartItemAppear : { break; }
            case Settlement::StartItemAppearInBound : {
                if(startBound.empty()) {
                    std::cerr << "need bound." << std::endl;
                    return false;
                }
                out.write((const char*)&startBound.x,sizeof(int));
                out.write((const char*)&startBound.y,sizeof(int));
                out.write((const char*)&startBound.width,sizeof(int));
                out.write((const char*)&startBound.height,sizeof(int));
                break;
            }
            case Settlement::StartItemStayInBound   : {
                if(startBound.empty()) {
                    std::cerr << "need bound." << std::endl;
                    return false;
                }
                out.write((const char*)&startBound.x,sizeof(int));
                out.write((const char*)&startBound.y,sizeof(int));
                out.write((const char*)&startBound.width,sizeof(int));
                out.write((const char*)&startBound.height,sizeof(int));

                out.write((const char*)&startDuration,sizeof(int));
            }
        }

        out.write((const char*)&endType,SETTLEMENT_ENDTYPE_SIZE);
        auto endSize = endItemsID.size();
        out.write((const char*)&endSize,sizeof(size_t));
        for(const auto& itemID : endItemsID) {
            out.write((const char*)&itemID,sizeof(ItemID));
        }
        switch(endType) {
            case Settlement::EndItemDisappear : { break; }
            case Settlement::EndItemAppearInBound :
            case Settlement::EndItemDisappearInBound : {
                if(endBound.empty()) {
                    std::cerr << "need bound." << std::endl;
                    return false;
                }
                out.write((const char*)&endBound.x,sizeof(int));
                out.write((const char*)&endBound.y,sizeof(int));
                out.write((const char*)&endBound.width,sizeof(int));
                out.write((const char*)&endBound.height,sizeof(int));
                break;
            }
            case Settlement::EndItemStayInBound : {
                if(endBound.empty()) {
                    std::cerr << "need bound." << std::endl;
                    return false;
                }
                out.write((const char*)&endBound.x,sizeof(int));
                out.write((const char*)&endBound.y,sizeof(int));
                out.write((const char*)&endBound.width,sizeof(int));
                out.write((const char*)&endBound.height,sizeof(int));
                out.write((const char*)&endDuration,sizeof(int));
            }
        }

        return true;
    }
} // hzd
