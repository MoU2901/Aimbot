#include <math.h>
#include "Classes.h" 

Point::x = 0.0f;
Point::y = 0.0f;
Point::z = 0.0f;

float Point::distance(const Point& p)const
{
    return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
}
