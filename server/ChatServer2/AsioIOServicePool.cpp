#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size):_works(size),_ioService(size)
{
    for (std::size_t i = 0;i < size; i++) {
        _works[i] = std::unique_ptr<Work>(new Work(_ioService[i]));
    }

    //�������ioservice,�������̣߳�ÿ���߳�����һ��ioService
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
    //��Ϊ����ִ��work.reset��������iocontext��run��״̬���˳�
    //��iocontext�Ѿ����˶���д�ļ����¼��󣬻���Ҫ�ֶ�stop�÷���

    for (auto& work : _works) {
        work->get_io_context().stop();
        work.reset();
    }

    for (auto& t : _threads) {
        t.join();
    }
}


