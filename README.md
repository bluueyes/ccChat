"# ccChat" 

#### 介绍
这是一个由 C++实现的仿微信聊天项目，用到了grpc,并发线程，网络编程，qt开发，数据库等多种技术综合应用

## 功能特性
1.gRPC使用Protocol Buffers作为数据格式，可以在不同的平台上进行应用程序之间的通信
2.利用boost的asio库实现tcp通信服务
3.采用Qt6.8实现聊天客户端对微信的仿照,实现了聊天，添加好友，发送信息等功能
4.利用gRPC实现服务器通信，搭建了分布式服务器

## 依赖
C++编译器，支持C++11或更高版本。
配置:Boost,json,grpc,asio,redis,mysql等
可根据项目的ini文件自行配置相关服务的ip和端口。

#启动
用visual studio打开服务器sln项目
用Qt6及以上打开ChatUI的pro文件