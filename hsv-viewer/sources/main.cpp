//
// main.cpp
// C++ source file
// Version : 1.0
// Project : opencv/hsv-viewer
//
// Created by Teppei Kobayashi
// Last Modified 2016/11/25
//


/*========================================================================*/

/*----------
  library declaration
  ----------*/
// standard C++ libraries
#include <iostream>

// OpenCV libraries
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
// #include "opencv2/photo.hpp"

/*----------
  namespace declaration
  ----------*/
using namespace std;
using namespace cv;

/*========================================================================*/

/*
  main function
*/
int main(int argc, char *argv[]){

  // declearation and load image file.
  Mat src = imread(argv[1]);

  // scale image.
  float scale = atof(argv[2]);
  Mat image = Mat::ones(src.rows * scale, src.cols * scale, src.type());
  resize(src, image, image.size(), INTER_CUBIC);

  // error.
  if(argc > 3){
    cout << "[error] invalid syntax." << endl;
    return -1;
  } else if(image.data == NULL){
    cout << "[error] bad argument." << endl;
    return -1;
  }

  // decleration vector<Mat> for hsv.
  Mat hsvImage;
  vector<Mat> hsvSplit;
  // color space convert.
  cvtColor(image, hsvImage, COLOR_RGB2HSV);
  split(hsvImage, hsvSplit);

  /*
  // binarize image.
  for(int i=0; i<(int)hsvSplit.size(); i++){
    fastNlMeansDenoising(hsvSplit[i], hsvSplit[i], 15, 3, 7);
    normalize(hsvSplit[i], hsvSplit[i], 0, 255, NORM_MINMAX, CV_8UC3);
    //Laplacian(hsvSplit[i], hsvSplit[i], hsvSplit[i].type(), 3, 1);
    adaptiveThreshold(hsvSplit[i], hsvSplit[i], 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11, 11);
    dilate(hsvSplit[i], hsvSplit[i], Mat(), Point(-1, -1), 1);
    erode(hsvSplit[i], hsvSplit[i], Mat(), Point(-1, -1), 1);
  }
  */

  // show converted images.
  namedWindow("rgb", CV_WINDOW_AUTOSIZE);
  namedWindow("hsv", CV_WINDOW_AUTOSIZE);
  namedWindow("hsv - hue", CV_WINDOW_AUTOSIZE);
  namedWindow("hsv - saturation", CV_WINDOW_AUTOSIZE);
  namedWindow("hsv - value", CV_WINDOW_AUTOSIZE);

  imshow("rgb", image);
  imshow("hsv", hsvImage);
  imshow("hsv - hue", hsvSplit[0]);
  imshow("hsv - saturation", hsvSplit[1]);
  imshow("hsv - value", hsvSplit[2]);

  waitKey(0);

  return 0;
}
