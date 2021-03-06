#include "io_context_pool.h"

#include "server.h"
#include <boost/thread/thread.hpp>

io_context_pool::io_context_pool(std::size_t pool_size) : next_io_context_(0) {
    if (pool_size == 0)
        throw std::runtime_error("io_context_pool size is 0");

    // Give all the io_contexts work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    for (std::size_t i = 0; i < pool_size; ++i) {
        io_context_ptr io_context(new boost::asio::io_context);
        io_contexts_.push_back(io_context);
        work_.push_back(boost::asio::make_work_guard(*io_context));
    }
}

void io_context_pool::run() {
    // Create a pool of threads to run all of the io_contexts.
    std::vector<boost::shared_ptr<boost::thread> > threads;
    for (std::size_t i = 0; i < io_contexts_.size(); ++i) {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
                boost::bind(&boost::asio::io_context::run, io_contexts_[i])));
        threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit
    for(const auto &t : threads)
        t->join();
}

void io_context_pool::stop() {
    // Explicitly stop all io_contexts.
    for(const auto &io_c : io_contexts_)
        io_c->stop();
}

boost::asio::io_context &io_context_pool::get_io_context() {
    // Use a round-robin scheme to choose the next io_context to use.
    boost::asio::io_context &io_context = *io_contexts_[next_io_context_];
    ++next_io_context_;
    if (next_io_context_ == io_contexts_.size())
        next_io_context_ = 0;
    return io_context;
}