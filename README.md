# Yutrel
个人所制作的一个小渲染器，也可以认为是小引擎的雏形，主要目的在于个人学习OpenGL、Vulkan、c++17，以及加深对游戏引擎架构的理解，同时方便个人学习并实现诸多图形学算法。

## WIP

目前已全面转向Vulkan，OpenGL的部分现已废弃，下方的部分作为历史留档，已不具备实际意义

## 参考项目

[bevy](https://github.com/bevyengine/bevy)

[gecs](https://github.com/VisualGMQ/gecs)

[Vulkan Guide](https://github.com/vblanco20-1/vulkan-guide)

[Piccolo](https://github.com/BoomingTech/Piccolo)

# 历史

## 特点

* 以类似[bevy引擎](https://github.com/bevyengine/bevy)的ECS架构进行资源管理，窗口、渲染器等均作为ECS系统的资源(resource)，同时任意的结构体、类均可以作为实体(Entity)的组件(component)，并在系统(system)中进行创建更新或销毁。
* 渲染器内部类似游戏引擎的架构，分为核心(core)，平台(platform)、功能(function)、资源(resource)四层
* 提供对渲染API的RHI封装，可以方便地进行创建、绑定、删除等操作
* 部分预定义的组件(变换、光照、摄像机、模型、动画、skybox)

## 当前已实现算法或功能

* 基于Cook-Torrance BRDF模型的PBR渲染
* 计算着色器实现IBL烘焙
* 点光源OSM万向阴影贴图
* 平行光CSM级联阴影贴图
* 重要性混合实现OIT顺序无关透明
* 正向渲染与延迟渲染
* SSAO屏幕空间环境光遮蔽
* 基于AABB的视锥体裁剪
* 简单的骨骼动画

## 待实现清单

* 更好的摄像机组件
* arcball对场景物体进行编辑
* 更多的算法
    * 离屏MSAA
    * VXGI
    * SSR
    * 体积云
    * 风格化渲染
    * ......