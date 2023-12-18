/**
 * 存储部分路径，方便以相对路径进行索引
 * 默认资源的一部分
 * 初始化时会搜索当前目录，并存储部分路径方便索引
 */
#pragma once

#include <string>

namespace Yutrel
{
    class Paths final
    {
    public:
        Paths();

    public:
        std::string root;
        std::string source;
        std::string resource;
        std::string asset;
    };
} // namespace Yutrel