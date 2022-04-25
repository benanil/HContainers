Single header lightweight plain data structures containers:
* Linked List
* Array List (std::vector functionality)
* Priority Queue 
* Stack
* Queue
* Graph
and BinaryTree 

```cpp
CountryGraph graph = CountryGraph();

VertexIndex handle0 = graph.AddVertex(City("Istanbul", 16'000'000, 3400));
VertexIndex handle1 = graph.AddVertex(City("Ankara", 6'000'000, 06000));
VertexIndex handle2 = graph.AddVertex(City("Izmir", 5'000'000, 00000));
VertexIndex handle3 = graph.AddVertex(City("Bursa", 3'500'000, 00000));

graph.ConnectEdge(handle0, handle1, Road("E5", 500, 120));
graph.ConnectEdge(handle1, handle2, Road("Bla", 400, 120));
graph.ConnectEdge(handle2, handle3, Road("Bla", 200, 120));

VertexIndex secondHandle0 = graph.AddVertex(City("Berlin", 3'600'000, 0000));
VertexIndex secondHandle1 = graph.AddVertex(City("Cologne", 1'600'000, 0000));
VertexIndex secondHandle2 = graph.AddVertex(City("Hamburg", 1'800'000, 0000));

graph.ConnectEdge(secondHandle0, secondHandle1, Road("Road Bla", 255, 120));
graph.ConnectEdge(secondHandle1, secondHandle2, Road("Road Bla", 255, 120));

std::cout << "DFS" << std::endl;
DFS<City, Road> dfs = DFS<City, Road>(&graph);
dfs.Iterate(secondHandle0, IterateG);
std::cout << std::endl;
std::cout << "BFS" << std::endl;
BFS<City, Road> bfs = BFS<City, Road>(&graph);
bfs.Iterate(secondHandle0, IterateG);

MinimumSpanningTreePrim<City, Road> prim = MinimumSpanningTreePrim<City, Road>(graph);
CountryGraph::Vertex** primRes = prim.Solve(handle0);
int index = 0;

while (primRes[index] != nullptr) {
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
String subString = string.SubString(0, 3);
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
```
