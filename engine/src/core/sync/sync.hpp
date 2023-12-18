/**
 * 同步
 * 主要是供compute shader使用
 * todo 重写
 */
#pragma once

#include <glad/glad.h>
#include <stdint.h>

namespace Yutrel
{
    // 这个类是凑起来的，以后重写
    class Sync
    {
    public:
        Sync(uint32_t id);
        ~Sync();

        Sync(const Sync&)             = delete;
        Sync(Sync&& other)            = delete;
        Sync& operator=(const Sync&)  = delete;
        Sync& operator=(Sync&& other) = delete;

        bool Signaled();
        void ClientWaitSync(uint64_t timeout = 1e5); // 0.1 ms
        void ServerWaitSync();

        static int64_t GetServerTimeout();
        static void WaitFlush();
        static void WaitFinish();

    private:
        uint32_t id;
        GLsync sync = nullptr;
    };

} // namespace Yutrel