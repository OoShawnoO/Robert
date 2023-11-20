/**
  ******************************************************************************
  * @file           : Item.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/20
  ******************************************************************************
  */

#include "MissionReactor/Item.h"

namespace hzd {

    Item::Item(int x, int y, int width, int height) : Rect_(x, y, width, height) {}

    Item::Item(const cv::Rect & _rect) : Rect_(_rect) {}

    bool Item::Cross(const Item &A, const Item &B) {
        return A.x < B.x + B.width
            && A.y < B.y + B.height
            && A.x + A.width > B.x
            && A.y + A.height > B.y;
    }

    bool Item::Consist(const Item &A, const Item &B) {
        return A.x <= B.x
            && A.x + A.width >= B.x + B.width
            && A.y <= B.y
            && A.y + A.height >= B.y + B.height;
    }

    bool Item::Separation(const Item &A, const Item &B) {
        return !Cross(A,B);
    }

    double Item::Distance(const Item &A, const Item &B) {
        cv::Point centerA = {A.x + A.width/2,A.y + A.height/2};
        cv::Point centerB = {B.x + B.width/2,B.y + B.height/2};
        return sqrt(
                (centerA.x - centerB.x)*(centerA.x - centerB.x)
                + (centerA.y - centerB.y)*(centerA.y - centerB.y)
        );
    }

} // hzd
