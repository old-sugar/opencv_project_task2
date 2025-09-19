#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;

int main(){
    cv::Mat src1;
    cv::Mat HSVimg;
    cv::Mat gray;
    cv::Mat blurImg;
    cv::Mat gaussianImg;
    cv::Mat medianImg;
    cv::Mat RedImg1;
    cv::Mat RedImg2;
    // 读取测试图片（小花）
    src1 = cv::imread("resources/test_image.png",cv::IMREAD_COLOR);
    if (src1.empty()){
        cout << "test img not found" << endl;
        return 1;
    }
    // 图像颜色空间转换
    // 转化为灰度图
    cv::cvtColor(src1, gray, cv::COLOR_BGR2GRAY);
    // 转化为HSV
    cv::cvtColor(src1, HSVimg, cv::COLOR_BGR2HSV);
    
    // 各种滤波操作
    // 均值滤波
    cv::blur(HSVimg, blurImg, cv::Size(5,5));
    // 高斯滤波
    cv::GaussianBlur(HSVimg, gaussianImg, cv::Size(5,5), 0);

    // 特征提取
    // 提取红色区域(红色分两部分提取，完成后相加即可)

    cv::inRange(gaussianImg, cv::Scalar(0,43,46),cv::Scalar(10,255,255), RedImg1);
    cv::inRange(gaussianImg, cv::Scalar(156,43,46),cv::Scalar(180,255,255), RedImg2);

    cv::Mat RedImg = RedImg1 + RedImg2;

    // 寻找红色外轮廓
    // 借助刚刚提取的红色区域即可
    // 同时框出对应的红色的bounding box, 计算轮廓面积

    cv::Mat temp1(src1.rows, src1.cols,CV_8UC3, cv::Scalar(0,0,0));
    src1.copyTo(temp1);
    vector<vector<cv::Point2i>> contours1;
    vector<cv::Vec4i> hier1;

    cv::findContours(RedImg, contours1, hier1, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    for (int i = 0; i+1; i=hier1[i][0]){
        double S = cv::contourArea(contours1[i]);
        
        // 对所有外轮廓进行筛选，较小的轮廓将被忽略
        
        if (S > 1000){ 
            cv::drawContours(temp1,contours1, i, cv::Scalar(0,0,255),1);
            cout << "The Area of Contours " << i << " is " << S << endl;

            cv::Rect boundingBox = cv::boundingRect(contours1[i]);
            cv::rectangle(temp1, boundingBox, cv::Scalar(0,0,255));
        }
    }
    //cv::imshow("red img", temp1);
    //cv::waitKey(0);
    
    // 提取高亮颜色区域并进行图形学处理
    // 灰度化: 转化为灰度图中已经演示过了

    // 二值化
    cv::Mat BinImg;

    cv::threshold(gray, BinImg, 128, 255, cv::THRESH_BINARY_INV); // 由于天空亮度过高，考虑反转
    // cv::imshow("threshold", BinImg);
    // cv::waitKey(0);    
    
    // 膨胀操作
    cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)); 
    cv::Mat dilateImg;
    cv::dilate(BinImg, dilateImg, kernel1);
    
    //cv::imshow("dilate Img", dilateImg);
    //cv::waitKey(0);

    // 腐蚀操作
    cv::Mat erodeImg;
    cv::erode(BinImg, erodeImg, kernel1);
    
    // cv::imshow("erode Img", erodeImg);
    // cv::waitKey(0);

    // 漫水操作:从左上角的点开始漫水，把天空换成绿幕
    cv::Mat floodFillImg;
    src1.copyTo(floodFillImg);
    cv::Rect zeroRec;
    cv::floodFill(floodFillImg, cv::Point(cv::Size(1,1)), cv::Scalar(0,255,0),&zeroRec, cv::Scalar(3,3,3),cv::Scalar(3,3,3));
    // cv::imshow("floodFill Img", floodFillImg);
    // cv::waitKey(0);

    // 图像绘制
    // 此处绘制方形，圆形和文字
    cv::Mat background1(720,1280,CV_8UC3);
    cv::circle(background1, cv::Point(200,200), 100, cv::Scalar(255,255,255));
    cv::rectangle(background1, cv::Point(600, 200), cv::Point(800, 400), cv::Scalar(255,255,255));
    cv::putText(background1,"Hello World", cv::Point(640, 500),cv::FONT_HERSHEY_COMPLEX, 1,cv::Scalar(255,255,255));
    // cv::imshow("square & circles & text",background1);
    // cv::waitKey(0);

    // 红色外轮廓和bounding box前面已经绘制过

    // 对图像进行处理
    // 图像旋转35度
    cv::Mat rotatedMat= cv::getRotationMatrix2D(cv::Point(640,360), 35, 1);
    cv::Mat rotatedImg;
    cv::warpAffine(src1, rotatedImg, rotatedMat, src1.size());
    // cv::imshow("rotated Img", rotatedImg);
    // cv::waitKey(0);

    // 图像裁剪为原图左上角1/4
    cv::Mat cutImg = src1(cv::Range(0,(src1.size().height)/2),cv::Range(0, src1.size().width/2));
    // cv::imshow("Cut Img", cutImg);
    // cv::waitKey(0);








    cv::Mat src;

    /* 以下是识别装甲板
    -------------------------------------------------------------------------------------------*/

    

    /*
    读取图片，读入的是彩色图
    同时判断是否读取失败
    */
    src = cv::imread("resources/test_image_2.png", cv::IMREAD_COLOR);
    if (src.empty()){
        cout << "img not found" << endl;
        return 1;
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
    综合考虑亮度和蓝色的数值(可能超出范围)
    */
    cv::Mat finalScore;
    finalScore = 0.80 * channels[0] + 0.1 * channels[1] + 0.1 * channels[2];

    // 二值化
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(6,6));
    cv::threshold(finalScore, gray,250,255,cv::THRESH_BINARY);
    
    cv::morphologyEx(gray,gray,cv::MORPH_OPEN,kernel);
    
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
            cout << rotRect.size.area() << endl;
            cv::drawContours(res, contours,i,cv::Scalar(255,255,255));
        } 
    }
    cv::rectangle(src,(pts[0][0]+cv::Point2f(0,100)), pts[1][2]+cv::Point2f(0,-100),cv::Scalar(0,255,0),5,16);


    // 显示图片,同时调整大小，不至于铺满整个屏幕
    cv::resize(res, res, cv::Size(1280, 720));
    cv::resize(src, src, cv::Size(1280, 720));
    cv::imshow("result", res);
    cv::imshow("src", src);
    cv::imwrite("./resources/ArmorRecognized.jpg", src);
    cv::waitKey(0);
    return 0;
}
