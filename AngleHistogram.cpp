//
// Created by zkrm on 28/03/19.
//

#include "AngleHistogram.h"



AngleHistogram::AngleHistogram(Mat img1, Mat img2){
    this->img1 = img1;
    this->img2 = img2;
    histo = vector<double>(360,0);

    max = 0;
    model = vector<double>(360,0);
    /*for(int i = 90; i< 270; i++){
        double tetha = i *(CV_PI / 180) ;
        model[i] = cos(tetha)*cos(tetha);
    }*/
    for(int i = 90+45;i<=45+90+45;i++){
        model[i] = (i-90-45)/45.0f;
    }
    for(int i = 45+90+45; i<=90+90+45;i++){
        model[i] = 1-((i-45-90-45)/45.0f);
    }
    //std::rotate(model.begin(),model.begin(),model.end());
}
AngleHistogram::AngleHistogram(){
    histo = vector<double>(360,0);
    max = 0;
    model = vector<double>(360,0);
    /*for(int i = 90; i< 270; i++){
        double tetha = i *(CV_PI / 180) ;
        model[i] = cos(tetha)*cos(tetha);
    }*/

    for(int i = 90+45;i<=45+90+45;i++){
        model[i] = (i-90-45)/45.0f;
    }
    for(int i = 45+90+45; i<=90+90+45;i++){
        model[i] = 1-((i-45-90-45)/45.0f);
    }


}


void AngleHistogram::vectToFile(vector<double> values, string histogramType, string fileName){


    ofstream fichier(fileName);

    for(int i = 0; i < values.size(); i++){
        fichier << values[i] << endl;
    }

    fichier.close();

    //cout << fileName;

}




int AngleHistogram::drawLines(int angle, Mat img1original, Mat img2original){

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
    bool refImg1 = false; //true if image 1 is passsed first, else false
    bool refImg2 = false; //true if image 2 is passsed first, else false
    int value_histogramme = 0; //Value to be returned and graph in Bar chart
    int detected_whitePixels_1 = 0; // Sum of pixels in a single line for img1.
    int detected_whitePixels_2 = 0; // Sum of pixels in a single line for img2.
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

    LineIterator it(img1,P1,P2,8);         //Function in opencv that will draw the Bresenham line.
    vector<Point> buf(it.count);       //Vector containing Points of the Bresenham line.
    vector<int> image1_values;             //Vector to store values the sum of eache line for img1.
    vector<int> image2_values;             //Vector to store values the sum of eache line for img2.

    //Draw the Bresenham line.
    for(int i=0; i<it.count; i++){
        //Image 1
        if(img1.at<uchar>(it.pos()) == 255){
            if(!refImg2){                     //refImg1 set to true if we detect img1 first before img2
                detected_whitePixels_1 += 1;
                refImg1 = true;
            }
        }
        //Image 2
        if(img2.at<uchar>(it.pos()) == 255){
            if(refImg1){                     //+1 to pixel if img1 is detected first
                detected_whitePixels_2 += 1;
            }else if(!refImg1){              //else +0
                refImg2 = true;
            }
        }
        buf[i]=it.pos();
        it++;
    }

    image1_values.push_back(detected_whitePixels_1);  //Store value in vector of img1.
    image2_values.push_back(detected_whitePixels_2);  //Store value in vector of img2.


    if(angle>=45){
        //Draw, using the Bresenham line, by permutating to the left.
        for(int x=1; x<=buf[it.count-1].x; x++){
            detected_whitePixels_1 = 0; //Reset to 0.
            detected_whitePixels_2 = 0; //Reset to 0.
            refImg1 = false; //set to false
            refImg2 = false; //set to false
            for(int y=0; y<buf.size(); y++){
                //If Point is inside the image, color it white, else do nothing.
                if(buf[y].x-x>=0){
                    //Verification for overlapping(Condition: stay inside the image matrix) on img1
                    if(img1.at<uchar>(buf[y].y, buf[y].x-x) == 255){
                        if(!refImg2){
                            detected_whitePixels_1++;
                            refImg1 = true;
                        }
                    }
                    //Verification for overlapping(Condition: stay inside the image matrix) on img2
                    if(img2.at<uchar>(buf[y].y, buf[y].x-x) == 255){
                        if(refImg1){
                            detected_whitePixels_2++;
                        }else if(!refImg1){
                            refImg2 = true;
                        }
                    }
                }
            }
            image1_values.push_back(detected_whitePixels_1); //Store value in vector of img1
            image2_values.push_back(detected_whitePixels_2); //Store value in vector of img2.
        }
        //Draw, using the Bresenham line, by permutating to the right.
        for(int x=1; x<img1.cols; x++){
            detected_whitePixels_1 = 0; //Reset to 0.
            detected_whitePixels_2 = 0; //Reset to 0.
            refImg1 = false; //set to false
            refImg2 = false; //set to false
            for(int y=0; y<buf.size(); y++){
                //If Point is inside the image, color it white, else do nothing.
                if(buf[y].x+x<img1.cols){
                    //Verification for overlapping(Condition: stay inside the image matrix) on img1.
                    if(img1.at<uchar>(buf[y].y, buf[y].x+x) == 255){
                        if(!refImg2){
                            detected_whitePixels_1++;
                            refImg1 = true;
                        }
                    }
                    //Verification for overlapping(Condition: stay inside the image matrix) on img2.
                    if(img2.at<uchar>(buf[y].y, buf[y].x+x) == 255){
                        if(refImg1){
                            detected_whitePixels_2++;
                        }else if(!refImg1){
                            refImg2 = true;
                        }
                    }
                }
            }
            image1_values.push_back(detected_whitePixels_1); //Store value in vector of img1
            image2_values.push_back(detected_whitePixels_2); //Store value in vector of img2.
        }
    }else if(angle<45){
        //Draw, using the Bresenham line, by permutating upwards.
        for(int x=1; x<=buf[it.count-1].x; x++){
            detected_whitePixels_1 = 0; //Reset to 0.
            detected_whitePixels_2 = 0; //Reset to 0.
            refImg1 = false; //set to false
            refImg2 = false; //set to false
            for(int y=0; y<buf.size(); y++){
                //If Point is inside the image, color it white, else do nothing.
                if(buf[y].y-x>=0){
                    //Verification for overlapping(Condition: stay inside the image matrix) on img1.
                    if(img1.at<uchar>(buf[y].y-x, buf[y].x) == 255){
                        if(!refImg2){
                            detected_whitePixels_1++;
                            refImg1 = true;
                        }
                    }
                    //Verification for overlapping(Condition: stay inside the image matrix) on img2.
                    if(img2.at<uchar>(buf[y].y-x, buf[y].x) == 255){
                        if(refImg1){
                            detected_whitePixels_2++;
                        }else if(!refImg1){
                            refImg2 = true;
                        }
                    }
                }
            }
            image1_values.push_back(detected_whitePixels_1); //Store value in vector of img1.
            image2_values.push_back(detected_whitePixels_2); //Store value in vector of img2.
        }
        //Draw, using the Bresenham line, by permutating downwards.
        for(int x=1; x<img1.rows; x++){
            detected_whitePixels_1 = 0; //Reset to 0.
            detected_whitePixels_2 = 0; //Reset to 0.
            refImg1 = false; //set to false
            refImg2 = false; //set to false
            for(int y=0; y<buf.size(); y++){
                //If Point is inside the image, color it white, else do nothing.
                if(buf[y].y+x<img1.rows){
                    //Verification for overlapping(Condition: stay inside the image matrix) on img1.
                    if(img1.at<uchar>(buf[y].y+x, buf[y].x) == 255){
                        if(!refImg2){
                            detected_whitePixels_1++;
                            refImg1 = true;
                        }
                    }
                    //Verification for overlapping(Condition: stay inside the image matrix) on img2.
                    if(img2.at<uchar>(buf[y].y+x, buf[y].x) == 255){
                        if(refImg1){
                            detected_whitePixels_2++;
                        }else if(!refImg1){
                            refImg2 = true;
                        }
                    }
                }
            }
            image1_values.push_back(detected_whitePixels_1); //Store value in vector of img1.
            image2_values.push_back(detected_whitePixels_2); //Store value in vector of img2.
        }
    }

    /*
    //Prints the buffer for the 2 images
    for(int i=0; i<image1_values.size();i++){
        cout << "img1[" << i << "]: " << image1_values[i] << "       img2[" << i << "]: " << image2_values[i] <<endl;
    }*/

    //Total sum of the sum of the prodcuts for each line in a certain angle
    for(int i=0; i<image1_values.size();i++){
        value_histogramme += (image1_values[i] * image2_values[i]);
    }

/*
    imshow("My Window1", img1);
    imshow("My Window2", img2);

    waitKey();
*/
    return value_histogramme;
}
void AngleHistogram::normaliser() {

    double theta;
    for(int i = 0; i< 45; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/sin(theta));
    }
    for(int i = 45; i< 90; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/cos(theta));
    }
    for(int i = 90; i< 135; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/sin(theta));
    }
    for(int i = 135; i< 180; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/cos(theta));
    }
    for(int i = 180; i< 225; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/sin(theta));
    }
    for(int i = 225; i< 270; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/cos(theta));
    }
    for(int i = 270; i< 315; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/sin(theta));
    }
    for(int i = 315; i<= 360; i++){
        theta = i *(CV_PI / 180) ;
        histo[i] = abs(histo[i]/cos(theta));
    }

}




void AngleHistogram::calculHistogramme(int scaling,bool imgRef,int definition) {

    Mat calculImg1;
    Mat calculImg2;
    resize(img1,calculImg1,Size(img1.rows/definition,img1.cols/definition));
    resize(img2,calculImg2,Size(img2.rows/definition,img2.cols/definition));
    max = 0;
    histo = vector<double>(360,0);
    if(imgRef){
        for( int i = 0; i< 360 ; i+=scaling){
            histo[i] = drawLines(i,calculImg1,calculImg2);
            if(max<histo[i])
                max = histo[i];
            cout << histo[i] << "-- ";
        }
    }
    else{
        for( int i = 0; i< 360 ; i+=scaling){
            histo[i] = drawLines(i,calculImg2,calculImg1);
            if(max<histo[i])
                max = histo[i];
            //cout << histo[i] << " ";
        }
    }

    for(int i = 0; i<histo.size();i++){
        histo[i] = histo[i]/max;
        //cout << i <<"=" << histo[i] << " ";
    }

}


vector<double> AngleHistogram::intersection(vector<double> histo, vector<double> model) {
    vector<double> intersection = vector<double>(model.size());
    for(int i = 0; i< 360 ; i++){
        if(histo[i]!=0 && model[i]!=0){
            intersection[i] = std::min(histo[i],model[i]);
        }
    }
    return intersection;

}

string AngleHistogram::evalRelation(vector<double> model, vector<double> histo){

    vector<double> intersection = this->intersection(model,histo);

    double max = 0;
    for(double val: intersection){
        if(max<val)
            max = val;
    }
    if(max==0)
        return "Not at all";
    else if(max>0&&max<0.25)
        return "Very little";
    else if(max>=0&&max<0.40)
        return "A little";
    else if(max>=0.40&&max<0.6)
        return "Moderately";
    else if(max>=0.6&&max<0.8)
        return "A lot";
    else if(max>=0.8 && max<1){
        return "Almost";
    }
    else if(max==1)
        return "Completely";
    return "";

}
const vector<double> &AngleHistogram::getHisto() const {
    return histo;
}

const Mat &AngleHistogram::getImg1() const {
    return img1;
}

const Mat &AngleHistogram::getImg2() const {
    return img2;
}

void AngleHistogram::setHisto(const vector<double> &histo) {
    AngleHistogram::histo = histo;
}

void AngleHistogram::setImg1(const Mat &img1) {
    AngleHistogram::img1 = img1;
}

void AngleHistogram::setImg2(const Mat &img2) {
    AngleHistogram::img2 = img2;
}
void AngleHistogram::setModel(const vector<double> &model){
    AngleHistogram::model = model;
}
