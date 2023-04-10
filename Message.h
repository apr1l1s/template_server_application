#pragma once
#include <string>

class Message
{
public:
	size_t sender_id;
	size_t recepient_id;
	std::string text;
	std::string time; //"hh:mm:ss dd.MM.yyyy"
	Message(size_t _sender_id, size_t _recepient_id, std::string _text, std::string _time)
	:sender_id(_sender_id),recepient_id(_recepient_id),text(_text),time(_time){}
};
