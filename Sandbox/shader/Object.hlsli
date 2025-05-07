#pragma once

#include "Common.hlsli"

interface Object
{
    bool hit(Ray ray, float t_min, float t_max, out HitRecord rec);
};