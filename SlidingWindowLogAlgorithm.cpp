#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
using namespace std;

class SlidingWindowLog
{
private:
    int windowSize;
    int maxReqPerWindow;
    void (*drop_callback)(int);
    void (*allow_callback)(int);
    queue<std::chrono::steady_clock::time_point> logs;

public:
    SlidingWindowLog(int maxReqPerWindow, int windowSize, void (*drop_callback)(int), void (*allow_callback)(int))
    {
        this->maxReqPerWindow = maxReqPerWindow;
        this->windowSize = windowSize;
        this->drop_callback = drop_callback;
        this->allow_callback = allow_callback;
    }

    void handleRequest(int req)
    {
        chrono::steady_clock::time_point currTime = chrono::steady_clock::now();

        while ((!logs.empty()) && ((currTime - logs.front()) > chrono::seconds(windowSize)))
        {
            logs.pop();
        }

        if (logs.size() < maxReqPerWindow)
        {
            logs.push(std::chrono::steady_clock::now());
            allow_callback(req);
        }
        else
        {
            drop_callback(req);
        }
    }
};

void allowPacketCb(int req)
{
    cout << "Allow this packet : " << req << endl;
}

void dropPacketCb(int req)
{
    cout << "Drop this packet : " << req << endl;
}

int main()
{
    SlidingWindowLog slidingWindowLog(100, 5, dropPacketCb, allowPacketCb);

    int count = 0;

    while (count < 400)
    {
        if ((count != 0) && (count % 200 == 0))
        {
            cout << "Sleep for 5sec" << endl;
            std::this_thread::sleep_for(chrono::seconds(5));
        }
        slidingWindowLog.handleRequest(count);
        count++;
    }

    return 0;
}