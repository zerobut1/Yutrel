add_requires("glfw","glad","glm","assimp","spdlog","entt")
add_requires("imgui v1.89-docking", {configs = {glfw_opengl3 = true}})

includes("editor","Yutrel")