class Ray
{
    double3 origin;
    double3 direction;

    double3 At(float t)
    {
        return origin + direction * t;
    }
};

struct HitRecord
{
    double3 position;
    double3 normal;
    double t;
};

interface Object
{
    bool hit(Ray ray, double t_min, double t_max, out HitRecord rec);
};