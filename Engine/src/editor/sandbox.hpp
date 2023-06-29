#pragma once
#include <Yutrel/Yutrel.hpp>

void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnBackpack(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnFramebuffer(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnTextureShader(Yutrel::Commands& cmd, Yutrel::Resources resources);

void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);

// void test1(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
// {
//     std::cout << "test1" << std::endl;
// }

// void test2(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
// {
//     std::cout << "test2" << std::endl;
// }

// class TestClass
// {
// public:
//     int m_value = 1;
// };

// void test3(Yutrel::Commands& cmd, Yutrel::Resources resources)
// {
//     cmd.Spawn<TestClass>(TestClass{});
// }

// void test4(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
// {
//     auto test_class_entities = querier.Query<TestClass>();
//     for (auto eneity : test_class_entities)
//     {
//         auto test_class_component = querier.Get<TestClass>(eneity);
//         LOG_TRACE("{0}", test_class_component.m_value);
//     }
// }