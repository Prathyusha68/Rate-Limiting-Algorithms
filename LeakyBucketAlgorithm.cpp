#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

class LeakyBucket
{
private:
    int capacity;
    int leakRate;
    queue<string> que;
    mutex mtx;
    condition_variable cv;
    bool stop;
    int reqsAdded;

public:
    LeakyBucket(int capacity, int leakRate) : capacity{capacity}, leakRate{leakRate}, reqsAdded{0}
    {
        cout << "Leak Bucket : capacity : " << capacity << " leakRate : " << leakRate << endl;
    }

    void addRequest(string buffer)
    {
        unique_lock<mutex> lock(mtx);
        if (reqsAdded < capacity)
        {
            que.push(buffer);
            reqsAdded++;
        }
        lock.unlock();

        cv.notify_one();
    }

    void leakWater()
    {
        while (!stop)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            cv.notify_all();
        }
    }

    void handleRequests()
    {
        while (1)
        {
            unique_lock<mutex> lock(mtx);

            cv.wait(lock, [&]
                    { return (stop || (!que.empty())); });

            if (stop || que.empty())
                break;

            int reqHandled = 0;

            while ((!que.empty()) && (reqsAdded > 0) && (reqHandled < leakRate))
            {
                string buffer = que.front();
                que.pop();
                cout << "Request is handled : " << buffer << endl;
                reqsAdded--;
                reqHandled++;
            }
        }
    }

    ~LeakyBucket()
    {
        lock_guard<mutex> lock(mtx);

        stop = true;

        cv.notify_all();
    }
};

int main()
{
    LeakyBucket leakBucket(10, 8);

    thread leakWaterThread(LeakyBucket::leakWater, &leakBucket);

    thread handleReqThread(LeakyBucket::handleRequests, &leakBucket);

    int count(100);

    while (count)
    {
        if (count % 40)
        {
            cout << "waited..." << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        string buffer = "req" + to_string(count);
        leakBucket.addRequest(buffer);
        count--;
    }

    //leakWaterThread.join();
    //handleReqThread.join();
}