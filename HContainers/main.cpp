#include <iostream>
#include <chrono>
#include <iostream>
#include "HContainers.hpp"
#include "HString.hpp"
#include "HGraph.hpp"
#include "StaticHashMap.hpp"
#include "BinaryTree.hpp"
#include "Pair.hpp"

using namespace HS;


#ifndef NDEBUG
#	define CSTIMER(message) Timer timer = Timer(message);
#else
#   define CSTIMER(message) // Timer timer = Timer(message);
#endif

struct Timer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
	bool printMilisecond;

	const char* message;

	Timer(const char* _message) : message(_message), printMilisecond(true)
	{
		start_point = std::chrono::high_resolution_clock::now();
	}

	const double GetTime()
	{
		using namespace std::chrono;
		auto end_point = high_resolution_clock::now();
		auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
		auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
		printMilisecond = false;
		return (end - start) * 0.001;
	}

	~Timer()
	{
		using namespace std::chrono;
		if (!printMilisecond) return;
		auto end_point = high_resolution_clock::now();
		auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
		auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
		auto _duration = end - start;
		std::cout << message << (_duration * 0.001) << "ms" << std::endl;
	}
};

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

struct City
{
	const char* name;
	int population;
	int zipCode;
	City() {}
	City(const char* _name, int _population, int _zipCode) : name(_name), population(_population), zipCode(_zipCode) {}
};

struct Road
{
	float length;
	int speedLimit;
	const char* name;
	Road() {}
	Road(const char* _name, float _length, int _speedLimit) : name(_name), length(_speedLimit), speedLimit(_speedLimit) {}
};

void IterateG(Graph<City, Road>::Edge* edge, Graph<City, Road>::Vertex* vertex)
{
	if (edge)   { std::cout << "edge data: " << edge->data.name << std::endl; }
	if (vertex) { std::cout << "vertex data: " << vertex->data.name << std::endl; }
}

typedef Graph<City, Road> CountryGraph;

bool RemoveLessThan2(const int& i) { return i < 5; }

int main()
{

	{
		StaticHashMap<int, float> hashMap;
		for (int i = 0; i < 80; ++i)
			hashMap.Insert(i, (float)i * 0.5f);
		
		bool contains = hashMap.Contains(5);
		float* find = hashMap.Find(40);
		hashMap[5] += 55.0f;
		
		int len = 0;
		for (auto& it : hashMap)
		{
			std::cout << it << std::endl;
			if (len++ > 10) break;
		}

		std::cout << hashMap[40] << std::endl;
		std::cout << hashMap[5] << std::endl;
	}
	
	return 0;

	{
		Array<int> removeTest = Array<int>(10);
		for (int i = 0; i < 10; i++) removeTest.Add(i);
		CSTIMER("RemoveAll: ");
		removeTest.RemoveAll(RemoveLessThan2);
		for (auto& i : removeTest)
		{
			std::cout << i << std::endl;
		}
	}
	{
		Array<int> removeTest1 = Array<int>(4001);
		for (int i = 0; i < 4001; i++) removeTest1.Add(i % 5);
		
		CSTIMER("Remove: ");
		HArrayResult result;
		do {
			result = removeTest1.Remove(RemoveLessThan2);
		} while (result == HArrayResult::Success);
	}
	
	CountryGraph graph = CountryGraph();

	VertexIndex turk0 = graph.AddVertex(City("Istanbul", 16'000'000, 3400));
	VertexIndex turk1 = graph.AddVertex(City("Ankara"  , 6'000'000, 06000));
	VertexIndex turk2 = graph.AddVertex(City("Izmir"   , 5'000'000, 00000));
	VertexIndex turk3 = graph.AddVertex(City("Bursa"   , 3'500'000, 00000));

	graph.ConnectEdge(turk0, turk1, Road("E5", 500, 120));
	graph.ConnectEdge(turk1, turk2, Road("Bla", 400, 120));
	graph.ConnectEdge(turk2, turk3, Road("Bla", 200, 120));

	VertexIndex german0 = graph.AddVertex(City("Berlin" , 3'600'000, 0000));
	VertexIndex german1 = graph.AddVertex(City("Cologne", 1'600'000, 0000));
	VertexIndex german2 = graph.AddVertex(City("Hamburg" , 1'800'000, 0000));

	graph.ConnectEdge(german0, german1, Road("Road Bla", 255, 120));
	graph.ConnectEdge(german1, german2, Road("Road Bla", 255, 120));

	std::cout << "DFS" << std::endl;
	DFS<City, Road> dfs = DFS<City, Road>(&graph);
	dfs.Iterate(german0, IterateG);
	std::cout << std::endl;
	std::cout << "BFS" << std::endl;
	BFS<City, Road> bfs = BFS<City, Road>(&graph);
	bfs.Iterate(german0, IterateG);

	MinimumSpanningTreePrim<City, Road> prim = MinimumSpanningTreePrim<City, Road>(graph);
	CountryGraph::Vertex** primRes = prim.Solve(turk0);
	int index = 0;

	while(primRes[index] != nullptr) {
		std::cout << "prim vert: " << primRes[index]->data.name << std::endl;
		index++;
	}
	free(primRes);

	
	WString wstring = WString(L"hello I'm wide string");
	std::cout << wstring << std::endl;

	String string = String("sad cat meows");
	std::cout << string.FindIndex("cat") << std::endl;
	string.Append(" append");
	string.Replace("append", "deppend");
	string.Remove("deppend");
	string.Replace("sad", "happy");
	// string.Insert(0, "happy ");
	String subString = string.SubString(0, 5);
	std::cout << "substring: " << subString << std::endl;
	std::cout << string << std::endl;
	
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

	return bt.Search(2) != nullptr; // binary tree search
}