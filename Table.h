#pragma once
#include <map>

template<typename T>
class Table
{
private:
	size_t id;
	std::map<size_t, T> data;
public:
	Table() :id(0) {}
	void add(T _data) {
		data.insert(std::pair<size_t, T>(++id, _data));
	}
	std::map<size_t, T> getlist() { return data; }
	bool del(size_t id) { 
		bool answ = false;
		for (auto it = data.begin(); it != data.end(); ++it) {
			if (it->first == id) {
				data.erase(id);
				answ = true;
			}
		}
		return answ;
		
	}
	void edit(size_t id, T item) {
		data.erase(id);
		data.insert(std::pair<size_t, T>(id, item));
	}
};

