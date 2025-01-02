#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size):_works(size),_ioService(size)
{
    for (std::size_t i = 0;i < size; i++) {
        _works[i] = std::unique_ptr<Work>(new Work(_ioService[i]));
    }

    //遍历多个ioservice,创建多线程，每个线程启动一个ioService
    for (std::size_t i = 0;i < size;i++) {
        _threads.emplace_back([this, i]() {
            _ioService[i].run();
            });
    }
}

AsioIOServicePool::~AsioIOServicePool()
{
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
    auto& service = _ioService[_nextIOService++];
    if (_nextIOService == _ioService.size()) {
        _nextIOService = 0;
    }
    return service;
}

void AsioIOServicePool::Stop()
{
    //因为仅仅执行work.reset并不能让iocontext从run的状态中退出
    //当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务

    for (auto& work : _works) {
        work->get_io_context().stop();
        work.reset();
    }

    for (auto& t : _threads) {
        t.join();
    }
}


