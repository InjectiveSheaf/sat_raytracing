#ifndef TRACER_H
#define TRACER_H

#include <QColor>

#include <cmath>
#include <vector>

#include <algorithm>

#include "geometricObjects.h"


class TLighting
{
public:
    TLighting(const Vec3d& iLight_pos, const double& iLight_force) : light_pos(iLight_pos), light_force(iLight_force) {   }
    Vec3d light_pos;
    double light_force;
    double get_lightforce(Vec3d N, Vec3d L, Vec3d V, TMaterial M)
    {
        double lf = light_force;
        if(M.isDiffusive)
        {
            double cosNL = (N * L)/(N.norm()*L.norm());
            if (cosNL < 0) cosNL = 0;
            lf += light_force * cosNL;
        }
        if (M.specularity > 0)
        {
            Vec3d R = reflectRay(L, N);
            double rv = R*V;
            lf += light_force*std::pow(rv/(R.norm()*V.norm()), M.specularity);
        }
        return lf;
    }
};


class Scene
{
public:
    Scene(int w, int h) : Cw(w), Ch(h) {
        Vh = 1;
        Vw = (Cw + Vh) / Ch;
        Dist = 1;
        CCx = w/2;
        CCy = h/2;

        dx = 0, dy = 0; // смещение камеры

        ambient_color = TColor(40, 40, 40);
        ambient_light = 0.5;

        TMaterial RedTransp(TColor(128, 0, 0), true, 1000.0, 0.2, 0);
        //TMaterial GreenOpaque(TColor(0, 60, 30), true, 1000.0, 0.2, 1);
        //TMaterial GreenTransp(TColor(0, 60, 30), true, 1000.0, 0.2, 0.2);
        TMaterial PinkOpaque(TColor(128, 90, 128), false, 500.0, 0.5, 1);
        TMaterial BlueTransp(TColor(0, 45, 175), false, 500.0, 0.5, 0.3);
        TMaterial BlueOpaque(TColor(0, 45, 175), false, 500.0, 0.5, 1);

        TSphere* TS = new TSphere(Vec3d(-1.5, -1, 5), 1, RedTransp);
        TSphere* TS2 = new TSphere(Vec3d(1.5, 3, 6), 1, RedTransp);
        TSphere* TS3 = new TSphere(Vec3d(2, 2.2, 15), 1, BlueOpaque);

        TTRiangle* TT = new TTRiangle(Vec3d(-2, -2, 10), Vec3d(1, 2, 10), Vec3d(4, -2, 10), BlueTransp);
        //TTRiangle* TT2 = new TTRiangle(Vec3d(-2, -2, 5), Vec3d(1, 2, 5), Vec3d(4, -2, 5), TriangleMat);

        TSphere* Tfloor = new TSphere(Vec3d(0, -1001, 5), 999, PinkOpaque);

        objects.push_back(TS);
        objects.push_back(TS2);
        objects.push_back(TS3);
        objects.push_back(TT);

        objects.push_back(Tfloor);

        TLighting* TL = new TLighting(Vec3d(-1, 1, 5), 1);
        lightings.push_back(TL);
        TLighting* TL2 = new TLighting(Vec3d(5, 5, 0), 1);
        lightings.push_back(TL2);
    }

    Scene(){
        ambient_color = TColor(40, 40, 40);
        ambient_light = 0.5;
        dx = 0, dy = 0; // смещение камеры

        TLighting* TL2 = new TLighting(Vec3d(-100, 25, -25), 0.8);
        lightings.push_back(TL2);
    }

    TColor getPixel(int x, int y);

    void setObjects(std::vector<TObject*>& obj){
        objects = obj;
    }

    void setSizes(int w, int h){
        Cw = w;
        Ch = h;
        Vh = 1;
        Vw = (Cw + Vh) / Ch;
        Dist = 1;
        CCx = w/2;
        CCy = h/2;
    }

    void changeCameraOffset(double x, double y, double z, double alpha, double beta, double gamma){
        dx = x;
        dy = y;
        dz = z;
        dalpha = alpha;
        dbeta = beta;
        dgamma = gamma;
    }

    TColor castRay(Vec3d& ray_src, Vec3d& ray_dir, double recursion_depth, uint max_depth);


protected:
    double Vw, Vh;
    double Dist;

    int CCx, CCy;

    int Cw;
    int Ch;

    double dx = 0, dy = 0, dz = 0;

    double ambient_light;
    TColor ambient_color;


    std::vector<TObject*> objects;
    std::vector<TLighting*> lightings;

    double dalpha = 0, dbeta = 0, dgamma = 0;
};

#endif // TRACER_H
