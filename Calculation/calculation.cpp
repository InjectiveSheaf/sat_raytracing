#include "calculation.h"



//***********************************************************************************



void CObject::interactWithObject(CObject *secondObject){
    Vec3d r2 = secondObject->r;
    Vec3d dr = r - r2;

    if(dr.norm() < 1/2) return;

    double m1 = mass, m2 = secondObject->mass;

    double dt = 0.000001; // так как время в Spg4 добавляется в микросекундах;
    double G = 6.67430e-20;      // берем в км^3/кг*с^2

    Vec3d a1 = -G * m1 * dr * std::pow(1.0/dr.norm(), 2); // ускорение направлено против вектора, соединяющего тела;
    Vec3d a2 =  G * m2 * dr * std::pow(1.0/dr.norm(), 2);

    v = v + a1*dt; // прибавляем скорость к начальной
    secondObject->v = secondObject->v + a2*dt;

    r = r + v*dt; // меняем радиус-векторы
    secondObject->r = secondObject->r + secondObject->v * dt;
}



//***********************************************************************************



void Cluster::pushToScene(std::vector<TObject*> &objects){
    for(size_t o(0); o < clusterObjects.size(); o++){
        TSphere* TS = clusterObjects[o]->toSceneSphere();
        objects.push_back(TS);
    }
}



//***********************************************************************************



World::World(std::vector<TObject*> &objects, double time){
    objects.clear();

    TMaterial m_EarthMaterial(TColor(20, 128, 20), true, 100, 0.5, 0.3);
    TMaterial m_MoonMaterial(TColor(127,127,127), true, 1000, 0.5, 0.5);

    Cluster* earth_cluster = new Cluster();
    Vec3d earth_rad(0, 0, 0);
    CObject* earth = new CObject(earth_rad, 0*earth_rad, 5.973e24, m_EarthMaterial, EARTH, Tle());
    earth_cluster->addObject(earth);
    addCluster(earth_cluster);

    Cluster* moon_cluster = new Cluster();
    Vec3d rad_vec(0, 384400, 0);
    Vec3d velocity(1.02, 0, 0);

    CObject* moon = new CObject(rad_vec, velocity, 7.36e22, m_MoonMaterial, MOON, Tle());
    moon_cluster->addObject(moon);
    addCluster(moon_cluster);

    for(size_t c(0); c < worldClusters.size(); c++)
        worldClusters[c]->pushToScene(objects);
}



void World::interact(){
    std::vector<CObject*> allObjects;

    for(size_t q = 0; q < worldClusters.size(); q++)
        for(size_t p = 0; p < worldClusters[q]->getClusterSize(); p++)
            allObjects.push_back(&worldClusters[q]->getObjectByNum(p));

    #pragma omp parallel for
    for(size_t q = 0; q < allObjects.size(); q++)
        for(size_t p = 0; p < allObjects.size(); p++)
            if(p != q) allObjects[p]->interactWithObject(allObjects[q]);

}



void World::pushToScene(std::vector<TObject*>& objects){
    objects.clear();
    for(size_t c = 0; c < worldClusters.size(); c++)
        worldClusters[c]->pushToScene(objects);
}



World::World()
{

}

//***********************************************************************************
