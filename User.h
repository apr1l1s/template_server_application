#pragma once
#include <string>
class User
{
public:
	std::string login;
	std::string pass;
	User(std::string _login, std::string _pass):login(_login),pass(_pass){}
};

