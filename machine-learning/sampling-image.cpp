//
// sampiling-image.cpp
// C++ source file
// Version : 1.0
// Project : opencv/machine-learning
//
// Created by Teppei Kobayashi
// Last Modified 2016/10/19
//


/*----------
  Bibliography
  ----------*/
// ご注文は機械学習ですか？ : kivantium.hateblo.jp/entry/2014/11/25/230658
// 物体検出 - OpenCV       : opencv.jp/opencv-2.1/cpp/object_detection.html


/*========================================================================*/

/*----------
  library declaration
  ----------*/
// standard C++ libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

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
  prototype declaration
  ----------*/
void detectAndExport(Mat image);

/*----------
  global variables declaration
  ----------*/
CascadeClassifier face_cascade;
unsigned int imageNum = 0;


/*========================================================================*/

/*
  main function
*/
int main(int argc, char *argv[]){

  // variables.
  stringstream progress;
  int frameNum = 0;
  int totalFrame;

  // load cascade file.
  face_cascade.load("resources/lbpcascade_animeface.xml");

  // load image for recognition test.
  Mat frame;
  VideoCapture video(argv[1]);

  // if failed to open video file.
  if(!video.isOpened()){
    cout << "[error] failed open video file." << endl;
    return -1;
  }
  else{
    cout << "[message] sucessful open video file." << endl;
  }

  // get frame count in loaded video file.
  totalFrame = video.get(CV_CAP_PROP_FRAME_COUNT);

  // export face detected result.
  // export interval : 1 [file] / 10 [flame]
  while(1){
    frameNum++;
    video >> frame;


    // if all video flame processed, break a loop sequence.
    if(frame.empty()){
      cout << "[processing] : 100% "<< "(" << totalFrame<<  "/"  << totalFrame << ")" << endl;
      cout << "[notification] all flame processed." << endl;
      break;
    }
    else{
      int processPer = (double)frameNum/(double)totalFrame*100;
      cout << "[processing] : " << setw(3) << setfill(' ') << processPer << "% "
           << "(" << frameNum <<  "/"  << totalFrame << ")" << "\r" << flush;
    }

    // detect and export image per 10 frames.
    if(frameNum%10 == 0)
      detectAndExport(frame);
  }

  return 0;
}


/*
  detect and export func.
*/
void detectAndExport(Mat image){

  // variables.
  vector<Rect> faces;
  Mat gray_image;
  stringstream fileName;

  // convert RGB to gray scale.
  cvtColor(image, gray_image, COLOR_BGR2GRAY);

  // equalize gray scale image histgram.
  equalizeHist(gray_image, gray_image);

  // detect face.
  face_cascade.detectMultiScale(gray_image, faces, 1.1, 3, 0, Size(80, 80));

  // show detect result.
  for(unsigned int i = 0; i<faces.size(); i++){

    // face: region of detected region in source flame.
    Mat face = image(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));

    // export detected result: set file name
    fileName.str("");
    fileName << "sample/image" << setw(3) << setfill('0') << imageNum << ".png";

    // export detected result
    imwrite(fileName.str(), face);
    imageNum++;
  }
}
