#pragma once

#include <deque>

template<class T>
class XQueue
{

private:
    std::mutex mu;
    std::condition_variable cond;

    std::deque<T> buffer_;
    const unsigned int size_ = 10;

public:
    XQueue() {}

    void add(T data)
    {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [this](){return buffer_.size() < size_;});
        buffer_.push_back(data);
        locker.unlock();
        cond.notify_all();
        return;
    }

    T remove()
    {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [this](){return buffer_.size() > 0;});
        T back = buffer_.back();
        buffer_.pop_back();
        locker.unlock();
        cond.notify_all();
        return back;
    }

};
