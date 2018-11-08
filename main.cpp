//
//  main.cpp
//  TopUrl100
//
#include <iostream>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <queue>
#include <map>
#include "hashutil.h"
#define N 100000   //一次读取url数目，可手动修改
#define K 100      //topk设置为too100
#define QLEN 1    //设置缓冲队列数目，目前数目为1，设置成pipeline设计
#define SUCCESS 0   //成功码
#define ERROR 1      //错误码
using namespace std;
using HashUrlMap = unordered_map<string, int>;
using UrlPair = pair<int, string>;

HashUrlMap UrlMap;
deque<string> UrlQueue[QLEN];
multimap<int, string, greater<int>> tmp;
priority_queue<UrlPair> TopK;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

/*
 生产者部分逻辑
 */

//生产者寻找队列，由于目前队列数目为1；所以，其实就是查看该队列是否可以操作（是否为空）
int chooseProduceDeque(){
  int index = -1;
  for(int i = 0; i < QLEN; i++){
    if(UrlQueue[i].empty())
      index = i;
  }
  return index;
}

//从文件中读取url并且把url压入对列之中
int readUrl(string& file){
  int ret = SUCCESS;
  int count = 0, index = -1;
  ifstream infile(file);
  if(!infile.is_open()){
    cout<<"Open File Error"<<'\n';
  }
  else{
    while(!infile.eof()){
      while((index = chooseProduceDeque()) == -1){
        pthread_mutex_lock(&mut);
        pthread_cond_wait(&cond1, &mut);
        pthread_mutex_unlock(&mut);
      }
      count = 0;
      while(count < N){
        if(infile.eof())   //最后一次可能不够N条数目，但是已经达到文件末尾了。
          break;
        string str;
        getline(infile, str);
        UrlQueue[index].push_back(str);
        ++count;
      }
      pthread_cond_broadcast(&cond2);
    }
    infile.close();
  }
  return ret;
}

/*
 消费者部分逻辑
*/

//选择可以消费的队列
int chooseConsumeDeque(){
  int index = -1;
  for(int i = 0; i < QLEN; i++){
    if(!UrlQueue[i].empty())
      index = i;
  }
  return index;
}

//将队列中元素逐步转换成hashmap中，并逐步清空队列，使得生产者可以操作
int convert(){
  int ret = SUCCESS;
  string str;
  int index = -1;
  while((index = chooseConsumeDeque()) == -1){
    pthread_mutex_lock(&mut);
    pthread_cond_wait(&cond2, &mut);
    pthread_mutex_unlock(&mut);
  }
  while(!UrlQueue[index].empty()){
    str = UrlQueue[index].front();
    if(UrlMap.count(str) > 0){
      UrlMap[str]++;
    }
    else{
      UrlMap[str] = 1;
    }
    UrlQueue[index].pop_front();
  }
  pthread_cond_broadcast(&cond1);
  return ret;
}

//选取topk元素，每一次先将hashmap中的元素转换成multimap中，根据频数排序，然后选择前k个元素；
//并且以堆的方式进行控制数目，每次转换到TopK优先队列之后，再把hashmap释放掉
int topk(){
  int ret = SUCCESS;
  transform(UrlMap.begin(), UrlMap.end(), std::inserter(tmp, tmp.begin()),
                 [](std::pair<string, int> a) { return std::pair<int, string>(a.second, a.first);});
  multimap<int, string>::iterator start = tmp.begin();
  for(int i = 0; i< K; i++){   //K次
    TopK.push(make_pair(start->first, start->second));
    if(TopK.size() > K){
      TopK.pop();
    }
    start++;
  }
  UrlMap.clear();
  tmp.clear();
  return ret;
}

//消费者
void consume(){
  int ret = SUCCESS;
  int index = -1;
  
  do{
    if(SUCCESS != (ret = convert())){
      cout<<"convert string to hashmap error"<<'\n';
    }
    else if(SUCCESS != (ret = topk())){
      cout<<"topk error"<<'\n';
    }
//    else{
//      while(!TopK.empty()){
//        cout<<TopK.top().first<<" "<<TopK.top().second<<endl;   //调试时用于输出每一次的topK
//        TopK.pop();
//      }
//      cout<<"**************"<<endl;
//    }
  }while((index = chooseConsumeDeque()) != -1);
  
  while(!TopK.empty()){
    cout<<TopK.top().first<<" "<<TopK.top().second<<endl;   //输出topk元素
    TopK.pop();
  }
}

int main(int argc, const char * argv[]) {
  int ret = SUCCESS;
  thread th(consume);
  string file_name = "./url.txt";
  if(SUCCESS != (ret = readUrl(file_name))){
    cout<<"read url from file error"<<'\n';
  }
  th.join();
  return ret;
}
