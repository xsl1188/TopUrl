//
//  producer.cpp
//  TopUrl100
//

#include "producer.h"
#include "consumer.h"
using namespace std;
/*
 生产者部分逻辑
 */
//生产者寻找队列，由于目前队列数目为1；所以，其实就是查看该队列是否可以操作（是否为空）
int Producer::chooseProduceQueue(){
  int index = -1;
  for(int i = 0; i < QLEN; i++){
    if(UrlQueue[i].empty())
      index = i;
  }
  return index;
}

//从文件中读取url并且把url压入对列之中
int Producer::produce(const string& file){
  int ret = SUCCESS;
  int count = 0, index = -1;
  ifstream infile(file);
  if(!infile.is_open()){
    cout<<"Open Fisle Error"<<'\n';
  }
  else{
    while(!infile.eof()){
      while(!infile.eof() && (index = chooseProduceQueue()) == -1){
        pthread_mutex_lock(&mut1);
        pthread_cond_wait(&cond1, &mut1);
        pthread_mutex_unlock(&mut1);
      }
      if(infile.eof()){
        isEnd = true;
        break;
      }
      count = 0;
      while(count < N){
        if(infile.eof()){   //最后一次可能不够N条数目，但是已经达到文件末尾了。
          isEnd = true;
          break;
        }
        else{
          string str;
          getline(infile, str);
          UrlQueue[index].push_back(str);
          ++count;
        }
      }
      pthread_cond_broadcast(&Consumer::cond2);
    }
    infile.close();
  }
  return ret;
}

bool Producer::getEnd(){
  return isEnd;
}

vector<string>* Producer::getUrlQueue(){
  return &UrlQueue[0];
}

//vector<string>* Producer::chooseConsumeQueue(){
//  int index = -1;
//  for(int i = 0; i < QLEN; i++){
//    if(!UrlQueue[i].empty())
//      index = i;
//  }
//  if(index != -1)
//    return &UrlQueue[index];
//  else
//    return NULL;
//}


