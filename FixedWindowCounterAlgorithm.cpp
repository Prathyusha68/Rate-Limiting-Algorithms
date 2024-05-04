#include <iostream>
#include <mutex>
#include <chrono>
#include<thread>
using namespace std;

class FixedWindowCounter
{
private:
    int maxReqPerWindow;
    int windowSize;
    int currCount;
    chrono::steady_clock::time_point windowStart;
    mutex mtx;

public:
    FixedWindowCounter(int maxReqPerWindow, int windowSize) : maxReqPerWindow{maxReqPerWindow}, windowSize{windowSize}
    {
        currCount = 0;
        windowStart = std::chrono::steady_clock::now();
    }

    bool allowReq()
    {
        if ((std::chrono::steady_clock::now() - windowStart) >= std::chrono::seconds(windowSize))
        {
            windowStart = std::chrono::steady_clock::now();
            currCount = 0;
        }

        if (currCount >= maxReqPerWindow)
        {
            cout << "Max req per window are already handled so can't hanlde thsi req" << endl;
            return false;
        }

        currCount++;
       
        return true;
    }
};

int main()
{
    FixedWindowCounter fixedWindowCounter(10, 1);

    int cnt = 1000;

    while(cnt)
    {
        if(cnt % 20 == 0)
        {
            cout << "sleep for 500ms" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        cnt--;
        if(fixedWindowCounter.allowReq())
        {
            cout << "Request can be allowd" << endl;
        }
        else
        {
            cout << "Request can't be handled" << endl;
        }
    }
        return 0;
}