#include "drawwidget.h"
#include <QPainter>
#include <QImage>

#include <chrono>
#include <ctime>

#include <omp.h>

DrawWidget::DrawWidget(QWidget *parent) : QWidget(parent)
{
    int w = width(), h = height();
    mainScene = new Scene();

    mainWorld = World(objects);
    changeOffset(0, 0, -3, 0, 0, 0);
}

void DrawWidget::recalc(double t, QTextBrowser& TB, double dx, double dy, double dz, double dalpha, double dbeta, double dgamma){
    mainWorld.interact();
    mainWorld.setDescription(TB, t);
    mainWorld.pushToScene(objects);
    changeOffset(dx, dy, dz, dalpha, dbeta, dgamma);

    int w = width(), h = height();
    mainScene->setSizes(w, h);
}

void DrawWidget::restart(){
    mainScene = new Scene();
    mainWorld = World(objects);
    changeOffset(0, 0,-3, 0, 0, 0);
}

void DrawWidget::paintEvent(QPaintEvent *event){
    QPainter p(this);

    int w = width(), h = height();
    mainScene->setSizes(w, h);
    mainScene->setObjects(objects);

    QImage img(w, h, QImage::Format_RGB32);

    #pragma omp parallel for
    for(int x = 0; x < w; x++){
        #pragma omp parallel for
        for(int y = 0; y < h; y++){
            img.setPixelColor(x, y, mainScene->getPixel(x,y).toQColor());
        }
    }

    p.drawImage(0,0,img);
}
