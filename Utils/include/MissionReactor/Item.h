/**
  ******************************************************************************
  * @file           : Item.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/20
  ******************************************************************************
  */

#ifndef ROBERT_ITEM_H
#define ROBERT_ITEM_H

#include <opencv2/core.hpp>     /* cv::Rect */

namespace hzd {
    // 规定: -1 表示空ID, >= 0 对应ID
    using ItemID = int;

    struct Item : cv::Rect {
        Item() = default;
        Item(int x,int y,int width,int height);
        explicit Item(const cv::Rect&);
        /**
          * @brief 相交 / Cross
          * @param A Item A
          * @param B Item B
          * @retval true 相交 / false 不相交
          */
        static bool Cross(const Item& A,const Item& B);
        /**
          * @brief 包含 / Consist
          * @param A Item A
          * @param B Item B
          * @retval true A包含B / false A不包含B
          */
        static bool Consist(const Item& A,const Item& B);
        /**
          * @brief 相离 / Separation
          * @param A Item A
          * @param B Item B
          * @retval true A与B相离 / false A与B相交
          */
        static bool Separation(const Item& A,const Item& B);
        /**
          * @brief 距离 / Distance
          * @param A Item A
          * @param B Item B
          * @retval A中心到B中心的距离
          */
        static double Distance(const Item& A,const Item& B);

    };

} // hzd

#endif //ROBERT_ITEM_H
