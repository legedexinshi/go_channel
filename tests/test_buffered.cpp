#include <iostream>
#include <cstring>
#include <string>
#include "../src/chan.h"

using namespace std;

void test_basic() {
	Chan<string> ch(2);
	string str1 = "hello";
	ch.send(str1);
	string str2 = "world";
	ch.send(str2);

	string str3;
	ch.recv(str3);
	cout << str3 << endl;
	ch.recv(str3);
	cout << str3 << endl;
}

long long cnt = 100000, sum = 0, num = 10, cur_num = 0, cap = 100;
Chan<int> chan(cap);
mutex m;
void sendFun() {
	long long sum = 0;
	for (int i = 0; i < cnt; i++) {
		chan.send(i); 
		sum += i;
	}
	unique_lock<mutex> lk(m);
	::sum += sum;
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
	::sum -= sum;
	cur_num++;
}

void test_concurrent() {
	for (int i = 0; i < num; i++) {
		thread t(sendFun);
		t.detach();
	}
	for (int i = 0; i < num; i++) {
		thread t(recvFun);
		t.detach();
	}
	while (cur_num != num*2) {
		printf("sleep %lld %lld\n", cur_num, sum);
		sleep(1);
	}
	printf("sleep %lld %lld\n", cur_num, sum);
}

int main () {
	test_basic();
	sleep(1);
	test_concurrent();
}
