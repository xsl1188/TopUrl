//
//  consumer.cpp
//  TopUrl100
//

#include "consumer.h"

/*
 消费者部分逻辑
 */
//选择可以消费的队列
int Consumer::chooseConsumeDeque(Producer* pro){
  int index = -1;
  for(int i = 0; i < QLEN; i++){
    if(!pro->getUrlQueue()[i].empty())
      index = i;
  }
  return index;
}

//将队列中元素逐步转换成hashmap中，并逐步清空队列，使得生产者可以操作
int Consumer::convert(Producer* pro){
  int ret = SUCCESS;
  string str;
  int index = -1;
  while((index = chooseConsumeDeque(pro)) == -1){
    pthread_mutex_lock(&mut2);
    pthread_cond_wait(&cond2, &mut2);
    pthread_mutex_unlock(&mut2);
  }
  vector<string> &vec = pro->getUrlQueue()[index];
  while(!vec.empty()){
    str = vec.back();
    cout<<str<<'\n';
    if(UrlMap.count(str) > 0){
      UrlMap[str]++;
    }
    else{
      UrlMap.emplace(str, 1);
    }
    vec.pop_back();
  }
  pthread_cond_broadcast(&Producer::cond1);
  return ret;
}

//选取topk元素，每一次先将hashmap中的元素转换成multimap中，根据频数排序，然后选择前k个元素；
//并且以堆的方式进行控制数目，每次转换到TopK优先队列之后，再把hashmap释放掉
int Consumer::topk(){
  multimap<int, string, greater<int>> tmp;
  int ret = SUCCESS;
  for(auto it : UrlMap){
    tmp.emplace(it.second, it.first);
  }
  //  transform(UrlMap.begin(), UrlMap.end(), std::inserter(tmp, tmp.begin()),
  //            [](std::pair<string, int> a) { return std::pair<int, string>(a.second, a.first);});
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


void Consumer::consume(Producer* pro){
  int ret = SUCCESS;
  int index = -1;
  
  do{
    if(SUCCESS != (ret = convert(pro))){
      cout<<"convert string to hashmap error"<<'\n';
    }
    else if(SUCCESS != (ret = topk())){
      cout<<"topk error"<<'\n';
    }
    else{
      //      while(!TopK.empty()){
      //        cout<<TopK.top().first<<" "<<TopK.top().second<<endl;   //调试时用于输出每一次的topK
      //        TopK.pop();
      //      }
      //      cout<<"**************"<<endl;
    }
  }while(!(pro->getEnd()) && (index = chooseConsumeDeque(pro)) != -1);
  
  while(!TopK.empty()){
    cout<<TopK.top().first<<" "<<TopK.top().second<<endl;   //输出topk元素
    TopK.pop();
  }
}

thread Consumer::getConsumerThread(Producer& pro){
  return thread(&Consumer::consume, this, &pro);
}


