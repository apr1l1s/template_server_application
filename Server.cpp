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
        Reply res = parse_request(req);
        size_t bytes = boost::asio::write(socket_, boost::asio::buffer(res.to_string()), ec);

    }
    catch (ErrorHandler::StatusType e) {
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

Reply Server::OPTIONS()
{
    return Reply(ErrorHandler::internal_server_error);
}

Reply Server::GET()
{
    return Reply(ErrorHandler::internal_server_error);
}

//Регистрация админа
//Регистрация пользователя (токен админа)
//Создание пользователя (токен админа)
//Отправить сообщение (токен юзера)
Reply Server::POST(Request req,bool is_admin, bool is_user)
{
    auto body =nlohmann::json::parse( req.get_body());
    auto path = req.get_path().first;
    auto token = req.get_path().second;
    if (path.find("/login")!= -1) {

        auto data = base.users.getlist();
        size_t id = -1;

        bool is_json_user = !(body["login"].is_null() || body["pass"].is_null());

        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->second.login == body["login"].get<std::string>() && it->second.pass == body["pass"].get<std::string>()) {
                id = it->first;
            }
        }
        auto new_token = generate_token(15);
        switch (id) {
        case -1:
            //Нет в бд
            break;
        case 1:
            active_admin_token = new_token;
        default:
            active_users_list.insert(std::pair<std::string, size_t>(new_token, id));
        }

        nlohmann::json j;
        j["token"] = new_token;
        ErrorHandler::show_error(ErrorHandler::ok,"Active token is now: " + new_token);
        Reply rep(ErrorHandler::ok, j.dump());
        return rep;
    }
    if (path.find("/regin") != -1) {
        auto data = base.users.getlist();
        std::cout << "Admin trying to add new user\n";
        nlohmann::json item = nlohmann::json::parse(req.get_body());
        bool is_body_user = !item["login"].empty() && !item["pass"].empty();
        if (is_body_user) {
            std::cout << "Request does not contains item\n";
            Reply rep(ErrorHandler::bad_request);
            return rep;
        }
        else {
            std::string login = item["login"].get<std::string>();
            std::string pass = item["pass"].get<std::string>();
            User user(login,pass);
            base.users.add(user);
            std::cout << "New data: " << login << ", " << pass << "\n";
            Reply rep(ErrorHandler::accepted, "");
            return rep;
        }
    }    
    return Reply(ErrorHandler::internal_server_error);
}
//Редактировать сообщение
Reply Server::PUT()
{
    return Reply(ErrorHandler::internal_server_error);
}
//Удаление сообщений
//Удаление всех сообщений
Reply Server::DELETE_(Request req)
{
    std::string s = req.get_path().first;
    if (true) {
        if (s.find("/msg/") != -1) {
            size_t pos = strlen("/msg/") + 1;
            size_t id = atoi(s.substr(pos, strlen(s.data()) - pos).data());
            std::cout << s.substr(pos, strlen(s.data()) - pos).data() << "\n\n";
            std::cout << "User trying to delete post: " << id << '\n';
            bool answ = base.messages.del(id);
            if (answ) {
                std::cout << "Success!\n\n";
                Reply rep(ErrorHandler::accepted);
                return rep;
            }
            else {
                std::cout << "Post with this id does not exists!\n\n";
                Reply rep(ErrorHandler::not_found);
                return rep;
            }
        }
        else {
            if (s.find("/msg") != -1) {
                auto data = base.messages.getlist();
                for (auto it = data.begin(); it != data.end(); ++it) {
                    base.messages.del(it->first);
                }
            }
        }
        return Reply(ErrorHandler::bad_request);
    }
    else {
        Reply rep(ErrorHandler::unauthorized);
        return rep;
    }
}
