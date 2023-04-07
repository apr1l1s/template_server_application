#include "Server.h"

int main(){
	auto s = Server();
	while (true) {
		s.work();
	}
}