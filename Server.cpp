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
    return Reply(ErrorHandler::ok);
}
//Получить сообщение
//Получить юзеров
Reply Server::GET(Request req)
{
    auto path = req.get_path().first;
    auto token = req.get_path().second;
    if (path.find("/msg") != -1) {
        std::cout << "User trying to get message with token: " << token << "\n";
        size_t id = active_users_list[token];
        std::cout << "User trying to get message with id: " << id << "\n";
        auto data = base.messages.getlist();
        nlohmann::json list;
        nlohmann::json item;
        
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->second.recepient_id == id || it->second.sender_id == id) {
                item["message_id"] = it->first;
                item["sender_id"] = it->second.sender_id;
                item["recepient_id"] = it->second.recepient_id;
                item["text"] = it->second.text;
                list["list"] += item;
            } 
        }
        std::cout << "Transfering items: " << list.size() << "\n\n";
        Reply rep(ErrorHandler::ok, list.dump());
        return rep;
    }
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

    //Вход всегда без токена
    //Если айди полученный по токену это -1, то нет пользователя
    //Если айди 1, то это админ и генерируется новый админский токен
    //Если 2 и больше, то гененируется юзерский токен
    if (path.find("/login")!= -1) {
        auto data = base.users.getlist();
        bool is_json_user = !(body["login"].is_null() || body["pass"].is_null());
        //Если в теле json пользователя, то дальше
        if (is_json_user) {
            User u(body["login"].get<std::string>(), body["pass"].get<std::string>());
            size_t id = base.find(u);
            auto new_token = generate_token(15);
            switch (id) {
            case -1:
                std::cout << "Not found user with this data\n\n";
                return Reply(ErrorHandler::not_found);
                break;
            case 1:
                std::cout << "New admin token is: " << new_token << "\n";
                active_admin_token = new_token;
                std::cout << "Active_Users: " << active_users_list.size() << "\n";
                std::cout << "Admin_Token: " << active_admin_token << "\n\n";
                break;
            default:
                std::cout << "New user token is: " << new_token << "\n";
                active_users_list.insert(std::pair<std::string, size_t>(new_token, id));
                std::cout << "Active_Users: " << active_users_list.size() << "\n";
                std::cout << "Admin_Token: " << active_admin_token << "\n\n";
                break;
            }
            nlohmann::json j;
            j["token"] = new_token;
            Reply rep(ErrorHandler::ok, j.dump());
            return rep;
        }
    }
    
    //Регистрация это добавление пользователя с админским токеном
    //Если всё успешно, то прога вернёт айди нового пользователя
    if (path.find("/regin") != -1) {
        std::cout << "Admin trying to add new user with token: " << token << "\n";
        if (is_admin) {
            auto data = base.users.getlist();
            std::cout << "Admin trying to add new user\n";
            nlohmann::json item = nlohmann::json::parse(req.get_body());
            bool is_body_user = !item["login"].empty() && !item["pass"].empty();
            if (!is_body_user) {
                std::cout << "Request does not contains item\n";
                Reply rep(ErrorHandler::bad_request);
                return rep;
            }
            else {
                std::string login = item["login"].get<std::string>();
                std::string pass = item["pass"].get<std::string>();
                User user(login, pass);
                base.users.add(user);
                size_t id = -1;
                auto data = base.users.getlist();
                for (auto it = data.begin(); it != data.end(); ++it) {
                    id = it->first;
                }
                std::cout << "New data: " << login << ", " << pass << "\n";
                nlohmann::json user_id;
                user_id["user_id"] = id;

                Reply rep(ErrorHandler::ok, user_id.dump());
                return rep;
            }
        }
        else {
            std::cout << "Token is outdated\n\n";
            return Reply(ErrorHandler::unauthorized);
        }
        
    }   
    if (path.find("/msg/") != -1) {
        //Токен с которым отправлен запрос это отправитель
        //айди который передали это айди получателя
        //тело состоит из json в котором text и дата отправки
        std::string s = req.get_path().first;
        size_t pos = strlen("/msg/") + 1;
        size_t recepient_id = atoi(s.substr(pos, strlen(s.data()) - pos).data());
        size_t sender_id = active_users_list[token];
        std::cout << s.substr(pos, strlen(s.data()) - pos).data() << "\n\n";
        if (sender_id == recepient_id) {
            std::cout << "User trying to send message to hifself\n\n";
            return Reply(ErrorHandler::bad_request);
        }
        std::cout << sender_id  << " trying to send message to " << recepient_id << '\n';
        std::time_t localtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (body["text"].empty()) {
            return Reply(ErrorHandler::bad_request);
        }
        std::string text = body["text"].get<std::string>();
        char date[50];
        struct tm timeinfo;
        localtime_s(&timeinfo, &localtime);
        strftime(date, sizeof date, "%d.%m.%Y, %H:%M:%S", &timeinfo);
        std::string d = date;
        std::cout << sender_id << "\n" << recepient_id << "\n" << text << "\n" << date << "\n\n";
        Message msg(sender_id, recepient_id, text, date);
        base.messages.add(msg);
        Reply rep(ErrorHandler::accepted,"");
        return rep;
    }
    return Reply(ErrorHandler::internal_server_error);

}

//Редактировать сообщение может пользователь
//
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
            std::cout << "User trying to delete message: " << id << '\n';
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
