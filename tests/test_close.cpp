#include <iostream>
#include <cstring>
#include <string>
#include "../src/chan.h"

using namespace std;

long long cnt = 100000, sum = 0, num = 10, cur_num = 0;
//int cap = 100; // for buffered
int cap = 0; // for unbuffered
Chan<int> chan(cap);
mutex m;
void sendFun() {
	long long sum = 0;
	for (int i = 0; i < cnt; i++) {
		chan.send(i); 
		sum += i;
	}
	unique_lock<mutex> lk(m);
	printf("send over\n");
	cur_num++;
}
void recvFun() {
	sleep(2);
	long long sum = 0;
	for (int i = 0; i < cnt; i++) {
		int x;
		chan.recv(x);
		sum += x;
	}
	unique_lock<mutex> lk(m);
	printf("recv over\n");
	cur_num++;
}

void test_close() {
	for (int i = 0; i < num; i++) {
		thread t(sendFun);
		t.detach();
	}
	for (int i = 0; i < num; i++) {
		thread t(recvFun);
		t.detach();
	}
	sleep(2);
	chan.close();
	chan.close();
	while (num*2 != cur_num) {
		printf("waiting %lld\n", cur_num);
		sleep(1);
	}
	printf("waiting %lld\n", cur_num);
}

int main () {
	test_close();
}
