#pragma once
#include <string>
class User
{
	std::string login;
	std::string pass;
	User(std::string _login, std::string _pass):login(_login),pass(_pass){}
};

