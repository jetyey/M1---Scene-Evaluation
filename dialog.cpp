#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
    movingForm = Mat();
    oui = Mat(1,1,CV_8U);
    o = OverlappingHistogram();

    scaling = 8;
    cosModel=vector<double>(360,0);
    for(int i = 90; i<= 270; i++){
        double tetha = i *(CV_PI / 180) ;
        cosModel[i] = cos(tetha)*cos(tetha);
    }
    triangleModel=vector<double>(360,0);
    for(int i = 90+45;i<=45+90+45;i++){
        triangleModel[i] = (i-90-45)/45.0f;
    }
    for(int i = 45+90+45; i<=90+90+45;i++){
        triangleModel[i] = 1-((i-45-90-45)/45.0f);
    }
    definition = 1;
    polarRepresentation = true;
    imgRef=true;
    modelType = 1;


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setFile1(QString s){
    this->file1 = s;
}
void Dialog::setFile2(QString s){
    this->file2 = s;
}

void Dialog::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Space){

        QChart *newChart;
        QChart *oldChart = ui->widget->chart();
        if (oldChart->chartType() == QChart::ChartTypeCartesian){
            newChart = new QPolarChart();
            polarRepresentation = true;
        }
            else{
                    newChart = new QChart();
                    polarRepresentation = false;
                }


            // Move series and axes from old chart to new one
            const QList<QAbstractSeries *> seriesList = oldChart->series();
            const QList<QAbstractAxis *> axisList = oldChart->axes();
            QList<QPair<qreal, qreal> > axisRanges;

            for (QAbstractAxis *axis : axisList) {
                QValueAxis *valueAxis = static_cast<QValueAxis *>(axis);
                axisRanges.append(QPair<qreal, qreal>(valueAxis->min(), valueAxis->max()));
            }

            for (QAbstractSeries *series : seriesList)
                oldChart->removeSeries(series);

            for (QAbstractAxis *axis : axisList) {
                oldChart->removeAxis(axis);
                newChart->addAxis(axis, axis->alignment());
            }

            for (QAbstractSeries *series : seriesList) {
                newChart->addSeries(series);
                for (QAbstractAxis *axis : axisList)
                    series->attachAxis(axis);
            }

            int count = 0;
            for (QAbstractAxis *axis : axisList) {
                axis->setRange(axisRanges[count].first, axisRanges[count].second);
                count++;
            }

            newChart->setTitle(oldChart->title());
            ui->widget->setChart(newChart);
            delete oldChart;



    }

}

void Dialog::replot(){


       if(modelType==1){
           h.setModel(triangleModel);
       }
       else{
           h.setModel(cosModel);
       }



   QChart *oldChart = ui->widget->chart();
   h.calculHistogramme(scaling,imgRef,definition);


   if(polarRepresentation){
       QPolarChart *representationPolaire = new QPolarChart();
       QValueAxis *angleAxis = new QValueAxis();
       angleAxis->setTickCount(5); // 5 ticks = 0 45 180 270 360
       angleAxis->setLabelFormat("%d "+QLatin1String("° "));
       angleAxis->setShadesVisible(true);
       angleAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
       representationPolaire->addAxis(angleAxis, QPolarChart::PolarOrientationAngular);//
       angleAxis->setRange(0,360);

       QValueAxis *radAxis = new QValueAxis();
       radAxis->setTickCount(5); // 5 ticks = 0.2 0.4 0.6 0.8 1
       radAxis->setLabelFormat("%.2f");
       radAxis->setRange(0,1);
       representationPolaire->addAxis(radAxis, QPolarChart::PolarOrientationRadial);//


       QLineSeries *relDroite = new QLineSeries();
       representationPolaire->addSeries(relDroite);
       vector<double> model = vector<double>(h.model);
       for(int i = 0; i< 360 ; i++){
           *relDroite<<QPointF(i,model[i]);
       }

       relDroite->attachAxis(radAxis);
       relDroite->attachAxis(angleAxis);
       relDroite->setName("Relation à droite");


       QLineSeries *relGauche = new QLineSeries();
       representationPolaire->addSeries(relGauche);
       vector<double> gauche = vector<double>(h.model);
       std::rotate(gauche.begin(),gauche.begin()+180,gauche.end());
       for(int i = 0; i< 360 ; i++){
           *relGauche<<QPointF(i,gauche[i]);
       }

       relGauche->attachAxis(radAxis);
       relGauche->attachAxis(angleAxis);
       relGauche->setName("Relation à gauche");

       QLineSeries *relHaut = new QLineSeries();
       representationPolaire->addSeries(relHaut);
       //histoValues->setName("Valeurs de l'histogramme");
       vector<double> haut = vector<double>(h.model);
       std::rotate(haut.begin(),haut.begin()+90,haut.end());
       for(int i = 0; i< 360 ; i++){
           *relHaut<<QPointF(i,haut[i]);
       }

       relHaut->attachAxis(radAxis);
       relHaut->attachAxis(angleAxis);
       relHaut->setName("Relation au dessus");

       QLineSeries *relBas = new QLineSeries();
       representationPolaire->addSeries(relBas);
       //histoValues->setName("Valeurs de l'histogramme");
       vector<double> bas = vector<double>(h.model);
       std::rotate(bas.begin(),bas.begin()+270,bas.end());
       for(int i = 0; i< 360 ; i++){
           *relBas<<QPointF(i,bas[i]);
       }

       relBas->attachAxis(radAxis);
       relBas->attachAxis(angleAxis);
       relBas->setName("Relation en dessous");


       QLineSeries *overLap = new QLineSeries();
       representationPolaire->addSeries(overLap);
       //histoValues->setName("Valeurs de l'histogramme");
       vector<double> overlap = vector<double>(o.getHisto());

       for(int i = 0; i< 360 ; i++){
           *overLap<<QPointF(i,overlap[i]);
       }

       overLap->attachAxis(radAxis);
       overLap->attachAxis(angleAxis);
       overLap->setName("Overlapping");





       //valeurs de l'histogramme
       QLineSeries *histoValues = new QLineSeries();

       vector<double> histogramme = h.getHisto();

       for(int i = 0; i< histogramme.size() ; i+=scaling){
           *histoValues<<QPointF(double(i),histogramme[i]);
       }

       QPen ligneHisto = histoValues->pen();
       ligneHisto.setWidth(2);
       histoValues->setPen(ligneHisto);

       QLineSeries *l = new QLineSeries();
       *l<<QPointF(0,0);

       QAreaSeries *area = new QAreaSeries(histoValues,l);
       representationPolaire->addSeries(area);
       area->attachAxis(radAxis);
       area->attachAxis(angleAxis);
       area->setColor(QColor(116, 152, 249));
       area->setBorderColor(QColor(116, 152, 249,0));
       area->setName("Force histogram values");


       string strD = h.evalRelation(model,histogramme)+" to the right\n";
       string strG = h.evalRelation(gauche,histogramme)+" to the left\n";
       string strH = h.evalRelation(haut,histogramme)+" above\n";
       string strB = h.evalRelation(bas,histogramme)+" below\n";
       ui->label->setText(QString::fromStdString(strD+strG+strH+strB));

       ui->widget->setRenderHint(QPainter::Antialiasing);
       ui->widget->setChart(representationPolaire);

       representationPolaire->setTitle(oldChart->title());
       ui->widget->setChart(representationPolaire);
   }
   else{

       QChart *representationNormale = new QChart();
       QValueAxis *angleAxis = new QValueAxis();
       angleAxis->setTickCount(5); // 5 ticks = 0 45 180 270 360
       angleAxis->setLabelFormat("%d "+QLatin1String("° "));
       angleAxis->setShadesVisible(true);
       angleAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
       representationNormale->addAxis(angleAxis, Qt::AlignBottom);//
       angleAxis->setRange(0,360);
       QValueAxis *radAxis = new QValueAxis();
       radAxis->setTickCount(5); // 5 ticks = 0.2 0.4 0.6 0.8 1
       radAxis->setLabelFormat("%.2f");
       radAxis->setRange(0,1);
       representationNormale->addAxis(radAxis, Qt::AlignLeft);//



       QLineSeries *relDroite = new QLineSeries();
       representationNormale->addSeries(relDroite);
       vector<double> model = vector<double>(h.model);
       for(int i = 0; i< 360 ; i++){
           *relDroite<<QPointF(i,model[i]);
       }

       relDroite->attachAxis(radAxis);
       relDroite->attachAxis(angleAxis);
       relDroite->setName("Relation à droite");


       QLineSeries *relGauche = new QLineSeries();
       representationNormale->addSeries(relGauche);
       vector<double> gauche = vector<double>(h.model);
       std::rotate(gauche.begin(),gauche.begin()+180,gauche.end());
       for(int i = 0; i< 360 ; i++){
           *relGauche<<QPointF(i,gauche[i]);
       }

       relGauche->attachAxis(radAxis);
       relGauche->attachAxis(angleAxis);
       relGauche->setName("Relation à gauche");

       QLineSeries *relHaut = new QLineSeries();
       representationNormale->addSeries(relHaut);
       //histoValues->setName("Valeurs de l'histogramme");
       vector<double> haut = vector<double>(h.model);
       std::rotate(haut.begin(),haut.begin()+90,haut.end());
       for(int i = 0; i< 360 ; i++){
           *relHaut<<QPointF(i,haut[i]);
       }

       relHaut->attachAxis(radAxis);
       relHaut->attachAxis(angleAxis);
       relHaut->setName("Relation au dessus");

       QLineSeries *relBas = new QLineSeries();
       representationNormale->addSeries(relBas);
       //histoValues->setName("Valeurs de l'histogramme");
       vector<double> bas = vector<double>(h.model);
       std::rotate(bas.begin(),bas.begin()+270,bas.end());
       for(int i = 0; i< 360 ; i++){
           *relBas<<QPointF(i,bas[i]);
       }

       relBas->attachAxis(radAxis);
       relBas->attachAxis(angleAxis);
       relBas->setName("Relation en dessous");


       QLineSeries *overLap = new QLineSeries();
       representationNormale->addSeries(overLap);
       //histoValues->setName("Valeurs de l'histogramme");
       vector<double> overlap = vector<double>(o.getHisto());

       for(int i = 0; i< 360 ; i++){
           *overLap<<QPointF(i,overlap[i]);
       }

       overLap->attachAxis(radAxis);
       overLap->attachAxis(angleAxis);
       overLap->setName("Overlapping");





       //valeurs de l'histogramme
       QLineSeries *histoValues = new QLineSeries();

       vector<double> histogramme = h.getHisto();

       for(int i = 0; i< histogramme.size() ; i+=scaling){
           *histoValues<<QPointF(double(i),histogramme[i]);
       }

       QPen ligneHisto = histoValues->pen();
       ligneHisto.setWidth(2);
       histoValues->setPen(ligneHisto);

       QLineSeries *l = new QLineSeries();
       *l<<QPointF(0,0);

       QAreaSeries *area = new QAreaSeries(histoValues,l);
       representationNormale->addSeries(area);
       area->attachAxis(radAxis);
       area->attachAxis(angleAxis);
       area->setColor(QColor(116, 152, 249));
       area->setBorderColor(QColor(116, 152, 249,0));
       area->setName("Force histogram values");

       string strD = h.evalRelation(model,histogramme)+" to the right\n";
       string strG = h.evalRelation(gauche,histogramme)+" to the left\n";
       string strH = h.evalRelation(haut,histogramme)+" above\n";
       string strB = h.evalRelation(bas,histogramme)+" below\n";
       ui->label->setText(QString::fromStdString(strD+strG+strH+strB));


       ui->widget->setRenderHint(QPainter::Antialiasing);
       ui->widget->setChart(representationNormale);

       representationNormale->setTitle(oldChart->title());
       ui->widget->setChart(representationNormale);


   }




   delete oldChart;

}

void Dialog::plot(){


    Mat img1 = imread(file1.toStdString(),CV_8U);
    Mat img2 = imread(file2.toStdString(),CV_8U);




    threshold(img1, img1, 50, 255, 0);
    threshold(img2, img2, 50, 255, 0);
    movingForm = img2.clone();
    refImg = img1.clone();


    //AngleHistogram h = AngleHistogram(img1,img2);
    //AngleHistogram h = AngleHistogram(img1,img2);
    h.setImg1(img1);
    h.setImg2(img2);
    h.calculHistogramme(1,true,1);

    o.setImg1(img1);
    o.setImg2(img2);
    o.calculHistogramme();

    QPolarChart *representationPolaire = new QPolarChart();

    QLineSeries *histoValues = new QLineSeries();

    vector<double> histogramme = h.getHisto();
    for(double val : histogramme)
        cout << val << " ";
    for(int i = 0; i< 360 ; i++){
        *histoValues<<QPointF(double(i),histogramme[i]);
    }

    QPen ligneHisto = histoValues->pen();
    ligneHisto.setWidth(2);
    histoValues->setPen(ligneHisto);



    QValueAxis *angleAxis = new QValueAxis();
    angleAxis->setTickCount(5); // 5 ticks = 0 45 180 270 360
    angleAxis->setLabelFormat("%d "+QLatin1String("° "));
    angleAxis->setShadesVisible(true);
    angleAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    representationPolaire->addAxis(angleAxis, QPolarChart::PolarOrientationAngular);
    angleAxis->setRange(0,360);

    QValueAxis *radAxis = new QValueAxis();
    radAxis->setTickCount(5); // 5 ticks = 0.2 0.4 0.6 0.8 1
    radAxis->setLabelFormat("%.2f");
    radAxis->setRange(0,1);
    representationPolaire->addAxis(radAxis, QPolarChart::PolarOrientationRadial);



    QLineSeries *l = new QLineSeries();
    *l<<QPointF(0,0);

    QAreaSeries *area = new QAreaSeries(histoValues,l);
    representationPolaire->addSeries(area);
    area->attachAxis(radAxis);
    area->attachAxis(angleAxis);
    area->setColor(QColor(116, 152, 249));
    area->setBorderColor(QColor(116, 152, 249,0));




    QLineSeries *relDroite = new QLineSeries();
    representationPolaire->addSeries(relDroite);
    vector<double> model = vector<double>(h.model);
    for(int i = 0; i< 360 ; i++){
        *relDroite<<QPointF(i,model[i]);
    }

    relDroite->attachAxis(radAxis);
    relDroite->attachAxis(angleAxis);
    relDroite->setName("Relation à droite");


    QLineSeries *relGauche = new QLineSeries();
    representationPolaire->addSeries(relGauche);
    vector<double> gauche = vector<double>(h.model);
    std::rotate(gauche.begin(),gauche.begin()+180,gauche.end());
    for(int i = 0; i< 360 ; i++){
        *relGauche<<QPointF(i,gauche[i]);
    }

    relGauche->attachAxis(radAxis);
    relGauche->attachAxis(angleAxis);
    relGauche->setName("Relation à gauche");

    QLineSeries *relHaut = new QLineSeries();
    representationPolaire->addSeries(relHaut);
    //histoValues->setName("Valeurs de l'histogramme");
    vector<double> haut = vector<double>(h.model);
    std::rotate(haut.begin(),haut.begin()+90,haut.end());
    for(int i = 0; i< 360 ; i++){
        *relHaut<<QPointF(i,haut[i]);
    }

    relHaut->attachAxis(radAxis);
    relHaut->attachAxis(angleAxis);
    relHaut->setName("Relation au dessus");

    QLineSeries *relBas = new QLineSeries();
    representationPolaire->addSeries(relBas);
    //histoValues->setName("Valeurs de l'histogramme");
    vector<double> bas = vector<double>(h.model);
    std::rotate(bas.begin(),bas.begin()+270,bas.end());
    for(int i = 0; i< 360 ; i++){
        *relBas<<QPointF(i,bas[i]);
    }

    relBas->attachAxis(radAxis);
    relBas->attachAxis(angleAxis);
    relBas->setName("Relation en dessous");


    QLineSeries *overLap = new QLineSeries();
    representationPolaire->addSeries(overLap);
    //histoValues->setName("Valeurs de l'histogramme");
    vector<double> overlap = vector<double>(o.getHisto());

    for(int i = 0; i< 360 ; i++){
        *overLap<<QPointF(i,overlap[i]);
    }

    overLap->attachAxis(radAxis);
    overLap->attachAxis(angleAxis);
    overLap->setName("Overlapping");



    area->setName("Valeurs de l'histogramme");

    ui->widget->setRenderHint(QPainter::Antialiasing);
    ui->widget->setChart(representationPolaire);

    string strD = h.evalRelation(model,histogramme)+" to the right\n";
    string strG = h.evalRelation(gauche,histogramme)+" to the left\n";
    string strH = h.evalRelation(haut,histogramme)+" above\n";
    string strB = h.evalRelation(bas,histogramme)+" below\n";




    vector<vector<Point>> contour;
    vector<Vec4i> hi;
    findContours(img2, contour,hi,RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    rect = boundingRect(contour[0]);
    Point center = (rect.br() + rect.tl())*0.5;
    //ui->horizontalScrollBar->setRange(center.x-rect.x,img2.cols-center.x);
    ui->horizontalScrollBar->setRange(-rect.tl().x,img2.cols-rect.br().x);
    ui->horizontalScrollBar->setValue(rect.tl().x+center.x);

    ui->verticalScrollBar->setRange(-rect.tl().y,img2.cols-rect.br().y);
    ui->verticalScrollBar->setValue(rect.tl().y+center.y);

    Mat fusion = img1.clone();
    addWeighted(img1,0.8,movingForm,0.8,0,fusion);
    currentPos = Rect(rect);

    ui->label->setText(QString::fromStdString(strD+strG+strH+strB+"\nOverlapping"));
    cout << rect.size();

    oui = img2(rect);
    //imshow("oui",oui);
    waitKey(0);
    ui->label_2->setPixmap(QPixmap::fromImage(QImage(fusion.data, fusion.cols, fusion.rows, fusion.step, QImage::Format_Indexed8)));
    ui->label_2->setScaledContents(true);




}

Mat Dialog::translateImg(Mat &img, int offsetx, int offsety){
    Mat trans_mat = (Mat_<double>(2,3) << 1, 0, offsetx, 0, 1, offsety);
    warpAffine(img,img,trans_mat,img.size());
    return img;
}

void Dialog::on_horizontalScrollBar_valueChanged(int value)
{
    /*Mat newImg = translateImg(movingForm,value,0);
    Mat fusion = Mat::zeros(newImg.rows,newImg.cols,CV_8U);
    addWeighted(movingForm,0.8,refImg,0.8,0,fusion);
    ui->label_2->setPixmap(QPixmap::fromImage(QImage(fusion.data, fusion.cols, fusion.rows, fusion.step, QImage::Format_Indexed8)));
    ui->label_2->update();*/
}

void Dialog::on_horizontalScrollBar_sliderMoved(int position)
{

    Mat fusion = Mat::zeros(movingForm.rows,movingForm.cols,CV_8U);
    newImg = Mat::zeros(movingForm.rows,movingForm.cols,CV_8U);

    Rect newPos = Rect(rect);
    newPos.x = newPos.x+position;
    newPos.y = currentPos.y;
    currentPos.x = newPos.x;
    //oui.copyTo(fusion(cv::Rect(150,150,oui.cols, oui.rows)));

    oui.copyTo(newImg(newPos));

    addWeighted(newImg,0.8,refImg,0.8,0,fusion);


    ui->label_2->setPixmap(QPixmap::fromImage(QImage(fusion.data, fusion.cols, fusion.rows, fusion.step, QImage::Format_Indexed8)));
    ui->label_2->repaint();
    //cout << position << " ";

}

void Dialog::on_verticalScrollBar_sliderMoved(int position)
{
    Mat fusion = Mat::zeros(movingForm.rows,movingForm.cols,CV_8U);
    newImg = Mat::zeros(movingForm.rows,movingForm.cols,CV_8U);

    Rect newPos = Rect(rect);
    newPos.y = newPos.y+position;
    newPos.x = currentPos.x;
    currentPos.y = newPos.y;
    //oui.copyTo(fusion(cv::Rect(150,150,oui.cols, oui.rows)));

    oui.copyTo(newImg(newPos));

    addWeighted(newImg,0.8,refImg,0.8,0,fusion);

    ui->label_2->setPixmap(QPixmap::fromImage(QImage(fusion.data, fusion.cols, fusion.rows, fusion.step, QImage::Format_Indexed8)));
    ui->label_2->repaint();
}



void Dialog::on_verticalScrollBar_sliderReleased()
{
    h.setImg2(newImg);
    h.calculHistogramme(scaling,imgRef,definition);
    //replot();

}

void Dialog::on_horizontalScrollBar_sliderReleased()
{
    h.setImg2(newImg);
    h.calculHistogramme(scaling,imgRef,definition);
    //o.setImg2(newImg);
    //o.calculHistogramme();
    //replot();
}

void Dialog::on_pushButton_clicked()
{
    if(imgRef){
        imgRef=false;
        ui->label_3->setText("Second object as reference");
    }
    else {
        imgRef=true;
        ui->label_3->setText("First object as reference");
    }

}

void Dialog::on_radioButton_2_clicked()
{
    modelType = 1;
}

void Dialog::on_radioButton_clicked()
{
    modelType = 0;
}

void Dialog::on_horizontalSlider_sliderMoved(int position)
{
    if(position ==4){
        ui->label_5->setText("Number of points: 360");
        scaling = 1;
    }
    else if(position == 3){
        ui->label_5->setText("Number of points: 180");
        scaling = 2;
    }
    else if(position == 2){
        ui->label_5->setText("Number of points: 90");
        scaling = 4;
    }
    else if(position == 1){
        ui->label_5->setText("Number of points: 45");
        scaling = 8;
    }
}

void Dialog::on_horizontalSlider_2_sliderMoved(int position)
{
    definition = position;

    ui->label_6->setText("Downscale image: /"+QString::fromStdString(std::to_string(position)));

}

void Dialog::on_pushButton_2_released()
{
    replot();
}
