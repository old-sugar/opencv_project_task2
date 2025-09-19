### 安装OpenCV库
从git上clone源码后使用cmake编译安装即可

安装过程中，由于本机安装了anaconda导致编译失败，修改相关配置文件后就能够成功编译安装

### 图像处理操作
基本在第二次培训文档中都有，没有什么需要特别关注的，代码中给出了必要的注释

此处对教程没有说明的几个操作做一些记录

##### 1. 漫水处理(``cv::floodFill()``)

漫水处理的效果在[OpenCV官方文档](https://docs.opencv.org/4.x/d7/d1b/group__imgproc__misc.html#ga366aae45a6c1289b341d140839f18717 "点击跳转官方文档")中是这么说的:

> Fills a connected component with the given color.

基本意思就是将相连的部分用指定颜色去填充

那么什么是相连的部分呢？和周围已知 相连于seed point的点 的 差值在一定范围内，那么我们就认为这个点也和seed point相连

这就是漫水处理




##### 2. 旋转和裁剪



### 识别装甲板




