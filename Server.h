#pragma once
#include <string>
#include <map>
#include "Database.h"
#include "Reply.h"
#include "Request.h"
#include "User.h"
#include <nlohmann/json.hpp>
#include "ErrorHandler.h"

class Server
{
private:
	Database base = Database();
	std::string active_admin_token;
	std::map<std::string,size_t> active_users_list;
public:
	void work();
	int server_port = 10001;
	std::string generate_token(int l);
	Reply parse_request(Request req) {
		bool is_body_not_empty = !req.get_body().empty();
		bool is_body_parceble;
		try {
			if (is_body_not_empty) {
				auto j = nlohmann::json::parse(req.get_body());
				is_body_parceble = true;
			}
			else is_body_parceble = false;
			
		}
		catch(int _){
			is_body_parceble = false;
		}


		std::string path = req.get_path().first;
		std::string token = req.get_path().second;
		bool is_user = !token.empty() && active_users_list.find(token)!=active_users_list.end();
		bool is_admin = !token.empty() && !active_admin_token.empty() && token == active_admin_token;
		Reply rep = Reply(ErrorHandler::internal_server_error);
		switch (req.get_method()) {
		case verb::options:
			//Cоблюдение CORS
			rep = OPTIONS();
			break;
		case verb::get:
			//Получить сообщение 
			if (is_user) {
				rep = GET(req);
			}
			break;
		case verb::post:
			//Отправить сообщение, регистрация
			if (is_body_parceble) {
				rep = POST(req, is_admin, is_user);
			}
			else {
				return Reply(ErrorHandler::StatusType::bad_request);
			}
			break;
		case verb::put:
			//Отредактировать сообщение
			if (is_body_parceble && is_user) {
				rep = PUT();
			}
			break;
		case verb::delete_:
			//Удалить сообщение
			if (is_admin) rep = DELETE_(req);
			break;
		default:
			ErrorHandler::show_error(ErrorHandler::bad_request, "Unknow method\n\n");
			break;
		}
		if (rep.status == ErrorHandler::internal_server_error) {
			return Reply(ErrorHandler::StatusType::bad_request);
		}
		else {
			return rep;
		}
	}
	Reply OPTIONS();
	Reply GET(Request req);
	Reply POST(Request req, bool is_admin, bool is_user);
	Reply PUT();
	Reply DELETE_(Request req);
};

