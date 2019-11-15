//
// Created by zkrm on 07/04/19.
//

#include "OverlappingHistogram.h"

OverlappingHistogram::OverlappingHistogram(Mat img1, Mat img2){
    this->img1 = img1;
    this->img2 = img2;
    histo = vector<double>(360,0);
    picAngle = -1;
    max = 0;
    model = vector<double>(360,0);
    for(int i = 90; i< 270; i++){
        double tetha = i *(CV_PI / 180) ;
        model[i] = cos(tetha)*cos(tetha);
    }
    //std::rotate(model.begin(),model.begin(),model.end());
}
OverlappingHistogram::OverlappingHistogram(){
    histo = vector<double>(360,0);
    picAngle = -1;
    max = 0;
    model = vector<double>(360,0);
    for(int i = 90; i< 270; i++){
        double tetha = i *(CV_PI / 180) ;
        model[i] = cos(tetha)*cos(tetha);
    }
}


void OverlappingHistogram::vectToFile(vector<double> values, string histogramType, string fileName){


    ofstream fichier(fileName);

    for(int i = 0; i < values.size(); i++){
        fichier << values[i] << endl;
    }

    fichier.close();

    //cout << fileName;

}




int OverlappingHistogram::drawLines(int angle, Mat img1original, Mat img2original){

    Mat img1 = img1original.clone();
       Mat img2 = img2original.clone();

       //Thresholding the 2 pictures.
       //threshold(img1, img1, 50, 255, 0);
       //threshold(img2, img2, 50, 255, 0);

       //Resizing if images are not square.
       if(img1.rows<img1.cols){
           resize(img1, img1, Size(img1.rows,img1.rows));
       }else if(img1.rows>img1.cols){
           resize(img1, img1, Size(img1.cols,img1.cols));
       }
       if(img2.rows<img2.cols){
           resize(img2, img2, Size(img2.rows,img2.rows));
       }else if(img2.rows>img2.cols){
           resize(img2, img2, Size(img2.cols,img2.cols));
       }

       //Variables

       int value_histogramme = 0; //Value to be returned and graph in Bar chart
       Point P1(0,0);    //Point 1 of img1. Src
       Point P2;         //Point 2 of img1. Destination

       //Set angle between 0 - 90 and flip images accordingly
       while(angle>90 || angle<0){
           if(angle>90 && angle<=180){
               flip(img1, img1, 1);
               flip(img2, img2, 1);
               angle = 180 - angle;
           }else if(angle>180 && angle<=270){
               flip(img1, img1, 0);
               flip(img1, img1, 1);
               flip(img2, img2, 0);
               flip(img2, img2, 1);
               angle = angle - 180;
           }else if(angle>270 && angle<=360){
               flip(img1, img1, 0);
               flip(img2, img2, 0);
               angle = 360 - angle;
           }else if(angle>360){
               angle = 360-angle;
           }else if(angle<0){
               angle = angle+360;
           }
       }

       //Establish Point of source(Here always 0,0) and point of destination
       if(angle>45 && angle<=90){
           P2.x =  (int)round(img1.rows * tan((90-angle) * CV_PI / 180.0));
           P2.y =  img1.rows;
       }else if(angle<=45 && angle>=0){
           P2.x = img1.cols;
           P2.y = (int)round(img1.cols * tan(angle * CV_PI / 180.0));
       }

       LineIterator it(img1,P1,P2,8);         //Function in opencv that will draw the line.
       vector<Point> buf(it.count);       //Vector containing Points of the line.
       vector<int> image1_values;             //Vector to store values the sum of eache line for img1.
       vector<int> image2_values;             //Vector to store values the sum of eache line for img2.

       //Draw the Bresenham line.
       for(int i=0; i<it.count; i++){
           if(img1.at<uchar>(it.pos()) == 255 && img2.at<uchar>(it.pos())==255)
               value_histogramme++;
           buf[i]=it.pos();
           it++;
       }


       bool obj1 = false;
       bool obj2 = false;

       if(angle>=45){
           //Draw, using the Bresenham line, by permutating to the left.
           for(int x=1; x<=buf[it.count-1].x; x++){
               for(int y=0; y<buf.size(); y++){
                   if(buf[y].x-x>=0){

                       if(img1.at<uchar>(buf[y].y, buf[y].x-x) == 255)
                           obj1 = true;

                       if((img2.at<uchar>(buf[y].y, buf[y].x-x) == 255)&&!obj1)
                           obj2 = true;

                       if(obj1&&!obj2){
                           if(img1.at<uchar>(buf[y].y, buf[y].x-x) == 255 && img2.at<uchar>(buf[y].y, buf[y].x-x) == 255){
                               value_histogramme++;
                           }
                       }

                   }
               }

               obj1 = false;
               obj2 = false;

           }
           //Draw, using the Bresenham line, by permutating to the right.
           for(int x=1; x<img1.cols; x++){
               for(int y=0; y<buf.size(); y++){

                   if(buf[y].x+x<img1.cols){
                       if(img1.at<uchar>(buf[y].y, buf[y].x+x) == 255)
                           obj1 = true;

                       if((img2.at<uchar>(buf[y].y, buf[y].x+x) == 255)&&!obj1)
                           obj2 = true;
                       //Verification for overlapping(Condition: stay inside the image matrix) on img1.
                       if(obj1&&!obj2){
                           if(img1.at<uchar>(buf[y].y, buf[y].x+x) == 255 && img2.at<uchar>(buf[y].y, buf[y].x+x) == 255){
                               value_histogramme++;
                           }
                       }
                   }
               }
               obj1 = false;
               obj2 = false;

           }
       }
       else if(angle<45){
           //Draw, using the Bresenham line, by permutating upwards.
           for(int x=1; x<=buf[it.count-1].x; x++){

               for(int y=0; y<buf.size(); y++){

                   if(buf[y].y-x>=0){

                       if(img1.at<uchar>(buf[y].y-x, buf[y].x) == 255)
                           obj1 = true;

                       if(img2.at<uchar>(buf[y].y-x, buf[y].x) == 255)
                           obj2 = true;


                       //Verification for overlapping(Condition: stay inside the image matrix) on img1.
                       if(obj1&&!obj2){
                           if(img1.at<uchar>(buf[y].y-x, buf[y].x) == 255 && img2.at<uchar>(buf[y].y-x, buf[y].x) == 255){
                               value_histogramme++;
                           }
                       }

                   }
               }
               obj1 = false;
               obj2 = false;
           }
           //Draw, using the Bresenham line, by permutating downwards.
           for(int x=1; x<img1.rows; x++){

               for(int y=0; y<buf.size(); y++){
                   //If Point is inside the image, color it white, else do nothing.
                   if(buf[y].y+x<img1.rows){

                       if(img1.at<uchar>(buf[y].y+x, buf[y].x) == 255)
                           obj1 = true;

                       if(img2.at<uchar>(buf[y].y+x, buf[y].x) == 255)
                           obj2 = true;


                       //Verification for overlapping(Condition: stay inside the image matrix) on img1.
                       if(obj1&&!obj2){
                           if(img1.at<uchar>(buf[y].y+x, buf[y].x) == 255 && img2.at<uchar>(buf[y].y+x, buf[y].x) == 255){
                               value_histogramme++;
                           }
                       }

                   }
               }
               obj1 = false;
               obj2 = false;
           }
       }

       return (value_histogramme*value_histogramme)/2;

}



void OverlappingHistogram::calculHistogramme() {

    for( int i = 0; i< 360 ; i++){
        histo[i] = drawLines(i,img1,img2);
        if(max<histo[i])
            max = histo[i];
        //cout << histo[i] << " ";
    }
    for(int i = 0; i<360;i++){
        histo[i] = drawLines(i,img2,img1);
        if(max<histo[i])
            max = histo[i];
    }
    picAngle = std::distance(histo.begin(), std::max_element(histo.begin(), histo.end()));
    for(int i = 0; i<histo.size();i++){
        histo[i] = histo[i]/max;
        //cout << i <<"=" << histo[i] << " ";
    }

}



const vector<double> &OverlappingHistogram::getHisto() const {
    return histo;
}

const Mat &OverlappingHistogram::getImg1() const {
    return img1;
}

const Mat &OverlappingHistogram::getImg2() const {
    return img2;
}

void OverlappingHistogram::setHisto(const vector<double> &histo) {
    OverlappingHistogram::histo = histo;
}

void OverlappingHistogram::setImg1(const Mat &img1) {
    OverlappingHistogram::img1 = img1;
}

void OverlappingHistogram::setImg2(const Mat &img2) {
    OverlappingHistogram::img2 = img2;
}

int OverlappingHistogram::getPicAngle() const {
    return picAngle;
}
