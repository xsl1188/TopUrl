//
//  common.h
//  TopUrl100
//

#ifndef common_h
#define common_h
#include <unordered_map>
#include <queue>
#include <map>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <memory.h>
using namespace std;

#define N 10000   //一次读取url数目，可手动修改
#define K 100      //topk设置为too100
#define QLEN 1    //设置缓冲队列数目，目前数目为1，设置成pipeline设计
#define SUCCESS 0   //成功码
#define ERROR 1      //错误码
using HashUrlMap = unordered_map<string, int>;
using UrlPair = pair<int, string>;

#endif /* common_h */
