#include <iostream>
#include "HContainers.hpp"
#include <chrono>
struct Timer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start_point;

	const char* message;

	Timer(const char* _message) : message(_message)
	{
		start_point = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		using namespace std::chrono;
		auto end_point = high_resolution_clock::now();
		auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
		auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
		auto _duration = end - start;
		float ms = _duration * 0.001;
		std::cout << message << ms << "ms" << std::endl;
	}
};

struct Test
{
	int a;
	Test(int _a) : a(_a) {}
};

namespace Compare
{
	template<> bool Less(Test a, Test b) { return a.a < b.a; }
	template<> bool Greater(Test a, Test b) { return a.a > b.a; }
	template<> bool Equal(Test a, Test b) { return a.a == b.a; }
	template<> bool GreaterEqual(Test a, Test b) { return a.a >= b.a; }
	template<> bool LessEqual(Test a, Test b) { return a.a <= b.a; }
}

int main()
{
	Array<Test> _array = Array<Test>();
	for (int i = 10; i >= 0; --i) { // generate unsorted array
		_array.Add(Test(i));
	}

	std::cout << "array queue" << std::endl;

	for (auto& value : _array) {
		std::cout << value.a << std::endl;
	}

	qsort(_array.ptr, _array.size, sizeof(Test), Compare::QGreater<int>);

	PriarotyQueue<Test> priorityQueue = PriarotyQueue<Test>(_array.ptr, _array.size);
	priorityQueue.Add(Test(11));
	priorityQueue.Add(Test(12));
	priorityQueue.Add(Test(13));

	Test pull = priorityQueue.Pull();
	std::cout << "pulled: " << pull.a << std::endl;
	std::cout << "priority queue" << std::endl;

	for (auto& value : priorityQueue) {
		std::cout << value.a << std::endl;
	}

	BinaryTree bt = BinaryTree(priorityQueue.begin(), priorityQueue.end());

	Test* heap = bt.ConvertToHeap();

	std::cout << "converted heap queue" << std::endl;

	for (int i = 0; i < bt.size; i++)
	{
		std::cout << heap[i].a << std::endl;
	}
	free(heap);
	return bt.Search(2) != nullptr;
}

