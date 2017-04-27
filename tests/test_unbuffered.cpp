#include <iostream>
#include "../src/chan.h"

using namespace std;

int round = 10, cnt = 100000, tot = 0, over = 0;
Chan<int> ch;
mutex _m;

void sendFun() {
	int sum = 0;
	for (int i = 1; i <= cnt; i++) {
		if (-1 == ch.send(i)) printf("error\n");
		sum += i;
	}
	unique_lock<mutex> lk(_m);
	tot -= sum;
	over++;
}
void recvFun() {
	int sum = 0;
	for (int i = 1; i <= cnt; i++) {
		int x;
		if (-1 == ch.recv(x)) printf("error\n");
		sum += x;
	}
	unique_lock<mutex> lk(_m);
	tot += sum;
	over++;
}

int main () {
	for (int i = 0; i < round; i++) {
		thread t(sendFun);
		t.detach();
		thread w(recvFun);
		w.detach();
	}
	while (over != round*2) {
		printf("waiting: %d %d\n", over, tot);
		sleep(1);
	}
	printf("Over: %d %d\n", over, tot);
}
