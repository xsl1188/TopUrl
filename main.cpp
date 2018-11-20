//
//  main.cpp
//  TopUrl100
//

#include "consumer.h"
int main(int argc, const char * argv[]) {
  int ret = SUCCESS;
  Consumer con;
  Producer pro;
  thread th = thread(&Consumer::consume, &con, &pro);
//  thread th = con.getConsumerThread(pro);
  string file_name = "/Users/shilei/Desktop/url.txt";
  if(SUCCESS != (ret = pro.produce(file_name))){
    cout<<"read url from file error"<<'\n';
  }
  th.join();
  return ret;
}
