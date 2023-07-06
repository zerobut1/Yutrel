#pragma once
#include <Yutrel/Yutrel.hpp>

void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnFramebuffer(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnLight(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnScene(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnSkybox(Yutrel::Commands& cmd, Yutrel::Resources resources);

void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);