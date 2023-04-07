#pragma once
#include <string>

class Message
{
	size_t sender_id;
	size_t recepient_id;
	std::string text;
	std::string time; //"hh.mm.ss dd.MM.yy"
};

