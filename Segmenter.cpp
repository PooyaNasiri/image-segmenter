#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
int CT = 100, T1 = 1;
const int addArae = 100000;
cv::Scalar color;
cv::Mat src;
int mouse_x, mouse_y;

cv::Mat RemoveAreas(cv::Mat mask)
{
    cv::Mat labels, stats, centroids;
    int nLabels = cv::connectedComponentsWithStats(mask, labels, stats, centroids);
    // std::cout << "nLabels" << nLabels << std::endl;
    for (int ms = 0; nLabels > T1+1 && ms < (mask.rows * mask.cols) / 4; ms += 50)
    {
        for (int i = 1; i < nLabels; i++)
        {
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            if (area < ms)
            {
                cv::Mat mask_i = (labels == i);
                mask.setTo(0, mask_i);
            }
        }
        nLabels = cv::connectedComponentsWithStats(mask, labels, stats, centroids);
        // std::cout << ms << std::endl;
    }
    // cv::imshow("Result Mask", result);
    return mask;
}

cv::Mat AddAreas(cv::Mat mask)
{
    cv::Mat labels, stats, centroids;
    mask = 255 - mask;
    int nLabels = cv::connectedComponentsWithStats(mask, labels, stats, centroids);
    for (int i = 1; i < nLabels; i++)
    {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area < addArae)
        {
            cv::Mat mask_i = (labels == i);
            mask.setTo(1, mask_i);
        }
    }
    mask = 255 - mask;
    // cv::imshow("mask", mask);
    return mask;
}

cv::Mat Mask(cv::Mat img, cv::Scalar mean, int T, int size)
{
    cv::Mat mask_img(src.rows, src.cols, CV_8UC1, cv::Scalar(0));

    int startX = mouse_x - size + 1;
    int startY = mouse_y - size + 1;
    int width = size * 2 - 1;
    int hight = size * 2 - 1;

    if (startX < 0)
        startX = 1;
    if (startY < 0)
        startY = 1;
    if (width + startX > img.cols)
        width = img.cols - startX - 1;
    if (hight + startY > img.rows)
        hight = img.rows - startY - 1;

    // cv::imshow("Cropped Image", img(cv::Rect(startX, startY, width, hight)));
    for (int y = startY; y < hight + startY; y++)
        for (int x = startX; x < width + startX; x++)
            if (abs(src.at<cv::Vec3b>(cv::Point(x, y))(0) - mean[0]) < T && abs(src.at<cv::Vec3b>(cv::Point(x, y))(1) - mean[1]) < T && abs(src.at<cv::Vec3b>(cv::Point(x, y))(2) - mean[2]) < T)
                mask_img.at<uchar>(y, x) = 255;
    return mask_img;
}
int MaxCropSize(cv::Mat img)
{
    if (mouse_x == 0 || mouse_y == 0)
        return 0;
    int maxSize = std::max(mouse_x, mouse_y);
    maxSize = std::max(maxSize, img.rows - mouse_y);
    maxSize = std::max(maxSize, img.cols - mouse_x);
    if (maxSize > img.rows && maxSize > img.cols)
        maxSize = std::max(img.rows, img.cols);
    return maxSize;
}
cv::Mat Dilate(cv::Mat img)
{
    cv::Mat dilated_mask;
    cv::dilate(img, dilated_mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    return dilated_mask;
}

cv::Mat Segment(cv::Scalar color, int colorT)
{
    cv::Mat img = src.clone();
    const unsigned int kernelSize = 5;
    cv::GaussianBlur(img, img, cv::Size(kernelSize, kernelSize), 0);
    int step = std::min(img.rows, img.cols) / 20;
    int size = MaxCropSize(img);
    do
    {
        img = Mask(img, color, colorT, size);
        img = RemoveAreas(img);
        img = Dilate(img);
        img = AddAreas(img);
        size -= step;
    } while (!(img.at<uchar>(mouse_y, mouse_x) != 0) && (size > 0));
    cv::Mat result;
    src.copyTo(result, img);
    cv::imshow("Masked", result);
    return img;
}

static void RefreshTrackBars(int, void *)
{
    // cv::imshow("Mask", Segment(color, CT));
}

void OnMouse(int event, int x, int y, int flags, void *)
{
    cv::Mat img = src.clone();
    const unsigned int neighborhood = 3;
    mouse_x = x;
    mouse_y = y;
    if (event == cv::EVENT_LBUTTONDOWN || event == cv::EVENT_RBUTTONDOWN)
    {
        if (y + neighborhood > img.rows || x + neighborhood > img.cols)
            return;
        color = cv::mean(img(cv::Rect(x, y, neighborhood, neighborhood)));
        cv::imshow("Mask", Segment(color, CT));
    }
}

static void CreateTrackBars()
{
    cv::createTrackbar("Color Th", "image", &CT, 255, RefreshTrackBars);
    cv::createTrackbar("Objs", "image", &T1, 10, RefreshTrackBars);
    // cv::createTrackbar("add T", "image", &T2, MaxT, RefreshTrackBars);
    // cv::imshow("Mask", Segment(color, CT, T1, T2));
    RefreshTrackBars(0, 0);
}

static void OpenCamera()
{
    cv::VideoCapture cap;
    cap.open(0, cv::CAP_ANY);
    for (;;)
    {
        cap.read(src);
        cv::imshow("image", src);
        if (cv::waitKey(5) >= 0)
            break;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        OpenCamera();
    else
    {
        src = cv::imread(argv[1]);
        if (src.empty())
            return -1;
    }
    cv::imshow("image", src);
    CreateTrackBars();
    cv::setMouseCallback("image", OnMouse, NULL);
    cv::waitKey(0);
    return 0;
}