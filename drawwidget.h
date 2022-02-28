#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include "Graphics/tracer.h"
#include "Calculation/calculation.h"

#include <queue>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = nullptr);

    void recalc(double t, QTextBrowser& TB, double dx, double dy, double dz, double dalpha, double dbeta, double dgamma);
    void restart();

    void changeOffset(double dx, double dy, double dz, double alpha, double beta, double gamma){
        mainScene->changeCameraOffset(dx, dy, dz, alpha, beta, gamma);
    }

    void addTLEObjects(std::string filename, double t){
        mainWorld.addClusterByTLE(filename, t);
    }

signals:

public slots:
    void paintEvent(QPaintEvent *event);

private:
    std::vector<TObject*> objects;
    Scene* mainScene;
    World mainWorld;

    std::vector<std::pair<int, int>> pixelCoords;
};

#endif // DRAWWIDGET_H
