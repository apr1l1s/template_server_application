#pragma once
#include <vector>
#include <string>
#include <boost/asio.hpp> 
#include <boost/beast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <boost/core/detail/string_view.hpp>
#include <boost/beast/core/string_type.hpp>


using boost::beast::http::parser;
using boost::beast::http::string_body;
using boost::beast::http::verb;
using boost::asio::buffer;

class Request
{
    verb method;
    std::string body;
    std::string method_string;
public:
    std::string get_body() {
        if (body.empty()) return "";
        else return body;
    }
    std::pair<std::string, std::string> get_path() {
        boost::string_view s = method_string;
        size_t pos = s.find('/');
        std::string token = "";
        std::string path = s.substr(pos - 1, s.length() - pos).data();
        if (s.find("?") != -1) {
            boost::string_view base_token = "?token=Jzr0O1lK8BTSAP8";

            token = s.substr(s.find('=') + 1, strlen("Jzr0O1lK8BTSAP8")).data();
            path = path.substr(0, path.length() - strlen("?token=Jzr0O1lK8BTSAP8"));
        }
        return std::pair<std::string, std::string>(path, token);
    }
    verb get_method() {
        return method;
    }
    Request(std::string _request) {
        boost::system::error_code ec;
        boost::string_view emulated_stream = _request;
        while (not emulated_stream.empty()) {
            parser<true, string_body> response_parser;
            while (not (ec or response_parser.is_done() or emulated_stream.empty()))
            {
                auto buf = buffer(emulated_stream.data(), emulated_stream.size());
                auto consumed = response_parser.put(buf, ec);
                if (ec == boost::beast::http::error::need_more) ec.clear();
                emulated_stream.remove_prefix(consumed);
            }
            auto res = response_parser.release();

            method = res.method();
            method_string = _request.substr(0, _request.find(" HTTP"));
            body = res.body();
        }
    }
};
