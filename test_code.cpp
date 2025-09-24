#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
int main()
{

    // Replace the path with the actual path to your image
    std::string path = "REPLACE THIS PATH";
    cv::Mat img = cv::imread(path);

    if (img.empty()) {
        std::cerr << "Could not read the image with path: " << path << std::endl;
        return 1;
    }

    namedWindow("First OpenCV Application", WINDOW_AUTOSIZE);
    cv::imshow("First OpenCV Application", img);
    cv::moveWindow("First OpenCV Application", 0, 45);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}