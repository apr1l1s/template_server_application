#pragma once
#include <string>
#include <iostream>
class ErrorHandler
{
public:
	enum StatusType {
		ok = 200,
		accepted = 202,
		no_content = 204,
		bad_request = 400,
		unauthorized = 401,
		not_found = 404,
		internal_server_error = 500,
	};
	//Выводит в консоль статус ошибки и причину
	static void show_error(StatusType e, std::string text) {
		switch (e) {
		case no_content:
			std::cout << "204 No Content:\n" << text << "\n\n"; 
			break;
		case bad_request:
			std::cout << "400 No Content:\n" << text << "\n\n"; 
			break;
		case unauthorized:
			std::cout << "401 No Content:\n" << text << "\n\n"; 
			break;
		case not_found:
			std::cout << "404 No Content:\n" << text << "\n\n"; 
			break;
		case internal_server_error:
			std::cout << "500 No Content:\n" << text << "\n\n";
			break;
		default:break;
		}
	}
};

