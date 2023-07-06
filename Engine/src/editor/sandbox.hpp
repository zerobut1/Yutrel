#pragma once
#include <Yutrel/Yutrel.hpp>

#include "ui/editor_ui.hpp"

// 渲染场景
void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnLight(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnScene(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnSkybox(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnShadowmap(Yutrel::Commands& cmd, Yutrel::Resources resources);
void UpdateScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);
void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);

// 显示UI
void SpawnViewport(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnUI(Yutrel::Commands& cmd, Yutrel::Resources resources);
void DrawUI(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);