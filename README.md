# TopUrl
从100GB文件中选取前100条，并且内存只有1GB。demo版本。

前提条件：
假设一条url最长有128个字符（大小128B），每个url为一行（也可改为以分号分割），100GB文件大概有9亿条url。
由于内存限制1GB，设置一次读取90w条url，大概需要读取1000次左右。
（暂时不考虑多文件情况，如果是多文件，则对文件读写处设计为多线程读取文件，存放在无锁队列中）

鉴于基础组件无锁循环队列、并行hashmap需要自己手动改写；所以修改策略，该用STL基本容器设计pipeline方法。

总体思想：
生产者线程：读文件并加入到队列中，全部加入之后，唤醒消费者线程
消费者线程：从队列中取元素，转换成hashmap，并且清空队列；当队列为空时，唤醒生产者线程，并且消费者线程继续做topk的选取。
备注：目前生产者线程和消费者线程设置为1:1，可根据两者操作时间差设置比例。

开发环境：
Xcode9.2

单元测试文件
url.txt
