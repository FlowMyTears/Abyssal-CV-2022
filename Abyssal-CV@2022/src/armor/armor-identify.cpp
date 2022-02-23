//
// Created by sleepingmachine on 2022/1/9.
//
#include "armor-identify.hpp"

extern std::mutex mutex1;
extern std::atomic_bool CameraisOpen;

//默认hsv颜色阈值
static int hmin = 63;
static int hmax = 255;
static int smin = 0;
static int smax = 255;
static int vmin = 235;
static int vmax = 255;

//默认二值化操作阈值
static int open = 1;
static int cclose = 18;
static int erode = 3;
static int dilate = 12;

static std::vector<std::vector<cv::Point2i>> allContours;
static std::vector<cv::Vec4i> hierarchy;
static std::vector<cv::RotatedRect> filteredLightBars;

static ArmorPara armorPara = ArmorParaFactory::getArmorPara();

cv::Mat IdentifyArmor::src(480, 640, CV_8UC3);
cv::Mat IdentifyArmor::srcHSV(480, 640, CV_8UC3);
cv::Mat IdentifyArmor::maskHSV(480, 640, CV_8UC3);
cv::Mat IdentifyArmor::dstHSV(480, 640, CV_8UC3);

IdentifyArmor::IdentifyArmor() {}

void IdentifyArmor::IdentifyStream(cv::Mat *pFrame) {
//#if DEBUG
    CreatTrackbars();
//#endif
    cv::Mat temp;
    while(CameraisOpen) {
        mutex1.lock();
        temp = *pFrame;
        mutex1.unlock();
        temp.copyTo(src);
        //cv::cvtColor(src, src, cv::COLOR_RGB2BGR);
        if (src.empty()) {
            std::cout << "Get Frame Fail" << std::endl;
            break;
        }

        ImagePreprocess(src);
        FindLightbar(dstHSV);
        LightBarsPairing();

        //cv::imshow("mask", maskHSV);
        cv::imshow("preprocessed dst", dstHSV);
        cv::imshow("src", src);
        cv::waitKey(5);
    }
}

void IdentifyArmor::CreatTrackbars() {
    cv::namedWindow("阈值调整",cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("hmin", "阈值调整",&::hmin, 255,NULL);
    cv::createTrackbar("hmax", "阈值调整",&::hmax, 255,NULL);
    cv::createTrackbar("smin", "阈值调整",&::smin, 255,NULL);
    cv::createTrackbar("smax", "阈值调整",&::smax, 255,NULL);
    cv::createTrackbar("vmin", "阈值调整",&::vmin, 255,NULL);
    cv::createTrackbar("vmax", "阈值调整",&::vmax, 255,NULL);
    cv::createTrackbar("open", "阈值调整",&::open, 10,NULL);
    cv::createTrackbar("close", "阈值调整",&::cclose, 30,NULL);
    cv::createTrackbar("erode", "阈值调整",&::erode, 10,NULL);
    cv::createTrackbar("dilate", "阈值调整",&::dilate, 20,NULL);
}

void IdentifyArmor::FindLightbar(cv::Mat &preprocessedImage) {
    cv::findContours(preprocessedImage, ::allContours, ::hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < ::allContours.size(); ++i) {
        if (::hierarchy[i][3] == -1) {
            cv::RotatedRect scanRect = cv::minAreaRect(::allContours[i]);                    //检测最小面积的矩形

            cv::Point2f vertices[4];
            scanRect.points(vertices);
            //std::cout << ArmorTool::getRectLengthWidthRatio(scanRect) << std::endl;
            cv::circle(src, cv::Point(vertices[0].x,vertices[0].y), 1, cv::Scalar(255, 255, 0), 2);  // 画半径为1的圆(画点）
            cv::circle(src, cv::Point(vertices[1].x,vertices[1].y), 1, cv::Scalar(255, 0, 255), 2);  // 画半径为1的圆(画点）
            cv::circle(src, cv::Point(vertices[2].x,vertices[2].y), 1, cv::Scalar(20, 243, 32), 2);  // 画半径为1的圆(画点）
            cv::circle(src, cv::Point(vertices[3].x,vertices[3].y), 1, cv::Scalar(255, 0, 0), 2);  // 画半径为1的圆(画点）
            cv::circle(src, cv::Point(vertices[4].x,vertices[4].y), 1, cv::Scalar(	0, 255, 255), 2);  // 画半径为1的圆(画点）

            //std::cout << "sfhdiwuda" << fabs(vertices[0].y - vertices[2].y) / fabs(vertices[0].x - vertices[2].x) << std::endl;

            if(fabs(GetTwoPiontDistance(vertices[1], vertices[0])) >= fabs(GetTwoPiontDistance(vertices[1], vertices[2])))
            {
                if (GetTwoPiontDistance(vertices[1], vertices[0]) / GetTwoPiontDistance(vertices[1], vertices[2]) < ::armorPara.minLightBarAspectRatio || GetTwoPiontDistance(vertices[1], vertices[0]) / GetTwoPiontDistance(vertices[1], vertices[2]) > ::armorPara.maxLightBarAspectRatio)
                {
                    continue;
                }

            }
            else{ if (GetTwoPiontDistance(vertices[1], vertices[2]) / GetTwoPiontDistance(vertices[1], vertices[0]) < ::armorPara.minLightBarAspectRatio || GetTwoPiontDistance(vertices[1], vertices[2]) / GetTwoPiontDistance(vertices[1], vertices[0]) > ::armorPara.maxLightBarAspectRatio)
                {
                    continue;
                }

            }
            if(scanRect.size.area() < ::armorPara.minLightBarArea || scanRect.size.area() > ::armorPara.maxLightBarArea)
            {
                continue;
            }

            float longSide = ArmorTool::findExtremumOfSide(scanRect, LONG_SIDE);
            float shortSide = ArmorTool::findExtremumOfSide(scanRect, SHORT_SIDE);
            if (longSide > ::armorPara.maxLightBarLength || longSide < ::armorPara.minLightBarLength || shortSide > ::armorPara.maxLightBarWidth || shortSide < ::armorPara.minLightBarWidth)
                continue;

            filteredLightBars.push_back(scanRect);

            for (int j = 0; j < 4; j++)
            {
                cv::line(src, vertices[j], vertices[(j + 1) % 4], cv::Scalar(219,112,147),1);
            }
            //std::cout << "scanRect.size.area():" << scanRect.size.area() << std::endl;

            /*
            //rect的高度、和宽度有一个小于板灯的最小高度就直接跳过本次循环
            float longSide = Util::findExtremumOfSide(scanRect, LONG_SIDE);
            float shortSide = Util::findExtremumOfSide(scanRect, SHORT_SIDE);
            //std::cout << "longSide:" << longSide << std::endl;
            //std::cout << "shortSide:" << shortSide << std::endl;
            if (longSide > _para.maxLightBarLength || longSide < _para.minLightBarLength || shortSide > _para.maxLightBarWidth || shortSide < _para.minLightBarWidth)
                continue;

            if (longSide > shortSide * 8.0f || longSide < shortSide * 1.5f)
                continue;

            possibleRects.push_back(scanRect);
            //std::cout << "*" << std::endl;*/
        }
    }
}

void IdentifyArmor::ImagePreprocess(const cv::Mat &src) {
    cv::cvtColor(src, srcHSV, CV_BGR2HSV, 0);

    cv::inRange(srcHSV, cv::Scalar(::hmin, ::smin, ::vmin), cv::Scalar(::hmax, ::smax, ::vmax), maskHSV);
    //cv::inRange(srcHSV, cv::Scalar(40,120,230), cv::Scalar(80, 120, 255), maskHSV);
    morphologyEx(maskHSV, dstHSV, 2, getStructuringElement(cv::MORPH_RECT,cv::Size(::open,::open)));
    morphologyEx(dstHSV, dstHSV, 3, getStructuringElement(cv::MORPH_RECT,cv::Size(::cclose,::cclose)));
    morphologyEx(dstHSV, dstHSV, 0, getStructuringElement(cv::MORPH_RECT,cv::Size(::erode,::erode)));
    morphologyEx(dstHSV, dstHSV, 1, getStructuringElement(cv::MORPH_RECT,cv::Size(::dilate,::dilate)));
}

float IdentifyArmor::GetTwoPiontDistance(cv::Point2f point_1, cv::Point2f point_2) {
    return sqrtf(pow((point_1.x - point_2.x), 2) + pow((point_1.y - point_2.y), 2));
}

void IdentifyArmor::LightBarsPairing() {
    if (filteredLightBars.size() < 2) {
        return;
    }
    cv::Point2f shortCenter0[2];
    cv::Point2f shortCenter1[2];
    for (int i = 0; i < filteredLightBars.size()-1; ++i) {
        
    }
}




