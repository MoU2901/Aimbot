#pragma once

class Player
{
public:
    int hp;
    int team;
    bool isSpotted;
    float pos[3];
    float ang[3];
    bool strzelono = false;
};
