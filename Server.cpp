#include "Server.h"
#include <boost/asio.hpp> 
#include <boost/beast.hpp>
#include <iostream>
#include "ErrorHandler.h"
#include "Request.h"
#include "Reply.h"
void Server::work()
{
    try {
        boost::asio::io_service io_service_;
        boost::asio::ip::tcp::endpoint end_point_(boost::asio::ip::tcp::v4(), server_port);
        boost::asio::ip::tcp::acceptor acceptor_(io_service_, end_point_);
        boost::asio::ip::tcp::socket socket_(io_service_);
        std::cout << "Listening on\n\n";
        acceptor_.accept(socket_);
        std::cout << "Accept new request:\n";
        boost::asio::streambuf buf;
        boost::system::error_code ec;
        size_t max_socket_size = buf.max_size();
        boost::asio::read_until(socket_, buf, "\r\n\r\n", ec);
        ec.clear();
        std::string request_string = boost::beast::buffers_to_string(buf.data());
        Request req(request_string);
        //Reply res = parse_request(req);
        //size_t bytes = boost::asio::write(socket_, boost::asio::buffer(res.to_string()), ec);

    }
    catch (ErrorHandler::ErrorType e) {
        ErrorHandler::show_error(e,"");
    }
}
std::string Server::generate_token(int l) {
    srand(time(NULL));
    char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(l);
    for (int i = 0; i < l; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}
