//
// main.cpp
// C++ source file
// Version : 1.0
// Project : opencv/face-detect
//
// Created by Teppei Kobayashi
// Last Modified 2016/11/25
//


/*----------
  Bibliography
  ----------*/
// OpenCVでアニメ顔検出をやってみた : livantium.hateblo.jp/entry/2014/09/16/231211


/*========================================================================*/

/*----------
  library declaration
  ----------*/
// standard C++ libraries
#include <iostream>

// OpenCV libraries
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

/*----------
  namespace declaration
  ----------*/
using namespace std;
using namespace cv;

/*----------
  global variables declaration
  ----------*/
CascadeClassifier face_cascade;

/*----------
  prototype declaration
  ----------*/
void detectAndDisplay(Mat image);

/*========================================================================*/

/*
  main function
*/
int main(int argc, char *argv[]){

  // load cascade file.
  face_cascade.load("resources/lbpcascade_animeface.xml");

  // load image for recognition test.
  Mat image, resized;
  image = imread(argv[1]);

  // resize image.
  float scale = atof(argv[2]);
  resized = Mat::ones(image.rows * scale, image.cols * scale, CV_8U);
  resize(image, resized, resized.size(), INTER_CUBIC);

  // detect face and display result.
  detectAndDisplay(resized);

  // quit.
  waitKey(0);

  return 0;
}


/*
  detect and display func.
*/
void detectAndDisplay(Mat image){

  // variables.
  vector<Rect> faces;
  Mat gray_image;

  // convert RGB to gray scale.
  cvtColor(image, gray_image, COLOR_BGR2GRAY);

  // equalize gray scale image histgram.
  equalizeHist(gray_image, gray_image);

  // detect face.
  face_cascade.detectMultiScale(gray_image, faces);

  // show detect result.
  for(unsigned int i = 0; i<faces.size(); i++){
    Point center(faces[i].x + faces[i].width/2, faces[i].y +faces[i].height/2);
    ellipse(image, center, Size(faces[i].width/2, faces[i].y/2), 0, 0, 360, Scalar(255, 0, 0), 2, 8, 0);
  }

  // show result.
  imshow("detect result", image);
}
