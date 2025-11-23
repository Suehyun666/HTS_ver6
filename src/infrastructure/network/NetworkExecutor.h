#pragma once
#include <QThreadPool>
#include <QRunnable>
#include <functional>
#include "../../core/model/Result.h"

class NetworkExecutor {
public:
    static NetworkExecutor& instance();

    Result<void> initialize(int maxThreads);
    void shutdown();

    template<typename Func>
    void execute(Func&& task) {
        class RunnableTask : public QRunnable {
        public:
            explicit RunnableTask(Func&& f) : func_(std::forward<Func>(f)) {}
            void run() override { func_(); }
        private:
            Func func_;
        };

        if (pool_) {
            pool_->start(new RunnableTask(std::forward<Func>(task)));
        }
    }

    bool isRunning() const { return pool_ != nullptr; }

private:
    NetworkExecutor() = default;
    ~NetworkExecutor();

    QThreadPool* pool_ = nullptr;
};
