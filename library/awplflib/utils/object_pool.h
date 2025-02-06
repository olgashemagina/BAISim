#pragma once

#include <iostream>
#include <memory>
#include <queue>
#include <mutex>
#include <functional>

namespace pool {

    // Object pool template
    // Provides a mechanism to manage reusable objects to reduce the cost of allocation and deallocation.
    template <typename T>
    class ObjectPool {
    public:
        using ObjectCreator = std::function<std::unique_ptr<T>()>;

        // Constructor takes a custom object creation function
        explicit ObjectPool(ObjectCreator creator, size_t limits = 0) 
            : creator_(std::move(creator))
        , limits_(limits) {}

        // Get an object from the pool. If the pool is empty, a new object is created.
        std::shared_ptr<T> Get() {
            size_t object_index = 0;

            {
                std::unique_lock<std::mutex> lock(mutex_);


                while (limits_ > 0 && objects_.size() >= limits_ && pool_.empty()) {
                    cv_.wait(lock);
                }

                
                if (!pool_.empty()) {
                    // Retrieve an object from the pool
                    T* obj = pool_.front();
                    pool_.pop();

                    // Wrap the object in a shared_ptr with a custom deleter to return it to the pool
                    return std::shared_ptr<T>(obj, [this](T* obj) {
                        this->ReturnObject(obj);
                        });
                }
                object_index = objects_.size();
                objects_.emplace_back();
            }
            // Create a new object if the pool is empty
            auto obj = creator_();
            std::cout << "Create Object " << objects_.size() << std::endl;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                // Create a new object if the pool is empty
                objects_[object_index] = std::move(obj);
                return std::shared_ptr<T>(objects_[object_index].get(), [this](T* obj) {
                    this->ReturnObject(obj);
                    });
            }
                        
        }

        void    WaitTask() {
            std::unique_lock<std::mutex>     locker(mutex_);

            if (objects_.size() > pool_.size()) {
                cv_.wait(locker);
            }
        }

        void    WaitAllTasks() {
            std::unique_lock<std::mutex>     locker(mutex_);

            while (objects_.size() > pool_.size()) {
                cv_.wait(locker);
            }
        }

    private:
        // Return an object to the pool
        void ReturnObject(T* obj) {
            std::unique_lock<std::mutex> lock(mutex_);
            //std::cout << "Return Object " << objects_.size() << std::endl;
            pool_.push(obj);
            //objects_count_--;
            cv_.notify_one();
        }

    private:

        ObjectCreator creator_;  // Function to create new objects

        std::vector<std::unique_ptr<T>> objects_;

        std::queue<T*> pool_;  // Queue to store available objects
        std::mutex mutex_;  // Mutex to ensure thread safety
        std::condition_variable         cv_;
            

        size_t  limits_ = 0;
    };
}