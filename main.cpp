#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>

using boost::asio::ip::tcp;

std::string make_date_time_string() {
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}


class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    using tcp_connection_ptr = std::shared_ptr<tcp_connection>;
    static tcp_connection_ptr create(boost::asio::io_context &io_context) {
        return tcp_connection_ptr(new tcp_connection(io_context));
    }

    tcp::socket& socket() {
        return _socket;
    }

    void start() {
        _message = make_date_time_string();
        boost::asio::async_write(_socket, boost::asio::buffer(_message),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)
         );
    }

private:

    explicit tcp_connection(boost::asio::io_context &io_context) : _socket(io_context) {
    }

    void handle_write(const boost::system::error_code& /*error*/,
                      size_t /*bytes_transferred*/)
    {
    }

    tcp::socket _socket;
    std::string _message;

};

class tcp_server {
public:
    tcp_server(boost::asio::io_context &io_context) :
        _io_context(io_context),
        _acceptor(io_context, tcp::endpoint(tcp::v4(), 13)),
        _workers(3) {
        start_accept();
    }


private:
    void start_accept() {
        auto new_connection = tcp_connection::create(_io_context);
        _acceptor.async_accept(new_connection->socket(),
                boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error)
        );
    }

    void handle_accept(tcp_connection::tcp_connection_ptr new_connection, const boost::system::error_code& error) {
        if (!error) {
            boost::asio::post(_workers, [new_connection]() {
                new_connection->start();
            });
            new_connection->start();
        }
        start_accept();
    }

    boost::asio::io_context& _io_context;
    tcp::acceptor _acceptor;
    boost::asio::thread_pool _workers;
};

int main() {
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}