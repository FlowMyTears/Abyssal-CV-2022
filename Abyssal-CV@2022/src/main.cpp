#include "camera/camera-stream.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "armor/armor-identify.hpp"
#include "video/video-save.hpp"

extern std::mutex mutex1;
extern std::atomic_bool CameraisOpen;

int main(int argc, char* argv[]) {
    CameraisOpen = true;
    cv::Mat frame(480, 640, CV_8UC3), gray;

    CameraStream::InitCamera();
    std::thread camera_thread(CameraStream::StreamRetrieve, &frame);
    std::thread armor_thread(IdentifyArmor::IdentifyStream, &frame);
    std::thread video_thread(VideoSave::SaveRunningVideo, &frame);
    /*
    while(CameraisOpen) {
        mutex1.lock();
        frame.copyTo(gray);
        mutex1.unlock();
        if (gray.empty()) {
            std::cout << "Get Frame Fail" << std::endl;
            break;
        }
        cv::imshow("video", gray);
        cv::waitKey(5);
    }
    */
    camera_thread.join();
    armor_thread.join();
    video_thread.join();
    CameraStream::UnInitCamera();

    return 0;
}