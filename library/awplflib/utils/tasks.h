#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <deque>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace tasks {

   
    // Executor pool class of threads
    template<class T>
    class Executor {
    public:
        explicit Executor(size_t max_threads) : max_threads_(max_threads), stop_flag_(false), active_threads_(0) {}

        ~Executor() {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                stop_flag_ = true;
            }
            condition_.notify_all();
            for (std::thread& worker : workers_) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        // Add a task to the end of the queue
        template<class ...A>
        void EnqueueBottomTask(A... args) {
            AddTask(false, std::forward<Args>(args)...);
        }

        // Add a task to the front of the queue
        template<class ...A>
        void EnqueueTopTask(ITask<T>* task, A... args) {
            AddTask(true, std::forward<Args>(args)...);
        }

    private:
        template<class ...A>
        void AddTask(bool front, A... args) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                if (stop_flag_) {
                    throw std::runtime_error("ThreadPool is stopping");
                }

                if (front) {
                    tasks_.emplace_front(std::forward<Args>(args)...);
                }
                else {
                    tasks_.emplace_back(std::forward<Args>(args)...);
                }

                if (active_threads_ < max_threads_ && workers_.size() < max_threads_) {
                    workers_.emplace_back(&Executor::WorkerThread, this);
                }
            }
            condition_.notify_one();
        }

        void WorkerThread() {
            while (true) {
                
                T task;

                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this] {
                        return stop_flag_ || !tasks_.empty();
                        });

                    if (stop_flag_ && tasks_.empty()) {
                        return;
                    }

                    task = std::move(tasks_.front());
                    
                    tasks_.pop_front();
                }

                ++active_threads_;

                task();

                --active_threads_;
                
            }
        }

        size_t max_threads_;  // Maximum number of threads
        std::vector<std::thread> workers_;  // Worker threads
        std::deque<T> tasks_;  // Task queue

        std::mutex queue_mutex_;
        std::condition_variable condition_;
        std::atomic<bool> stop_flag_;
        std::atomic<size_t> active_threads_;  // Number of active threads
    };
       
}  // namespace tasks
