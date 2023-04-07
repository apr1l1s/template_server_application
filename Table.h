#pragma once
#include <string>
#include <map>

template<typename T>
class Table
{
private:
	size_t id;
	std::map<size_t, T> data;
public:
	Table() :id(0){}
	void add(T _data);
	void del(size_t id);
	void del(T _data);
	void edit(size_t id, T _data);
	std::map<size_t, T> getlist() { return data; }
};

