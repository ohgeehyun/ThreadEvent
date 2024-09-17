#include <iostream>
#include<windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

using int32 = __int32;
using namespace std;

mutex m;
queue<int32> q;
HANDLE handle;
void Producer() {

	while (true) {
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		::SetEvent(handle);//커널 오브젝트의 시그널 상태를 변경
		this_thread::sleep_for(10000ms);
	}
	

}

void Consumer() {

	while (true){

		::WaitForSingleObject(handle, INFINITE); //핸들의 시그널을 확인(signal이면 통과)하여 2번쨰 인자에넣은 시간만큼 대기
												//현재는 핸들 시그널이 자동인상태라 스레드가 대기중이다 깨어나면 자동으로 시그널을 바꿔주지만 수동일 경우
												//ResetEvent(handle)을 통해 바꿔주어야한다.								
		unique_lock<mutex> lock(m);

			while (q.empty() == false) {
			int32 data = q.front();
			q.pop();
			cout << data << endl; //스레드로 실행중인 영역에서 cout등 커널을다시부르는 습관은 좋지 않다고 한다. 현재는 공부용이니 진행
		}
	}

}


int main()
{
	handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);//(1.보안속성,2.수동리셋 여부,3.초기상태,4.이벤트 객체의 이름) 
														//유저영역이 아닌 커널 영역에서 관리 하기에 커널 오브젝트라고도한다.
														//커널 오브젝트들의 공통적인 속성 몇가지가 있는데
														//Usage count 이 오브젝트 를 사용중인 대상의 수
														//signal(TRUE) / non-signal(FALSE)
														// Auto / Manual 리셋 속성 (2번 인자)

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);

}
