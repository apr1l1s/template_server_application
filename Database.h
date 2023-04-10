#pragma once
#include "User.h"
#include "Message.h"
#include "Table.h"

class Database
{
public:
	Table<User> users;
	Table<Message> messages;
	Database() {
		User u("admin", "admin");
		users.add(u);
	}
	//Возвращает айди по логину и паролю
	size_t find(User u) {
		auto data = users.getlist();
		std::string login = u.login;
		std::string pass = u.pass;
		for (auto it = data.begin(); it != data.end(); ++it) {
			if (it->second.login == login && it->second.pass == pass) {
				return it->first;
			}
		}
		return -1;
	}
};	

