#include <iostream>
#include <thread>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include <set>
#include <queue>

using namespace std;

class Chan {
	mutex _r_mutex, _w_mutex, _inter_mutex;
	condition_variable _cond;
	bool _buffered;
	int _pipe[2];
	int _send_num, _recv_num;

	Chan (): _buffered(false), _send_num(0), _recv_num(0) {
		if (pipe(_pipe) == -1) {
			printf("pipe error\n");
		}
	}

	int send(void *data) {
		unique_lock<mutex> w_lk(_w_mutex);
		if (!_buffered) {
			return unbuffered_send(data);
		}
	}
	int recv(void *data) {
		unique_lock<mutex> r_lk(_r_mutex);
		if (!_buffered) {
			return unbuffered_recv(data);
		}
	}

	int unbuffered_recv(void *data) {
		{
			unique_lock<mutex> _lk(_inter_mutex);
			_recv_num++;
			_cond.notify_one();
		}
		return read(_pipe[0], data, sizeof(void*)) > 0? 0: -1;
	}
	int unbuffered_send(void *data) {
		unique_lock<mutex> _lk(_inter_mutex);
		_send_num++;
		_cond.wait(_lk, [this]{return this->_recv_num > 0;});
		_send_num--;
		_recv_num--;
		return write(_pipe[1], data, sizeof(void*)) > 0? 0: -1;
	}

};

int main () {

}
