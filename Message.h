#pragma once
#include <string>

class Message
{
public:
	size_t sender_id;
	size_t recepient_id;
	std::string text;
	std::string time; //"hh.mm.ss dd.MM.yy"
	Message(size_t _sender_id, size_t _recepient_id, std::string text, std::string time) {}
};

