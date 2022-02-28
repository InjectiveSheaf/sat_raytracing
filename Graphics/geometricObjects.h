#ifndef GEOMETRICOBJECTS_H
#define GEOMETRICOBJECTS_H

// #include <eigen3/Eigen/Core>
#include <QColor>
#include <cmath>
#include <functional>
#include <vector>


//*********************************************************************




class Vec3d{
public:
    Vec3d() {  }
    Vec3d(double ix, double iy, double iz) : x(ix), y(iy), z(iz) {   }

    Vec3d operator +(const Vec3d &v) const{
        return Vec3d(x + v.x, y + v.y, z + v.z);
    }
    Vec3d operator -(const Vec3d &v) const{
        return Vec3d(x - v.x, y - v.y, z - v.z);
    }
    double operator *(const Vec3d &v) const{
        return x * v.x + y * v.y + z * v.z;
    }
    Vec3d operator *(const double &d) const{
        return Vec3d(x * d, y * d, z * d);
    }
    bool operator ==(const Vec3d &v) const{
        return x == v.x && y == v.y && z == v.z;
    }
    double norm() const{
        return std::sqrt(x*x + y*y + z*z);
    }
    void normalize(){
        double l = std::abs(this->norm());
        if (l == 0) return;
        x = x / l;
        y = y / l;
        z = z / l;
    }

    double x, y, z;

    void rotateX(double alpha){
        double y_new = y*std::cos(alpha) - z*std::sin(alpha);
        double z_new = z*std::cos(alpha) + y*std::sin(alpha);
        y = y_new;
        z = z_new;
    }

    void rotateY(double alpha){
        double x_new = x*std::cos(alpha) + z*std::sin(alpha);
        double z_new = z*std::cos(alpha) - x*std::sin(alpha);
        x = x_new;
        z = z_new;
    }

    void rotateZ(double alpha){
        double x_new = x*std::cos(alpha) - y*std::sin(alpha);
        double y_new = y*std::cos(alpha) + x*std::sin(alpha);
        x = x_new;
        y = y_new;
    }
};


inline Vec3d operator*(const double&f, const Vec3d &v) {
    return v*f;
}


inline Vec3d crossProd(const Vec3d& v1, const Vec3d& v2) {
    double Vx = v1.y * v2.z - v1.z * v2.y,
           Vy = v1.z * v2.x - v1.x * v2.z,
           Vz = v1.x * v2.y - v1.y * v2.x;
    return Vec3d(Vx, Vy, Vz);
}


inline Vec3d reflectRay(Vec3d R, Vec3d N){
    return 2*N*(R*N) - R;
}


//*********************************************************************


class TColor
{
public:
    TColor() {   }
    TColor(ushort iR, ushort iG, ushort iB) : R(iR), G(iG), B(iB) {   }
    TColor lightened(double light_force) const{
        ushort tR = std::min(255, static_cast<int>(light_force * R));
        ushort tG = std::min(255, static_cast<int>(light_force * G));
        ushort tB = std::min(255, static_cast<int>(light_force * B));
        return TColor(tR, tG, tB);
    }
    QColor toQColor() const{
        return QColor(R, G, B);
    }

    TColor operator *(const double &d) const{
        return TColor(R * d, G * d, B * d).lightened(1);
    }
    TColor operator +(const TColor &v) const{
        return TColor(R + v.R, G + v.G, B + v.B).lightened(1);
    }

//private:
    ushort R, G, B;
};


struct TMaterial{
    TMaterial (TColor col, bool _isDiff, double _spec, double _reflect, double _op) :
        color(col), isDiffusive(_isDiff), specularity(_spec), reflectivity(_reflect), opacity(_op) {   }
    TMaterial () {   }
    TColor color;
    bool isDiffusive;
    double specularity;
    double reflectivity;
    double opacity;
};


//*********************************************************************


class TObject
{
public:
    TObject() {
        material.color = TColor(127, 127, 127);
    }
    TObject(const TMaterial& M) : material(M) {   }

    virtual bool isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir,
                             double & tau, Vec3d& intersect_pt, Vec3d& normal_vector) = 0;

    const TColor & get_color() const { return material.color; }
    const bool   & get_type() const { return material.isDiffusive; }
    const double & get_specularity() const { return material.specularity; }
    const double & get_reflectivity() const { return material.reflectivity; }
    const TMaterial & get_material() const {return material;}

protected:
    TMaterial material;
};



class TSphere : public TObject
{
public:
    TSphere(const Vec3d& iCenter, const double& iR, const TMaterial& M) :
        TObject(M), center(iCenter), R(iR) {   }

    virtual bool isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir, double & tau);
    virtual bool isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir,
                                  double & tau, Vec3d& intersect_pt, Vec3d& normal_vector);
private:
    Vec3d center;
    double R;
};



class TTRiangle : public TObject
{
public:
    TTRiangle(const Vec3d& _A, const Vec3d& _B, const Vec3d& _C, const TMaterial& M) :
        TObject(M), A(_A), B(_B), C(_C) {   }

    virtual bool isIntersect(const Vec3d& ray_src, const Vec3d& ray_dir,
                                  double & tau, Vec3d& intersect_pt, Vec3d& normal_vector);
private:
    Vec3d A, B, C;
};



class TSurface : public TObject
{
public:
    TSurface(const TMaterial& M, std::function<double(double, double)> F_xz, double xmin, double xmax, double zmin, double zmax, double gridsize, Vec3d offset) :
        TObject(M), m_xmin(xmin), m_xmax(xmax), m_zmin(zmin), m_zmax(zmax)
    {
        for(double x = m_xmin; x < m_xmax; x += gridsize){
            for(double z = m_zmin; z < m_zmax; z += gridsize){
                Vec3d m_A(x,          F_xz(x,z),                   z),
                      m_B(x+gridsize, F_xz(x+gridsize,z+gridsize), z+gridsize),
                      m_C(x+gridsize, F_xz(x+gridsize, z),         z),
                      m_D(x,          F_xz(x, z+gridsize),         z+gridsize);
                TTRiangle* T1 = new TTRiangle(m_A+offset, m_B+offset, m_C+offset, M);
                TTRiangle* T2 = new TTRiangle(m_A+offset, m_D+offset, m_B+offset, M);
                triangles.push_back(T1);
                triangles.push_back(T2);
            }
        }
    }

    void pushToObjects(std::vector<TObject*>& objects){
        objects.insert(objects.end(), triangles.begin(), triangles.end());
    }

private:
    double m_xmin, m_xmax, m_zmin, m_zmax;
    std::vector<TObject*> triangles;
};


//*********************************************************************


#endif // GEOMETRICOBJECTS_H
