//
// export-result.cpp
// C++ source file
// Version : 1.0
// Project : opencv/machine-learning
//
// Created by Teppei Kobayashi
// Last Modified 2016/10/20
//


/*----------
  Bibliography
  ----------*/
// ご注文は機械学習ですか？ : kivantium.hateblo.jp/entry/2014/11/25/230658


/*========================================================================*/

/*----------
  library declaration
  ----------*/
// standard C++ libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

// OpenCV libraries
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/ml/ml.hpp"

/*----------
  macro declaration
  ----------*/
#define ATTRIBUTES 64           // number of elements in feature vector (64-colors)
#define CLASSES 10              // number of labels (9 peoples and others)

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

/*
  neural network initialization
*/
CvANN_MLP nnetwork;
CvFileStorage* storage;
CvFileNode *n;
VideoWriter exportVideo;


/*========================================================================*/

/*
  main function
*/
int main(int argc, char *argv[]){

  // variables.
  stringstream progress;
  int frameNum = 0;
  int totalFrame;

  // load neural network.
  storage = cvOpenFileStorage("learning-result.xml", 0, CV_STORAGE_READ);
  n = cvGetFileNodeByName(storage, 0, "DigitOCR");
  nnetwork.read(storage, n);
  cvReleaseFileStorage(&storage);

  // load cascade file.
  face_cascade.load("resources/lbpcascade_animeface.xml");

  // load video file.
  Mat frame;
  VideoCapture video(argv[1]);

  // if failed to open video file.
  if(!video.isOpened()){
    cout << "[error] failed import video file." << endl;
    return -1;
  }
  else{
    cout << "[message] sucessful import video file." << endl;
  }

  // set export destination for video file.
  exportVideo.open(argv[2], CV_FOURCC_MACRO('D','X','5','0'),
              video.get(CV_CAP_PROP_FPS),
              Size(video.get(CV_CAP_PROP_FRAME_WIDTH), video.get(CV_CAP_PROP_FRAME_HEIGHT)));

  // if failed to open video file.
  if(!exportVideo.isOpened()){
    cout << "[error] failed export video file." << endl;
    return -1;
  }
  else{
    cout << "[message] sucessful export video file." << endl;
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

    // detect and export.
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
  face_cascade.detectMultiScale(gray_image, faces, 1.1, 3, 0, Size(50, 50));

  // show detect result.
  for(unsigned int i = 0; i<faces.size(); i++){

    // face: region of detected region in source flame.
    Mat face = image(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
    Mat norm (face.size(), face.type());
    int pixelIndex;
    float train[64];
    for(int j=0; j<64; j++){
      train[j] = 0;
    }

    // normalize image.
    normalize(face, norm, 0, 255, NORM_MINMAX, CV_8UC3);

    // generate feature vector.
    for(int y=0; y<norm.rows; y++){
      for(int x=0; x<norm.cols; x++){
        // calc. pixel coordinate
        pixelIndex = y*norm.step + x*norm.elemSize();

        // calc. color vector.
        // color: 0xBBRRGG
        int color = (norm.data[pixelIndex+0]/64)
                  + (norm.data[pixelIndex+1]/64) * 4
                  + (norm.data[pixelIndex+2]/64) * 16;
        train[color] += 1;
      }
    }

    // calc. histgram.
    int pixel = norm.rows * norm.cols;
    for(int j=0; j<64; j++){
      train[j] /= pixel;
    }

    // recognition using neural network.
    Mat data(1, ATTRIBUTES, CV_32F);
    for(int cols=0; cols<=ATTRIBUTES; cols++){
      data.at<float>(0, cols) = train[cols];
    }
    int maxIndex=0;
    Mat classOut(1, CLASSES, CV_32F);
    nnetwork.predict(data, classOut);

    // analysis recognized result value.
    float value;
    float maxValue=classOut.at<float>(0, 0);
    for(int index=1; index<CLASSES; index++){
      value = classOut.at<float>(0, index);
      if(value > maxValue){
        maxValue = value;
        maxIndex = index;
      }
    }

    // show recognized result.
    // label
    // hanayo:1 / rin:2    / maki:3
    // honoka:4 / kotori:5 / umi:6
    // niko:7   / eri:8    / nozomi:9
    // others:0
    Scalar color;
    switch(maxIndex){
        case 0:
          color = Scalar(0, 0, 0);       // others
          break;
        case 1:
          color = Scalar(0, 255, 0);     // hanayo
          break;
        case 2:
          color = Scalar(0, 255, 255);   // rin
          break;
        case 3:
          color = Scalar(0, 0,255);      // maki
          break;
        case 4:
          color = Scalar(0, 127, 255);   //honoka
          break;
        case 5:
          color = Scalar(255, 255, 255); // kotori
          break;
        case 6:
          color = Scalar(255, 0, 0);     // umi
          break;
        case 7:
          color = Scalar(255, 0, 255);   // niko
          break;
        case 8:
          color = Scalar(255, 255, 0);   // eri
          break;
        case 9:
          color = Scalar(255, 0, 127);   // nozomi
          break;
    }
    // drawing rectangle recognized region.
    rectangle(image, Point(faces[i].x, faces[i].y), Point(faces[i].x+faces[i].width, faces[i].y+faces[i].height), color, 5);
  }

  // export recognition result to frame.
  exportVideo << image;
}
