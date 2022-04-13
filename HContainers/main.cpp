#include <iostream>
#include "HContainers.hpp"
#include "HStack.hpp"
#include "HString.hpp"

using namespace HS;

class Test
{
public:
	int a = 0;
	Test(int _a) : a(_a) {}
};

// these functions required for binary tree and priority queue
namespace HS::Compare
{
	template<> bool Less(Test a, Test b)  { return a.a < b.a; }
	template<> bool Equal(Test a, Test b) { return a.a == b.a; }
}

void XorSwap(int& x, int& y) {
	x ^= y; y ^= x; x ^= y;
}

//sorting with less chace usage
void XorSort(int arr[], int n)
{
	for (int i = 0; i < n; ++i)
		for (int j = i + 1; j < n; ++j)
			if (arr[j] < arr[i])
				XorSwap(arr[j], arr[i]);
}

int main()
{
	WString wstring = WString(L"hello I'm wide string");
	std::cout << wstring  << std::endl;

	String string = String("sad cat meows");
	std::cout << string.FindIndex("cat") << std::endl;
	string.Append(" append");
	string.Replace("append", "deppend");
	string.Remove("deppend");
	string.Replace("sad", "happy");
	// string.Insert(0, "happy ");
	String subString = string.SubString(0, 3);
	std::cout << "substring: " << subString << std::endl;
	std::cout << string << std::endl;
	// dynamicly allocate stack memory
	Slice* slice = HStack::Allocate(10, sizeof(int));
	int* ptr = slice->As<int>();

	for (int i = 0; i < 10; ++i) {
		ptr[i] = 4;
	}
	std::cout << "slice value" << std::endl;
	for (int i = 0; i < 10; ++i) {
		std::cout << ptr[i] << std::endl;
	}
	slice->~Slice();

	Slice* slice1 = HStack::Allocate(10, sizeof(int));
	ptr = slice1->As<int>();

	std::cout << "slice1 value" << std::endl;
	for (int i = 0; i < 10; ++i)
	{
		std::cout << ptr[i] << std::endl;
	}
	slice1->~Slice();

	ptr = reinterpret_cast<int*>(HStack::DebugPtr());

	std::cout << "cleared hstack memory" << std::endl;
	for (int i = 0; i < 20; i++)
	{
		std::cout << ptr[i] << std::endl;
	} 

	// Linked List
	LinkedList<Test> linkedList = LinkedList<Test>(Test(1));

	for (int i = 2; i < 6; ++i)
	{
		linkedList.AddFront(Test(i));
	}
	
	linkedList.Reverse();
	std::cout << "reversed linkedList" << std::endl;
	
	linkedList.Iterate([](Test test) {
		std::cout << test.a << std::endl;
	});
	// Array
	Array<Test> _array = Array<Test>();
    for (int i = 0; i < 10; ++i) { 
    	_array.Add(Test(i));
    }

	int* binarySearchResult = BinarySearch<int>(2, reinterpret_cast<int*>(_array.ptr), _array.size);
	if (binarySearchResult)
		std::cout << "binarySearchResult: " << (*binarySearchResult) << std::endl;

	std::cout << "array queue" << std::endl;
	
	for (auto& value : _array) {
		std::cout << value.a << std::endl;
	}
	// qsort(_array.ptr, _array.size, sizeof(Test), Compare::QGreater<int>);
	// PriorityQueue
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
	// Binary Tree
	BinaryTree bt = BinaryTree(priorityQueue.begin(), priorityQueue.end());
	priorityQueue.ptr = nullptr; // set nullptr here because we use  
	priorityQueue.size = 0;      // Priority queue's data in binary tree
	
	Test* heap = bt.ConvertToHeap();
	
	std::cout << "converted heap queue" << std::endl;
	
	for (int i = 0; i < bt.size; i++)
	{
		std::cout << heap[i].a << std::endl;
	}
	// Queue
	Queue<int> queue = Queue<int>(4);
	queue.Enqueue(1);
	queue.Enqueue(2);
	queue.Enqueue(3);
	queue.Enqueue(4);
	std::cout << "queue" << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		std::cout << queue.Dequeue() << std::endl;
	}
	// Stack
	Stack<int> stack = Stack<int>(2);
	stack.Push(1);
	stack.Push(2);
	stack.Push(3);
	stack.Push(4);
	std::cout << "stack" << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		std::cout << stack.Pop() << std::endl;
	}
	return bt.Search(2) != nullptr;
}

