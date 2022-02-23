//
// Created by sleepingmachine on 2022/1/12.
//

#ifndef ABYSSAL_CV_2022_ARMOR_TOOL_HPP
#define ABYSSAL_CV_2022_ARMOR_TOOL_HPP
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "cmath"

class ArmorTool{
public:
    static inline float findExtremumOfSide(cv::RotatedRect &rect, bool flag) {
        if (flag)
            return rect.size.height > rect.size.width ? rect.size.height : rect.size.width;
        else
            return rect.size.height < rect.size.width ? rect.size.height : rect.size.width;
    }

    static inline float getRectLengthWidthRatio(cv::RotatedRect &rect) {
        float longSide = rect.size.height > rect.size.width ? rect.size.height : rect.size.width;   //获取识别出矩形长
        float shortSide = rect.size.height < rect.size.width ? rect.size.height : rect.size.width;  //获取识别出矩形宽
        return longSide / shortSide;
    }

    static inline float lineToLineAngle(cv::Point2f &p1, cv::Point2f &p2, cv::Point2f &p3, cv::Point2f &p4) {
        if (p2.x == p1.x) {
            p2.x += 1e-10f;
        }
        if (p3.x == p4.x) {
            p3.x += 1e-10f;
        }
        float tan1 = (p2.y - p1.y) / (p2.x - p1.x);
        float tan2 = (p4.y - p3.y) / (p4.x - p3.x);
        float angle1 = radianToAngle(atanf(tan1));
        float angle2 = radianToAngle(atanf(tan2));
        float skew = fabs(fabs(angle1 - angle2) - 90);
        return 90.0f - skew;
    }

    static inline float radianToAngle(float p) {
        return p * 180.0f / PI_F();
    }

    static inline float PI_F() {
        return float(CV_PI);
    }
};


#endif //ABYSSAL_CV_2022_ARMOR_TOOL_HPP
