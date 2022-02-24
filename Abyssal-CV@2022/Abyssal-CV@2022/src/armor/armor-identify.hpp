//
// Created by sleepingmachine on 2022/1/9.
//
#ifndef ABYSSAL_CV_2022_ARMOR_IDENTIFY_HPP
#define ABYSSAL_CV_2022_ARMOR_IDENTIFY_HPP

#include "../asset/RoboMasterConfig.hpp"
#include "../src/armor/armor-tool.hpp"
#include "./armor-tool.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <iostream>
#include <atomic>

class IdentifyArmor{
private:

    struct ArmorStruct {
        cv::RotatedRect partLightBars[2];
        cv::RotatedRect armorRect;
        float lightAngle;
        bool armorType = false;
        int carId = 0;
    };
    static cv::Mat src;
    static cv::Mat srcHSV;
    static cv::Mat maskHSV;
    static cv::Mat dstHSV;

    static int hmin;
    static int hmax;
    static int smin;
    static int smax;
    static int vmin;
    static int vmax;
    static int open;
    static int cclose;
    static int erode;
    static int dilate;

    //static std::vector<std::vector<cv::Point2i>> allContours;
    //static std::vector<cv::Vec4i> hierarchy;
    //static std::vector<cv::RotatedRect> filteredLightBars;

    static ArmorPara armorPara;

    //void ImagePreprocess(const cv::Mat &src, EnemyColor enemyColor);
    static void ImagePreprocess(const cv::Mat &src);
    static void CreatTrackbars();
    static void FindLightbar(cv::Mat &preprocessed);
    static void LightBarsPairing();


public:
    IdentifyArmor();
    ~IdentifyArmor() {}


    static void IdentifyStream(cv::Mat* pFrame);


};

    //std::vector<cv::Mat> splitSrc;


#endif //ABYSSAL_CV_2022_ARMOR_IDENTIFY_HPP
