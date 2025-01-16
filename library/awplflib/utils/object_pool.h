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
        explicit ObjectPool(ObjectCreator creator) : creator_(std::move(creator)) {}

        // Get an object from the pool. If the pool is empty, a new object is created.
        std::shared_ptr<T> GetObject() {
            std::lock_guard<std::mutex> lock(mutex_);

            if (pool_.empty()) {
                // Create a new object if the pool is empty
                return std::shared_ptr<T>(creator_().release(), [this](T* obj) {
                    this->ReturnObject(std::unique_ptr<T>(obj));
                    });
            }

            // Retrieve an object from the pool
            std::unique_ptr<T> unique_obj = std::move(pool_.front());
            pool_.pop();

            // Wrap the object in a shared_ptr with a custom deleter to return it to the pool
            return std::shared_ptr<T>(unique_obj.release(), [this](T* obj) {
                this->ReturnObject(std::unique_ptr<T>(obj));
                });
        }

    private:
        // Return an object to the pool
        void ReturnObject(std::unique_ptr<T> obj) {
            std::lock_guard<std::mutex> lock(mutex_);
            pool_.push(std::move(obj));
        }

        ObjectCreator creator_;  // Function to create new objects
        std::queue<std::unique_ptr<T>> pool_;  // Queue to store available objects
        std::mutex mutex_;  // Mutex to ensure thread safety
    };
}