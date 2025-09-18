#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;

void DrawRotatedRect(cv::Mat mask,const cv::RotatedRect &rotatedrect,const cv::Scalar &color,int thickness, int lineType)
{
    // 提取旋转矩形的四个角点
	cv::Point2f ps[4];
	rotatedrect.points(ps);
 
    // 构建轮廓线
	std::vector<std::vector<cv::Point>> tmpContours;    // 创建一个InputArrayOfArrays 类型的点集
	std::vector<cv::Point> contours;
	for (int i = 0; i != 4; ++i) {
		contours.emplace_back(cv::Point2i(ps[i]));
	}
	tmpContours.insert(tmpContours.end(), contours);
 
    // 绘制轮廓，即旋转矩形
	drawContours(mask, tmpContours, 0, color,thickness, lineType);  // 填充mask
}

int main(){
    cv::Mat src;
    cv::Mat gray;
    cv::Mat blurImg;
    cv::Mat gaussianImg;
    cv::Mat medianImg;

    

    /*
    读取图片，读入的是彩色图
    同时判断是否读取失败
    */
    src = cv::imread("resources/test_image_2.png", cv::IMREAD_COLOR);
    if (src.empty()){
        cout << "img not found" << endl;
        return 0;
    }
    cv::Mat res(src.rows,src.cols, CV_8UC3, cv::Scalar(0,0,0));
    // 使用滤波函数
    cv::GaussianBlur(src, blurImg,cv::Size(3,3),0);
    /*
    将通道分离，选取蓝色通道进行操作
    */
    cv::Mat channels[3];
    cv::split(blurImg, channels);


    /*
    转化灰度图
    */
    //cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    

    /*
    综合考虑亮度和蓝色的数值(可能超出范围)
    */
    cv::Mat finalScore;
    finalScore = 0.80 * channels[0] + 0.1 * channels[1] + 0.1 * channels[2];

    // 二值化
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(6,6));
    cv::threshold(finalScore, gray,250,255,cv::THRESH_BINARY);
    //cv::erode(gray,gray,kernel);
    
    cv::morphologyEx(gray,gray,cv::MORPH_OPEN,kernel);
    // canny边缘检测
    // cv::Canny(channels[0], res, 100, 300);
    
    // 定义存储边缘和结构列表的容器
    vector<vector<cv::Point2i>> contours;
    vector<cv::Vec4i> hier;
    cv::findContours(gray, contours, hier, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    
    // 存储两个灯条关键点的容器
    vector<vector<cv::Point2f>> pts;
    
    for (int i = 0; i+1; i=hier[i][0]){
        cv::RotatedRect rotRect = cv::minAreaRect(contours[i]); 
        cv::Rect rec = cv::boundingRect(contours[i]);
        if (rotRect.size.area() > 900 && rotRect.size.area()/rec.area() >0.9){
            vector<cv::Point2f> temp;
            rotRect.points(temp);
            pts.push_back(temp);        
            DrawRotatedRect(src,rotRect, cv::Scalar(0,0,255), 3, 16);
            DrawRotatedRect(res,rotRect, cv::Scalar(0,0,255), 1, 16);
            cout << rotRect.size.area() << endl;
            cv::drawContours(res, contours,i,cv::Scalar(255,255,255));
        } 
    }
    cv::rectangle(src,pts[0][0], pts[1][3],cv::Scalar(0,255,0),5,16);


    //res = gray;
    // 显示图片,同时调整大小，不至于铺满整个屏幕
    cv::resize(res, res, cv::Size(1280, 720));
    cv::resize(src, src, cv::Size(1280, 720));
    cv::imshow("result", res);
    cv::imshow("src", src);
    cv::imwrite("./resources/ArmorRecognized.jpg", src);
    cv::waitKey(0);
    return 0;
}
