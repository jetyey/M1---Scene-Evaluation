#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include "AngleHistogram.h"
#include "OverlappingHistogram.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    void plot();
    ~Dialog();
    void setFile1(QString s);
    void setFile2(QString s);
    Mat translateImg(Mat &img, int offsetx, int offsety);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_horizontalScrollBar_valueChanged(int value);


    void on_horizontalScrollBar_sliderMoved(int position);

    void on_verticalScrollBar_sliderMoved(int position);

    void on_verticalScrollBar_sliderReleased();

    void on_horizontalScrollBar_sliderReleased();

    void on_pushButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_pushButton_2_released();

private:


    Ui::Dialog *ui;
    QString file1;
    QString file2;
    Mat movingForm, refImg, oui, newImg;
    Rect rect,currentPos;
    AngleHistogram h;
    OverlappingHistogram o;
    int scaling;
    int definition;
    bool polarRepresentation;
    bool imgRef;
    int modelType;
    vector<double> cosModel,triangleModel;
    void replot();



};

#endif // DIALOG_H
