#include "tracer.h"
#include <cmath>

bool TSphere::isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir, double & tau){

    Vec3d  v = ray_src - center;
    double A = ray_dir * ray_dir;
    double B = v * ray_dir;
    double C = v*v - R*R;

    double D4 = B*B - A*C;
    if (D4 < 0) return false;

    double t1 = (-B - std::sqrt(D4)) / A;
    double t2 = (-B + std::sqrt(D4)) / A;

    if (t1 < 0 && t2 < 0) return false;
    else if (t1 < 0)  tau = t2;
    else tau = t1;

    return true;
}



bool TSphere::isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir, double & tau, Vec3d& intersect_pt,
                          Vec3d& normal_vector){
    if(isIntersect(ray_src, ray_dir, tau)){
        intersect_pt = ray_src + tau * ray_dir;
        normal_vector = (intersect_pt - center);
        normal_vector.normalize();
        return true;
    }
    else return false;
}



bool TTRiangle::isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir,
                            double & tau, Vec3d& intersect_pt, Vec3d& normal_vector){
    Vec3d AB = B - A;
    Vec3d AC = C - A;
    Vec3d AO = A - ray_src;

    double denom = AB*crossProd(AC, ray_dir);

    double u = -1*AO*crossProd(AC, ray_dir)/denom;
    if(u < 0 || u > 1) return false;

    double v = -1*AB*crossProd(AO, ray_dir)/denom;
    if(v < 0 || u+v > 1) return false;

    double t = AB*crossProd(AC, AO)/denom;
    if(t <= 0) return false;

    intersect_pt = ray_src + t*ray_dir;
    tau = t;
    normal_vector = crossProd(AB,AC);
    normal_vector.normalize();
    return true;
}


TColor Scene::castRay(Vec3d& ray_src, Vec3d& ray_dir, double recursion_depth, uint max_depth){
    bool intersect = false;
    double tau;
    Vec3d P, N; // intersect point and normal vector to ray to object;
    TMaterial M;// object material

    #pragma omp parallel for
    for(size_t q = 0; q < objects.size(); q++){
        double tT;
        Vec3d  tP, tN;
        if(objects[q]->isIntersect(ray_src, ray_dir, tT, tP, tN)){
            if(!intersect || tT < tau){
                tau = tT;
                N = tN;
                M = objects[q]->get_material();
                P = tP;
            }
            intersect = true;
        }
    }

    if(!intersect || recursion_depth > max_depth) return ambient_color;

    // теперь освещаем
    double l_force = ambient_light;

    for(size_t l(0); l < lightings.size(); l++)
    {
        Vec3d l_dir = lightings[l]->light_pos - P;
        if (l_dir * N > 0)  l_force += (l_dir * N)/(l_dir.norm() * N.norm()) * lightings[l]->get_lightforce(N, l_dir, ray_dir, M);
    }

    // если не пересеклись и не достигли предела рекурсии, начинаем считать отраженный свет
    Vec3d ref_src, ref_dir;

    ref_dir = reflectRay(-1*ray_dir, N);
    ref_src = ref_dir*N < 0 ? P - N*1e-3 : P + N*1e-3;
    TColor reflected_color = castRay(ref_src, ref_dir, recursion_depth+1, max_depth);

    // посчитали отраженный, теперь хотим преломленный
    // необходимо смешать цвет, который проходит сквозь объект, т.е. исходная точка остаётся P, но луч не преломляется

    Vec3d ref_src2, ref_dir2;
    ref_dir2 = ray_dir;
    ref_src2 = ref_dir2*N < 0 ? P - N*1e-3 : P + N*1e-3;
    TColor op_color = castRay(ref_src2, ref_dir2, recursion_depth+1, max_depth);

    reflected_color = reflected_color * M.reflectivity + M.color * (1-M.reflectivity);
    TColor final_color = reflected_color * M.opacity + op_color * (1-M.opacity);

    return final_color.lightened(l_force);
}


TColor Scene::getPixel(int x, int y){
    double rX = Vw * (x - CCx) / Cw;
    double rY = Vh * (CCy - y) / Ch;

    Vec3d ray_src(rX+dx, rY+dy, Dist+dz);
    Vec3d ray_dir(rX, rY, Dist);

    if(dalpha != 0) ray_dir.rotateY(dalpha);
    if(dgamma != 0) ray_dir.rotateX(dgamma);
    if(dbeta != 0) ray_dir.rotateZ(dbeta);

    return castRay(ray_src, ray_dir, 0, 2);
}


/*


TColor Scene::getPixel(int x, int y){
    double rX = Vw * (x - CCx) / Cw;
    double rY = Vh * (CCy - y) / Ch;

    Vec3d ray_src(rX+dx, rY+dy, Dist+dz);
    Vec3d ray_dir(rX, rY, Dist);

    if(angle != 0) ray_dir.rotateY(angle);

    bool intersect = false;
    double tau;
    Vec3d P, N;
    TColor Col;
    TProperty lightningType;
    double specularity;

    for(size_t q(0); q < objects.size(); q++){
        double tT;
        Vec3d  tP, tN;
        if(objects[q]->isIntersect(ray_src, ray_dir, tT, tP, tN)){
            if(!intersect || tT < tau){
                tau = tT;
                N = tN;
                Col = objects[q]->get_color();
                lightningType = objects[q]->get_type();
                specularity = objects[q]->get_specularity();
                P = tP;
            }
            intersect = true;
        }
    }

    if(!intersect) return ambient_color;

    double l_force = ambient_light;
    for(size_t l(0); l < lightings.size(); l++){
        Vec3d l_dir = lightings[l]->light_pos - P;
        l_dir.normalize();
        if (l_dir * N > 0)
            l_force += (l_dir * N) * lightings[l]->get_lightforce(N, l_dir, ray_dir, specularity, lightningType);
    }

    return Col.lightened(l_force);
}*/

