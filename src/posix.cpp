#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <shared_mutex>


// shared_mutex_write_prio is a shared_mutex
// that gives writer priority. This prevents starvation 
// of write locks when multiple readers are polling
// the resource.
class shared_mutex_write_prio : public std::shared_mutex
{
    public:
    shared_mutex_write_prio()
    {
#ifdef __USE_GNU

        // Lets do this the ugly way.
        // We need to setup 
        auto m = static_cast<pthread_rwlock_t *>(native_handle());
        // Destroy already initialized rwlock
        pthread_rwlock_destroy(m);
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
        // Create a new one
        pthread_rwlock_init(m, &attr);
#else
    #error Must be using GNU for this
#endif
    }

    ~shared_mutex_write_prio() = default;

    shared_mutex_write_prio(const shared_mutex_write_prio&) = delete;
    shared_mutex_write_prio& operator=(const shared_mutex_write_prio&) = delete;
};

class Bar
{
    public:
    Bar() = default;

    shared_mutex_write_prio m_mutex;
    unsigned int m_num_clients{0};
    bool m_running{true};

};

void shared_poller(Bar& f, int n)
{
    while(f.m_running)
    {
        std::shared_lock<std::shared_mutex> lock(f.m_mutex);
        std::cout << "Poller " << n << " has lock" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lock.unlock();
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void shared_releaser(Bar& f)
{
    std::cout << "Releaser sleeping for 2s" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Releaser trying to take mutex" << std::endl;

    std::unique_lock<std::shared_mutex> lock(f.m_mutex);
    std::cout << "Releaser stop running" << std::endl;
    f.m_running = false;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Bar f;

    std::cout << "Starting thread1" << std::endl;
    std::thread thread1(shared_poller, std::ref(f), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Starting thread2" << std::endl;
    std::thread thread2(shared_poller, std::ref(f), 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Starting thread3" << std::endl;
    std::thread thread3(shared_releaser, std::ref(f));


    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}
