//
// learning-data.cpp
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
//

/*========================================================================*/

/*----------
  library declaration
  ----------*/
// standard libraries
#include <stdio.h>
#include <fstream>
#include <iomanip>

// OpenCV libraries
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/ml/ml.hpp"

/*----------
  namespace declaration
  ----------*/
using namespace std;
using namespace cv;

/*----------
  macro declaration
  ----------*/
#define TRAINING_SAMPLES 262    // number of sampled data (image000.png - image261.png)
#define ATTRIBUTES 64           // number of elements in feature vector (64-colors)
#define TEST_SAMPLES 262        // number of test data (image000.png - image 261.png)
#define CLASSES 10              // number of labels (9 peoples and others)

/*----------
  prototype declaration
  ----------*/
void read_dataset(char *filename, Mat &data, Mat &classes,  int total_samples);


/*========================================================================*/

/*
  main function
*/
int main(int argc, char** argv){

  // variables.
  Mat training_set(TRAINING_SAMPLES,ATTRIBUTES,CV_32F);                 // matrix: import training data
  Mat training_set_classifications(TRAINING_SAMPLES, CLASSES, CV_32F);  // matrix: import training data labels

  Mat test_set(TEST_SAMPLES,ATTRIBUTES,CV_32F);                         // matrix: import test data
  Mat test_set_classifications(TEST_SAMPLES,CLASSES,CV_32F);            // matrix: import test labels

  Mat classificationResult(1, CLASSES, CV_32F);                         // matrix: classification result.

  /*----------
    preparation of machine learning.
    ----------*/
  // load training data and test data
  read_dataset(argv[1], training_set, training_set_classifications, TRAINING_SAMPLES);
  read_dataset(argv[2], test_set, test_set_classifications, TEST_SAMPLES);

  // definition of neural network
  Mat layers(3,1,CV_32S);              // neural network has 3 layers (input, middle, output)
  layers.at<int>(0,0) = ATTRIBUTES;    // number of input layer (number of elements in feature vector: 64-colors)
  layers.at<int>(1,0) = 16;            // number of unit in middle layer
  layers.at<int>(2,0) = CLASSES;       // number of output layer (number of labels: 10)

  // generate neural network
  CvANN_MLP nnetwork(layers, CvANN_MLP::SIGMOID_SYM,0.6,1);

  // training parameter setting.
  // when quit learning : learning 1000 times or under minimum error.
  // lerning method: error back propagation method
  CvANN_MLP_TrainParams params(cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, 0.000001),
                               CvANN_MLP_TrainParams::BACKPROP, 0.1, 0.1);


  /*----------
    execute machine learning.
    ----------*/
  // machine learning
  cout << "[message] using training dataset" << endl;
  int iterations = nnetwork.train(training_set, training_set_classifications, Mat(), Mat(), params);
  cout << "[message] training iterations: " <<  iterations << endl;;

  if(!strcmp(argv[1], argv[2])){
    cout << "[warning] using same data in evaluation and training process." << endl;
    cout << "training   :" << argv[1] << endl;
    cout << "evaluation :" << argv[2] << endl;
  }

  // save learning result to csv file.
  CvFileStorage* storage = cvOpenFileStorage("learning-result.xml", 0, CV_STORAGE_WRITE );
  nnetwork.write(storage,"DigitOCR");
  cvReleaseFileStorage(&storage);


  /*----------
    check result of machine learning.
    ----------*/
  // check learning result with test data.
  Mat test_sample;
  int correct_class = 0;
  int wrong_class = 0;

  // variable: classification result.
  int classification_matrix[CLASSES][CLASSES]={{}};

  for (int tsample = 0; tsample < TEST_SAMPLES; tsample++) {
    test_sample = test_set.row(tsample);
    nnetwork.predict(test_sample, classificationResult);

    // classify to class of maximum weight.
    int maxIndex = 0;
    float value = 0.0f;
    float maxValue = classificationResult.at<float>(0,0);

    for(int index=1; index < CLASSES; index++){
      value = classificationResult.at<float>(0,index);
      if(value > maxValue){
        maxValue = value;
        maxIndex = index;
      }
    }

    // compare between detect result and correct.
    if(test_set_classifications.at<float>(tsample, maxIndex) != 1.0f){
      cout << tsample << endl;
      wrong_class++;
      //find the actual label 'class_index'
      for(int class_index=0; class_index < CLASSES; class_index++) {
        if(test_set_classifications.at<float>(tsample, class_index) == 1.0f){
          classification_matrix[class_index][maxIndex]++;// A class_index sample was wrongly classified as maxindex.
          break;
        }
      }
    }
    else {
      correct_class++;
      classification_matrix[maxIndex][maxIndex]++;
    }
  }

  cout << endl << "<< Result on the testing dataset >>" << endl;
  cout << "\tCorrect classification:" << setw(4) << correct_class << " /" << setw(4) << (double) correct_class*100/TEST_SAMPLES << "%" << endl;
  cout << "\tWorng classification  :" << setw(4) << wrong_class << " /" << setw(4) << (double) wrong_class*100/TEST_SAMPLES << "%" << endl;

  // output test result.
  cout << "   ";
  for (int i=0; i < CLASSES; i++){
    cout << i << "\t";
  }
  cout << endl;
  for(int row=0; row<CLASSES; row++){
    cout << row << "  ";
    for(int col=0; col<CLASSES; col++){
      cout << classification_matrix[row][col] << "\t";
    }
    cout << endl;
  }

  return 0;
}


/*
  import feature vector data from csv file.
*/
// rows   : the feature vector of image.
// column : 0 - 63: vector elements, 64: label
//
// label
// hanayo:1 / rin:2    / maki:3
// honoka:4 / kotori:5 / umi:6
// niko:7   / eri:8    / nozomi:9
// others:0
void read_dataset(char *filename, Mat &data, Mat &classes,  int total_samples)
{
  int label;
  float pixelvalue;

  // import file read-only mode.
  FILE* inputfile = fopen(filename, "r");

  for(int row = 0; row < total_samples; row++){
    for(int col = 0; col <= ATTRIBUTES; col++){
      // import the feature vector elements.
      if (col < ATTRIBUTES){
        fscanf(inputfile, "%f,", &pixelvalue);
        data.at<float>(row,col) = pixelvalue;
      }
      // import the feature vector label.
      else if (col == ATTRIBUTES){
        fscanf(inputfile, "%i", &label);
        classes.at<float>(row,label) = 1.0;
      }
    }
  }
  fclose(inputfile);
}
