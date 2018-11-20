//
//  consumer.hpp
//  TopUrl100
//

#ifndef consumer_h
#define consumer_h
#include "producer.h"
#include "common.h"
using namespace std;

class Producer;
class Consumer{
public:
  Consumer(){init();}
  inline void init();
  int chooseConsumeDeque(Producer*);
  int convert(Producer*);
  int topk();
  void consume(Producer*);
  thread getConsumerThread(Producer&);
//  void ConsumerThread(Producer&);
public:
  static pthread_cond_t cond2;
  static pthread_mutex_t mut2;
private:
  HashUrlMap UrlMap;
  priority_queue<UrlPair> TopK;
};

void Consumer::init(){
  memset(this, 0, sizeof(*this));
  UrlMap.reserve(N);
  UrlMap.rehash(N);
}

pthread_cond_t Consumer::cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t Consumer::mut2 = PTHREAD_MUTEX_INITIALIZER;
#endif /* consumer_h */

