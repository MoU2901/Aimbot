#pragma once

class Point
{
public:
    float x;
    float y;
    float z;

    double distance(const Point& p)
    {
        return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
    }
};

class Player
{
public:
    int hp;
    int team;
    bool isSpotted;
    Point pos;
    Point ang;
    bool strzelono = false;
};
