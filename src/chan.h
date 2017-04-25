#include <thread>
#include <cstdio>
#include <unistd.h>

using namespace std;

template<typename T>
class Chan {
	mutex _r_mutex, _w_mutex, _inter_mutex;
	condition_variable _cond;
	bool _buffered;
	int _send_num, _recv_num;
	int _pipe[2];

 public:
	Chan (): _buffered(false), _send_num(0), _recv_num(0) {
		if (pipe(_pipe) == -1) {
			printf("pipe error\n");
		}
	}

	int send(T *data) {
		unique_lock<mutex> w_lk(_w_mutex);
		if (!_buffered) {
			return unbuffered_send(data);
		}
		return -1;
	}
	int recv(T *data) {
		unique_lock<mutex> r_lk(_r_mutex);
		if (!_buffered) {
			return unbuffered_recv(data);
		}
		return -1;
	}

 private:
	int unbuffered_recv(T *data) {
		{
			unique_lock<mutex> lk(_inter_mutex);
			_recv_num++;
		}
		_cond.notify_one();
		return read(_pipe[0], data, sizeof(T)) > 0? 0: -1;
	}
	int unbuffered_send(T *data) {
		unique_lock<mutex> _lk(_inter_mutex);
		_send_num++;
		_cond.wait(_lk, [this]{return this->_recv_num > 0;});
		_send_num--;
		_recv_num--;
		return write(_pipe[1], data, sizeof(T)) > 0? 0: -1;
	}

};
