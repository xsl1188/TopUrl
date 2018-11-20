//
//  producer.hpp
//  TopUrl100
//

#ifndef producer_h
#define producer_h
#include "common.h"
#include "consumer.h"
using namespace std;

class Consumer;
class Producer{
public:
  Producer(){init();}
  inline void init();
  int produce(const string& file);
  int chooseProduceQueue();
  vector<string>* getUrlQueue();
  vector<string>* chooseConsumeQueue();
  pthread_cond_t getCond();
  bool getEnd();
public:
  static pthread_cond_t cond1;
  static pthread_mutex_t mut1;
private:
  vector<string> UrlQueue[QLEN];
  bool isEnd;
};

void Producer::init(){
  memset(this, 0, sizeof(*this));
  for(int i = 0; i < QLEN; i++){
    UrlQueue[i].reserve(N);
  }
  isEnd = false;
}

pthread_cond_t Producer::cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t Producer::mut1 = PTHREAD_MUTEX_INITIALIZER;
#endif /* producer_h */

