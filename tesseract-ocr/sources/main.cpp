//
// main.cpp
// C++ source file
// Version 1.0
//
// Created by Teppei Kobayashi
// Last Modified 2016/11/25
//


/*----------
  Bibliography
  ----------*/
// Scene Text Recognition : docs.opencv.org/3.0-beta/modules/text/doc/ocr.html


/*========================================================================*/

/*----------
  library declaration
  ----------*/
// standard C++ libraries
#include <iostream>
#include <iomanip>
#include <vector>
// OpenCV libraries
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/text.hpp"

/*----------
  namespace declaration
  ----------*/
using namespace std;

/*----------
  global variables declaration
  ----------*/
cv::Rect region;
bool drawing_box = false;
bool region_specified =false;

/*----------
  prototype declaration
  ----------*/
void drawBox(cv::Mat* img, cv::Rect rect);
void mouseCallback(int event, int x, int y, int flags, void* param);


/*========================================================================*/

/*
  main function
*/
int main(int argc, char *argv[]){

  // declaration variables.
  string originalName = "read image";
  string regionName   = "specified region";
  region = cv::Rect(-1, -1, 0, 0);

  // Argument check. If specified wrong argument, application will exit.
  if (argc != 2){
    cout << "[Error] Bad argument." << endl;
    cout << "[Usage] <command> <image file>" << endl;
    return -1;
  }

  // Open spicified image file.
  cv::Mat image = cv::imread(argv[1]);
  if(image.data == NULL){
    cout << "[Error] Failed to open image. Didn't you specified except image file?" << endl;
    return -1;
  }
  cv::namedWindow(originalName, CV_WINDOW_AUTOSIZE);

  // Create temporary image.
  cv::Mat temp = image.clone();
  cv::setMouseCallback(originalName, mouseCallback, (void *)&image);

  // Overlay specifing region.
  while (1){
    image.copyTo(temp);

    if(drawing_box){
      drawBox(&temp, region);
    }

    // Display draw campus.
    cv::imshow(originalName, temp);
    cv::waitKey(10);

    // When drawed region on campus, go to next state.
    if(region_specified == true)
      break;
  }

  // Show specified region of image.
  cv::Mat roi = image(cv::Rect(region.x, region.y, region.width, region.height));
  cv::namedWindow(regionName, CV_WINDOW_AUTOSIZE);
  cv::imshow(regionName, roi);

  // Create gray scale image that using OCRTesseract.
  cv::Mat gray;
  cv::cvtColor(roi, gray, cv::COLOR_RGB2GRAY);

  // Initialize OCR class instance.
  // [Usage] cv::text::OCRTesseract::create(<the name of directory the parent directory of tessdata>, <language>);
  cv::Ptr<cv::text::OCRTesseract> ocr = cv::text::OCRTesseract::create("/usr/share/tesseract-ocr/tessdata", "eng");


  // Declaration variables that using store OCR result.
  string text;
  vector<cv::Rect> boxes;
  vector<string> words;
  vector<float> confidences;

  // Recognize text using the tesseract-OCR Api.
  // [Usage] run(<input image>, <output text>, <text elements coordinate>, <words of text lines>, <OCR reliability>)
  ocr->run(gray, text, &boxes, &words, &confidences);

  // Output OCR result.
  cout << text.c_str() << endl;
  cout << "   Figure   |    Place    |     Size    | Reliability " << endl;
  cout << "------------+-------------+------- -----+-------------" << endl;
  for (unsigned int i = 0; i < boxes.size(); i++)
    {
      cout.setf(ios::left, ios::adjustfield);
      cout << setw(12) << words[i].c_str() ;
      cout.setf(ios::right, ios::adjustfield);
      cout << "| ("
           << setw(4) << boxes[i].x << "," << setw(4) << boxes[i].y << ") | ("
           << setw(4) << boxes[i].width << "," << setw(4) << boxes[i].height << ") | "
           << setw(9) <<confidences[i] << endl;
    }

  // Exit if pressed Esc.
  while (1){
    if (cv::waitKey(0) == 27){
      cv::destroyAllWindows();
      return 0;
    }
  }
}



/*
  draw specifing region function
*/
void drawBox(cv::Mat* img, cv::Rect rect){
  cv::rectangle(*img, cv::Point2d(region.x, region.y), cv::Point2d(region.x + region.width, region.y + region.height), cv::Scalar(0x00, 0xff, 0x00));
}



/*
  mouse callback function
*/
void mouseCallback(int event, int x, int y, int flags, void* param){

  // Declaration variable.
  cv::Mat* image = static_cast<cv::Mat*>(param);

  // Get mouse event.
  switch (event){

  // Get mouse coordinate and mouse moving.
  case cv::EVENT_MOUSEMOVE:
    if(drawing_box){
      region.width  = x - region.x;
      region.height = y - region.y;
    }
    break;

  // Get mouse button state.
  // When left button pushed.
  case cv::EVENT_LBUTTONDOWN:
    drawing_box = true;
    region = cv::Rect(x, y, 0, 0);
    break;

  // When left button released.
  case cv::EVENT_LBUTTONUP:
    drawing_box = false;
    if (region.width < 0){
      region.x += region.width;
      region.width *= -1;
    }
    if (region.height < 0){
      region.y += region.height;
      region.height *= -1;
    }
    drawBox(image, region);
    region_specified = true;
    break;
  }
}
