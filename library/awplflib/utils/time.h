#pragma once
#include <chrono>
#include <iostream>

class TimeDiff
{
public:
    TimeDiff(bool print_on_destroy = false) : print_on_destroy_(print_on_destroy) {}
    ~TimeDiff() {
        if (print_on_destroy_)
            std::cout << "Elapsed time is " << GetDiffMs() << " ms." << std::endl;
    }
    
    int GetDiffMs() {
        const std::chrono::duration<double, std::milli> elapsed = 
            std::chrono::high_resolution_clock::now() - start_;
        return elapsed.count();
    }
private:
    std::chrono::steady_clock::time_point start_ = std::chrono::high_resolution_clock::now();
    bool print_on_destroy_ = false;
};
