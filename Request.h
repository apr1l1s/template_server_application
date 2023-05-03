#pragma once
#include <vector>
#include <string>
#include <boost/asio.hpp> 
#include <boost/beast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <boost/core/detail/string_view.hpp>
#include <boost/beast/core/string_type.hpp>
#include <map>


using boost::beast::http::parser;
using boost::beast::http::string_body;
using boost::beast::http::verb;
using boost::asio::buffer;

class Request
{
    verb method;
    std::string body;
    std::string method_string;
    std::map<std::string,std::string> params;
public:
    std::string get_body() {
        if (body.empty()) return "";
        else return body;
    }

    std::pair<std::string, std::string> get_path() {
        boost::string_view s = method_string;
        auto answer = std::pair<std::string, std::string>("", "");
        //Если строка не пустая
        if (method_string.empty()) {
            return answer;
        }
        else {
            size_t pos = s.find('/');
            //Если в пути есть /
            if (pos == -1) {
                std::cout << "Empty path\n\n";
                return answer;
            }
            else {
                std::string token = "";
                std::string path = s.substr(pos - 1, s.length() - pos).data();
                //Если есть ?, то есть параметр
                if (s.find("?") != -1) {
                    // /msg?token=123456789012345
                    boost::string_view base_token = "?token=123456789012345";
                    token = s.substr(s.find('=') + 1, strlen("Jzr0O1lK8BTSAP8")).data();
                    path = path.substr(0, path.length() - strlen("?token=Jzr0O1lK8BTSAP8"));
                }
                return std::pair<std::string, std::string>(path, token);
            }
        }
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
