#pragma once

class Point
{
public:
    float x;
    float y;
    float z;

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
