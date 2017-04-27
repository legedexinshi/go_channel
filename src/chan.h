#include <thread>
#include <cstdio>
#include <unistd.h>
#include <deque>

using namespace std;

template<typename T>
class Chan {
	mutex _r_mutex, _w_mutex, _inter_mutex;
	condition_variable _w_cond, _r_cond;
	bool _buffered;
	int _recv_num;
	int _pipe[2];
	int _capacity;
	bool _closed;
	deque<T> _deque;

 public:
	Chan (int cap = 0):_capacity(cap), _recv_num(0), _closed(false) {
		if (cap == 0) {
			if (pipe(_pipe) == -1) {
				printf("pipe error\n");
			}
			_buffered = false;
		}
		else {
			_buffered = true;
		}
	}

	int send(const T &data) {
		if (_closed) return -1;
		if (!_buffered) {
			unique_lock<mutex> w_lk(_w_mutex);
			return unbuffered_send(data);
		}
		else {
			return buffered_send(data);
		}
		return -1;
	}
	int recv(T &data) {
		if (_closed) return -1;
		if (!_buffered) {
			unique_lock<mutex> r_lk(_r_mutex);
			return unbuffered_recv(data);
		}
		else {
			return buffered_recv(data);
		}
		return -1;
	}

	void close() {
		{
			unique_lock<mutex> _lk(_inter_mutex);
			if (_closed) return;
			_closed = true;
			_deque.clear();
			::close(_pipe[0]);
			::close(_pipe[1]);
		}
		_w_cond.notify_one();
		_r_cond.notify_one();
	}

 private:
	int buffered_recv(T &data) {
		{
			unique_lock<mutex> _lk(_inter_mutex);
			_r_cond.wait(_lk, [this]{return !this->_deque.empty() || this->_closed;});
			if (_closed) return -1;
			data = _deque.front();
			_deque.pop_front();
		}
		_w_cond.notify_one();
		return 0;
	}
	int buffered_send(const T &data) {
		{
			unique_lock<mutex> _lk(_inter_mutex);
			_w_cond.wait(_lk, [this]{return this->_deque.size() < this->_capacity || this->_closed;});
			if (_closed) return -1;
			_deque.push_back(data);
		}
		_r_cond.notify_one();
		return 0;
	}
	int unbuffered_recv(T &data) {
		{
			unique_lock<mutex> lk(_inter_mutex);
			_recv_num++;
		}
		_w_cond.notify_one();
		return read(_pipe[0], &data, sizeof(T)) > 0? 0: -1;
	}
	int unbuffered_send(const T &data) {
		unique_lock<mutex> _lk(_inter_mutex);
		_w_cond.wait(_lk, [this]{return this->_recv_num > 0 || this->_closed;});
		if (_closed) return -1;
		_recv_num--;
		return write(_pipe[1], &data, sizeof(T)) > 0? 0: -1;
	}

};
