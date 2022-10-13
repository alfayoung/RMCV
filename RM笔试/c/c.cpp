#include <stdlib.h>  // std::rand()

#include <condition_variable>  // std::condition_variable
#include <iostream>            // std::cout
#include <mutex>               // std::mutex, std::unique_lock
#include <queue>               // std::queue
#include <thread>              // std::thread

const int NUM_PICTURES{100};

void publish(std::mutex* mtx, std::condition_variable* cv, std::queue<int>* q) {
  int count = NUM_PICTURES;
  while (count > 0) {
    std::unique_lock<std::mutex> lck{*mtx};
    // START CODE HERE (≈ 1 lines of code)
	while (!q->empty()) cv->wait(lck);
    // END CODE HERE
    int data = std::rand() % 100;
    q->push(data);
    --count;
    cv->notify_one();
  }
}

void subscribe(std::mutex* mtx, std::condition_variable* cv,
               std::queue<int>* q) {
  // START CODE HERE (≈ 9 lines of code)
  	for (;;) {
		std::unique_lock<std::mutex> lck{*mtx};
		while (q->empty()) cv->wait(lck);
		while (!q->empty()) {
	  		std::cout << "output : " << q->front() << std::endl;
    		q->pop();
		}
		cv->notify_one();
  	}
  // END CODE HERE
}

int main() {
  std::srand(1);
  std::mutex mtx;              // 全局互斥锁.
  std::condition_variable cv;  // 全局条件变量.
  std::queue<int> q;
  std::thread publisher{publish, &mtx, &cv, &q};
  std::thread subscriber{subscribe, &mtx, &cv, &q};
  publisher.join();
  subscriber.join();
  return 0;
}
