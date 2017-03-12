//
// matrix-image.cpp
// C++ source file
// Version : 1.0
// Project : opencv/matrix-image
//
// Created by Teppei Kobayashi
// Last Modified 2016/10/27
//


/*========================================================================*/
/*----------
  library declaration
  ----------*/
// standard C++ libraries
#include <iostream>
#include <time.h>
// OpenCV libraries
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

/*----------
  macro declaration
  ----------*/
#define CANVAS_SIZE_X 1920
#define CANVAS_SIZE_Y 1080
#define FONT_INTERVAL_X 15
#define FONT_INTERVAL_Y 15
#define TAIL_MAG 0.9

/*----------
  namespace declaration
  ----------*/
using namespace std;
using namespace cv;

/*----------
  prototype declaration
  ----------*/
void generateHSVCanvas(Mat srcImage);
void generateBackgroundCanvas();
void saturationConvolution();
void generateTextCanvas();

/*----------
  variable declaration
  ----------*/
Mat srcImageCanvas = Mat::zeros(CANVAS_SIZE_Y, CANVAS_SIZE_X, CV_8UC1);
Mat backgroundCanvas = Mat::zeros(CANVAS_SIZE_Y, CANVAS_SIZE_X, CV_8UC1);
Mat saturationCanvas = Mat::zeros(CANVAS_SIZE_Y, CANVAS_SIZE_X, CV_8UC1);
Mat textCanvas = Mat::zeros(CANVAS_SIZE_Y, CANVAS_SIZE_X, CV_8UC3);

int workingX = CANVAS_SIZE_X / FONT_INTERVAL_X;
int workingY = CANVAS_SIZE_Y / FONT_INTERVAL_Y + 1;
Mat bgWorkspace = Mat::zeros(workingY, workingX, CV_8UC1);



/*========================================================================*/
/*
  main function
*/
int main(int argc, char *argv[])
{
  // read image and scaling.
  Mat srcImage = imread(argv[1]);
  stringstream fileName;

  // error.
  if(argc != 2){
    cout << "[error] invalid syntax." << endl;
    return -1;
  } else if(srcImage.data == NULL){
    cout << "[error] bad argument." << endl;
    return -1;
  }

  // generate HSV canvas from source image.
  generateHSVCanvas(srcImage);

  // generate background canvas
  for(int i=0; i<50; i++){
    generateBackgroundCanvas();
  }

  // generate saturation logical operated image.
  saturationConvolution();

  // generate text canvas.
  generateTextCanvas();

  // show generated image.
  namedWindow("temp", CV_WINDOW_AUTOSIZE);

  // export detected result: set file name
  fileName.str("");
  fileName << argv[1] << "_matrix.png";
  imwrite(fileName.str(), textCanvas);

  return 0;
}



/*========================================================================*/
/*
  generateHSVCanvas
*/
void generateHSVCanvas(Mat srcImage)
{
  // generate image canvas.
  Mat hsvImage;
  vector<Mat> hsvSplit;

  // color space convert.
  cvtColor(srcImage, hsvImage, COLOR_RGB2HSV);
  split(hsvImage, hsvSplit);
  normalize(hsvSplit[1], hsvSplit[1], 0, 255, NORM_MINMAX, hsvSplit[1].type());

  // draw saturation image to saturationCanvas.
  resize(hsvSplit[1], srcImageCanvas, srcImageCanvas.size(), INTER_CUBIC);

  return;
}



/*========================================================================*/
/*
  generateBackgroundCanvas
*/
void generateBackgroundCanvas()
{
  // variables.
  int pixelIndex;
  Mat tempWorkspace = Mat::zeros(workingY, workingX, CV_8UC1);

  // generate background - 1. generate brighting point.
  for(int x=0; x<bgWorkspace.cols; x++){
    pixelIndex = bgWorkspace.step + x*bgWorkspace.elemSize();
    if (rand()%32 == 0){
      bgWorkspace.data[pixelIndex] = 255;
    }
  }

  // generate background - 2. move all pixels.
  Mat moveKernel = (Mat_<int>(3,3) << 0, 1, 0, 0, 0, 0, 0, 0, 0);
  filter2D(bgWorkspace, bgWorkspace, -1, moveKernel, Point(-1, -1), 0, BORDER_CONSTANT);

  // generate background - 3. draw brighting point tails.
  Mat drawKernel = (Mat_<float>(3,3) << 0, 0, 0, 0, 0, 0, 0, TAIL_MAG, 0);
  tempWorkspace = bgWorkspace.clone();
  for (int i=0; i<20; i++){
    filter2D(tempWorkspace, tempWorkspace, -1, drawKernel, Point(-1, -1), 0, BORDER_CONSTANT);
    tempWorkspace = tempWorkspace + bgWorkspace;
  }

  // generate background - 4. Copy workspace.
  Mat roi = tempWorkspace.rowRange(1, workingY);

  // generate background - 5. scale image.
  Mat bgGenerated = Mat::zeros(CANVAS_SIZE_Y, CANVAS_SIZE_X, CV_8UC1);
  resize(roi, bgGenerated, bgGenerated.size(), INTER_CUBIC);

  // generate background - 6. dilation and erosion image.
  dilate(bgGenerated, bgGenerated, Mat(), Point(-1, -1), 1);
  erode(bgGenerated, bgGenerated, Mat(), Point(-1, -1), 3);

  // generate background - complete. clone image.
  backgroundCanvas = bgGenerated.clone();

  return;
}



/*========================================================================*/
/*
  saturationConvolution
*/
void saturationConvolution()
{
  // variables.
  int pixelIndex = 0;

  // generate text canvas.
  for (int x=0; x<CANVAS_SIZE_X; x++){
    for (int y=0; y<CANVAS_SIZE_Y; y++){
      // calc. coordinate pointer.
      pixelIndex = y*saturationCanvas.step + x*saturationCanvas.elemSize();

      // compare src image saturation and background saturation.
      if(srcImageCanvas.data[pixelIndex] < backgroundCanvas.data[pixelIndex]){
        saturationCanvas.data[pixelIndex] = backgroundCanvas.data[pixelIndex];
      } else if(srcImageCanvas.data[pixelIndex] > backgroundCanvas.data[pixelIndex]){
        saturationCanvas.data[pixelIndex] = srcImageCanvas.data[pixelIndex];
      } else{
        saturationCanvas.data[pixelIndex] = backgroundCanvas.data[pixelIndex];
      }
    }
  }

  return;
}


/*========================================================================*/
/*
  generateTextCanvas
*/
void generateTextCanvas()
{
  // variables.
  int pixelIndex = 0;

  // initialize rand.
  srand((unsigned int)time(NULL));

  for (int x=0; x<=CANVAS_SIZE_X; x+=FONT_INTERVAL_X){
    for (int y=0; y<=CANVAS_SIZE_Y; y+=FONT_INTERVAL_Y){
      // get saturation value.
      pixelIndex = y*saturationCanvas.step + x*saturationCanvas.elemSize();
      int saturationValue = saturationCanvas.data[pixelIndex];


      // generate random character.
      char randChar;
      if (rand()%2 == 0){
        randChar = 'A' + rand()%24;
      } else {
        randChar = 'a' + rand()%24;
      }
      string printText{randChar};

      // print characters.
      putText(textCanvas, printText, Point(x,y), CV_FONT_NORMAL, 0.5, Scalar(0,saturationValue,0), rand()%2+1, 1);
    }
  }

  // apply gaussian blur.
  GaussianBlur(textCanvas, textCanvas, Size(3,3), 0);

  return;
}
