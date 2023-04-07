#pragma once
#include <string>
#include <map>
#include "Database.h"
#include "Reply.h"
#include "Request.h"

class Server
{
private:
	Database base;
	std::string active_admin_token;
	std::map<size_t,std::string> active_users_list;
public:
	void work();
	int server_port = 10001;
	std::string generate_token(int l);
	void parse_request(Request req) {
		bool is_user;
		bool is_admin;
		bool is_body_not_empty = !req.get_body().empty();
		bool is_body_parceble;
		std::string path = req.get_path().first;
		std::string token = req.get_path().second;
		switch (req.get_method()) {
		case verb::options:
			OPTIONS();
			break;
		case verb::get:
			GET(); 
			break;
		case verb::post:
			POST();
			break;
		case verb::put:
			PUT(); 
			break;
		case verb::delete_:
			DELETE_(); 
			break;
		default:
			std::cout << "Unknow method\n\n";
			break;
		}
	}
	void OPTIONS();
	void GET();
	void POST();
	void PUT();
	void DELETE_();
	
};

