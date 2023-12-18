#include "yutrel_pch.hpp"

#include "path.hpp"
#include <filesystem>
#include <unordered_map>

namespace Yutrel
{
    // 在启动时搜索路径
    Paths::Paths()
    {
        LOG_INFO("Searching sources and assets paths");

        std::filesystem::path root_path = std::filesystem::current_path();

        // 找到根目录
        while (!std::filesystem::exists(root_path / "Engine"))
        {
            root_path = root_path.parent_path();
        }

        YUTREL_ASSERT(std::filesystem::is_directory(root_path), "Root directory does not exist!");
        YUTREL_ASSERT(!std::filesystem::is_empty(root_path), "Root directory is empty!");

        // 得到各路径
        root     = root_path.string() + "\\";
        source   = (root_path / "Engine" / "src").string() + "\\";
        resource = (root_path / "resource").string() + "\\";
        asset    = (root_path / "Engine" / "asset").string() + "\\";
    }
} // namespace Yutrel