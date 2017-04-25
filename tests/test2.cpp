#include <iostream>
#include <cstring>
#include "../src/chan.h"

using namespace std;

Chan<char*> chan;
char ch[]="Hello World";

void sendFun() {
	char *pp = ch;
	chan.send(&pp);
}
void recvFun() {
	char *ch;
	chan.recv(&ch);
	printf("%s\n", ch);
}

int main () {
	thread t(sendFun);
	thread w(recvFun);
	t.join();
	w.join();
}
