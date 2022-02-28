#ifndef CALCULATION_H
#define CALCULATION_H

#include "Graphics/geometricObjects.h"

#include <string>
#include <vector>

#include <iostream>
#include <fstream>

#include "sgp4lib/SGP4.h"
#include "sgp4lib/Observer.h"
#include "sgp4lib/CoordGeodetic.h"

#include "QTextBrowser"
#include "QString"


enum OBJECT_TYPE{
    SATELLITE,
    EARTH,
    MOON
};


class CObject
{
public:
    CObject(Vec3d r0, Vec3d v0, double m, TMaterial m_mat, OBJECT_TYPE type, Tle tle) : r(r0), v(v0), mass(m), m_material(m_mat), T(type), tle_0(tle)  {    }
    Vec3d& getRadiusVector(){return r;}
    Vec3d& getVelocity(){return v;}

    void interactWithObject(CObject* secondObject);

    void setName(std::string _name){name = _name;}

    double countEnergyNorm(double time){
        if(T == SATELLITE){
            SGP4 sgp4(tle_0);
            DateTime dt = tle_0.Epoch();
            Eci eci = sgp4.FindPosition(dt.AddMicroseconds(time));
            Vector r_t = eci.Position();
            double enorm = Vec3d(r_t.x - r.x, r_t.y - r.y, r_t.z - r.z).norm();
            return enorm;
        }
        else return 0;
    }

    TSphere* toSceneSphere(){
        double rad = 0.005;
        if(T == EARTH) rad = 1;
        if(T == MOON) rad = 0.27;
        TSphere* TS = new TSphere(r*(1.0/6371.0), rad, m_material);
        return TS;
    }

    std::string getTextData(double& summary_norm, double time){
        std::string s;
        s = name;
        if(T == EARTH) s += "Earth       ";
        if(T == MOON) s += "Moon      ";
        s += "R = (" + std::to_string(r.x) + ", " + std::to_string(r.y) + ", " + std::to_string(r.z) + "),     ";
        s += "V = (" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ") ";
        s += "\n";
        if(T == SATELLITE){
            double E = countEnergyNorm(time);
            summary_norm += E;
            s += "Energy norm:" + std::to_string(E) + " km \n";
        }
        return s;
    }

private:
    Vec3d r;
    Vec3d v;
    double mass;
    TMaterial m_material;
    OBJECT_TYPE T;
    std::string name;
    Tle tle_0;
};



class Cluster
{
public:
    Cluster() : clusterMaterial(TColor(rand() % 255, rand() % 255, rand() % 255), 1000, true, 0.1, 0.5)  {   }

    Cluster(std::string filename, double time) : clusterMaterial(TColor(rand() % 255, rand() % 255, rand() % 255), 1000, true, 0.1, 0.5)
    {
        std::string s1, s2, s3;
        std::fstream f(filename);
        while(std::getline(f,s1) && std::getline(f, s2) && std::getline(f,s3)){
            Tle tle = Tle(s2.c_str(), s3.c_str());
            SGP4 sgp4(tle);
            DateTime dt = tle.Epoch();
            Eci eci = sgp4.FindPosition(dt.AddMicroseconds(time));

            Vector eci_vel = eci.Velocity();
            Vec3d vel_vec(eci_vel.x, eci_vel.y, eci_vel.z);

            Vector eci_pos = eci.Position();
            Vec3d rad_vec(eci_pos.x, eci_pos.y, eci_pos.z);

            double sat_mass = 300.0 + rand()%50;
            CObject* C = new CObject(rad_vec, vel_vec*1000, sat_mass, getMaterial(), SATELLITE, tle); // ~250-300 кг масса спутника
            C->setName(s1);
            addObject(C);
        }
    }

public:
    CObject& getObjectByNum(size_t N){
        if (N < clusterObjects.size()) return *clusterObjects[N];
        else return *clusterObjects[0];
    }

    void setRadVecByNum(size_t N, Vec3d* rad_vec){
        if (N < clusterObjects.size()){
            clusterObjects[N]->getRadiusVector().x = rad_vec->x;
            clusterObjects[N]->getRadiusVector().y = rad_vec->y;
            clusterObjects[N]->getRadiusVector().z = rad_vec->z;
        }
    }

    TMaterial getMaterial(){return clusterMaterial;}

    size_t getClusterSize(){return clusterObjects.size();}

    void addObject(CObject* C){ clusterObjects.push_back(C); }

    void pushToScene(std::vector<TObject*>& objects);

    void interactWithCluster(Cluster& secondCluster);

    void setClusterDescription(QTextBrowser& TB, double& summaryNorm , double worldTime){
        for(auto i : clusterObjects)
            cluster_str += i->getTextData(summaryNorm, worldTime);

        TB.insertPlainText(QString::fromStdString(cluster_str));
        cluster_str.clear();
    }

private:
    std::vector<CObject*> clusterObjects;
    TMaterial clusterMaterial;
    std::string cluster_str;
};




class World
{
public:
    World();

    World(std::vector<TObject*>& objects, double time = 0);

    void addCluster(Cluster* C){ worldClusters.push_back(C); }

    void addClusterByTLE(std::string TLEfilename, double world_time){
        Cluster* C = new Cluster(TLEfilename, world_time);
        worldClusters.push_back(C);
    }

    void interact();

    void pushToScene(std::vector<TObject*>& objects);

    void setDescription(QTextBrowser& TB, double world_time){
        TB.clear();
        std::string s = "Time:" + std::to_string((int)world_time) + ", Summary norm: " + std::to_string(summary_norm) + "\n";
        TB.insertPlainText(QString::fromStdString(s));

        summary_norm = 0;
        for(auto i : worldClusters)
            i->setClusterDescription(TB, summary_norm, world_time);
    }

private:
    double summary_norm = 0;
    std::vector<Cluster*> worldClusters;

};

#endif // CALCULATION_H
