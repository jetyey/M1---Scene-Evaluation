//
// Created by zkrm on 07/04/19.
//

#ifndef ANGLEHISTOGRAM_OVERLAPPINGHISTOGRAM_H
#define ANGLEHISTOGRAM_OVERLAPPINGHISTOGRAM_H
#include <vector>
#include <string>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

class OverlappingHistogram {
private:
    int picAngle;
    int max;
public:
    int getPicAngle() const;

    vector<double> model;
    string evalRelation(vector<double> model, vector<double> histo);

private:

    Mat img1, img2;
    void vectToFile(vector<double> values, string histogramType, string fileName);
    vector<double> histo;
    vector<double> intersection(vector<double> histo, vector<double> model);

public:
    OverlappingHistogram(Mat img1, Mat img2);
    OverlappingHistogram();
    int drawLines(int angle, Mat img1original, Mat img2original);
    const Mat &getImg1() const;

    void setHisto(const vector<double> &histo);

    void setImg1(const Mat &img1);

    void setImg2(const Mat &img2);

    const Mat &getImg2() const;


    void calculHistogramme();


    const vector<double> &getHisto() const;
};


#endif //ANGLEHISTOGRAM_OVERLAPPINGHISTOGRAM_H
