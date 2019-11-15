//
// Created by zkrm on 28/03/19.
//

#ifndef ANGLEHISTOGRAM_ANGLEHISTOGRAM_H
#define ANGLEHISTOGRAM_ANGLEHISTOGRAM_H
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <math.h>
#include <stdio.h>
#include <random>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

class AngleHistogram {

private:
    int max;
    int scaling;
public:

    vector<double> model;
    string evalRelation(vector<double> model, vector<double> histo);

private:

    Mat img1, img2;
    void vectToFile(vector<double> values, string histogramType, string fileName);
    vector<double> histo;
    vector<double> intersection(vector<double> histo, vector<double> model);

public:
    AngleHistogram(Mat img1, Mat img2);
    AngleHistogram();
    int drawLines(int angle, Mat img1original, Mat img2original);
    const Mat &getImg1() const;

    void setHisto(const vector<double> &histo);

    void setImg1(const Mat &img1);

    void setImg2(const Mat &img2);

    void setModel(const vector<double> &model);

    const Mat &getImg2() const;


    void calculHistogramme(int scaling,bool imgRef,int definition);
    void setScaling(const int scaling);

    void normaliser();

    const vector<double> &getHisto() const;

};



#endif //ANGLEHISTOGRAM_ANGLEHISTOGRAM_H
