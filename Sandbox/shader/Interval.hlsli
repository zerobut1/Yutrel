#pragma once

#include "Common.hlsli"

class Interval
{
    float min, max;

    float size()
    {
        return max - min;
    }

    bool contains(float x)
    {
        return min <= x && x <= max;
    }

    bool surrounds(float x)
    {
        return min < x && x < max;
    }
};