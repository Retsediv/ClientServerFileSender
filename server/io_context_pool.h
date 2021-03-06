#ifndef EZLOTESTTASK_IO_CONTEXT_POOL_H
#define EZLOTESTTASK_IO_CONTEXT_POOL_H


#include <boost/asio.hpp>
#include <list>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

class io_context_pool : private boost::noncopyable {
public:
    /// Construct the io_context pool.
    explicit io_context_pool(std::size_t pool_size);

    /// Run all io_context objects in the pool.
    void run();

    /// Stop all io_context objects in the pool.
    void stop();

    /// Get an io_context to use.
    boost::asio::io_context &get_io_context();

private:
    typedef boost::shared_ptr<boost::asio::io_context> io_context_ptr;
    typedef boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type> io_context_work;

    /// The pool of io_contexts.
    std::vector<io_context_ptr> io_contexts_;

    /// The work that keeps the io_contexts running.
    std::list<io_context_work> work_;

    /// The next io_context to use for a connection.
    std::size_t next_io_context_;
};


#endif