#pragma once
#include "User.h"
#include "Message.h"
#include "Table.h"

class Database
{
public:
	Table<User> users;
	Table<Message> messages;
	
};
