#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <condition_variable>

class Foo
{
    public:
    Foo() = default;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    unsigned int m_num_clients{0};
    bool m_running{true};

};

void poller(Foo& f)
{
    while(f.m_running)
    {
        std::unique_lock<std::mutex> lock(f.m_mutex);
        std::cout << "Poller has lock" << std::endl;
        ++f.m_num_clients;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        --f.m_num_clients;
        f.m_cv.notify_all();
        lock.unlock();
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void releaser(Foo& f)
{
    std::cout << "Releaser sleeping for 2s" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Releaser trying to take mutex" << std::endl;

    std::unique_lock<std::mutex> lock(f.m_mutex);
    while(f.m_num_clients)
        f.m_cv.wait(lock);
    std::cout << "Releaser stop running" << std::endl;
    f.m_running = false;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Foo f;

    std::cout << "Starting thread1" << std::endl;
    std::thread thread1(poller, std::ref(f));
    std::cout << "Starting thread2" << std::endl;
    std::thread thread2(releaser, std::ref(f));


    thread1.join();
    thread2.join();

    return 0;
}
