//
// feature-vector.cpp
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

  // variables.
  stringstream fileName,progress;
  int pixelIndex;
  float train[64];
  int imageNum = 0;
  int totalNum = atoi(argv[1]);

  // argument check.
  if(argc != 2){
    cout << "[error] bad argument. usage: ./feature-vector <num. of image>" << endl;
    return -1;
  }

  ofstream ofs;
  ofs.open("feature-vector.csv");

  while(1){
    // import images.
    fileName.str("");
    fileName << "sample/image" << setw(3) << setfill('0') << imageNum << ".png";

    Mat src;
    src = imread(fileName.str());

    // if failed to open image file.
    if(src.empty() && imageNum < totalNum){
      cout << "[error] image not found. " << endl;
      return -1;
    }
    else if(imageNum - 1 == totalNum){
      cout << "[processing] : 100% "<< "(" << totalNum <<  "/"  << totalNum << ")" << endl;
      cout << "[notification] all image processed." << endl;
      break;
    }
    else{
      int processPer = (double)imageNum/(double)totalNum*100;
      cout << "[processing] : " << setw(3) << setfill(' ') << processPer << "% "
           << "(" << imageNum <<  "/"  << totalNum << ")" << "\r" << flush;
    }

    // reset variable.
    for(int i=0; i<64; i++){
      train[i] = 0;
    }

    // normalize imported image
    Mat norm (src.size(), src.type());
    Mat sample(src.size(), src.type());
    normalize(src, norm, 0, 255, NORM_MINMAX, CV_8UC3);

    // generate feature vector.
    for(int y=0; y<sample.rows; y++){
      for(int x=0; x<sample.cols; x++){
        // calc. pixel coordinate
        pixelIndex = y*sample.step + x*sample.elemSize();

        // calc. color vector.
        // color: 0xBBRRGG
        int color = (norm.data[pixelIndex+0]/64)
                  + (norm.data[pixelIndex+1]/64) * 4
                  + (norm.data[pixelIndex+2]/64) * 16;
        train[color] += 1;
      }
    }

    // calc. histgram.
    int pixel = src.rows * src.cols;

    // export calc. result to file.
    for(int i=0; i<64; i++){
      train[i] /= pixel;
      ofs << train[i] << " ";
    }
    ofs << endl;

    imageNum++;
  }

  ofs.close();
  return 0;
}
