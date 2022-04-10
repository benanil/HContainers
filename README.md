Single header lightweight plain data structures containers:
* Linked List
* Array List (std::vector functionality)
* Priority Queue 
* Stack
* Queue
and BinaryTree 

```cpp
Array<Test> _array = Array<Test>();
for (int i = 10; i >= 0; --i) { // generate unsorted array
	_array.Add(Test(i));
}

std::cout << "array queue" << std::endl;

for (auto& value : _array) {
	std::cout << value.a << std::endl;
}

// qsort(_array.ptr, _array.size, sizeof(Test), Compare::QGreater<int>);
XorSort(reinterpret_cast<int*>(_array.ptr), _array.size);

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
priorityQueue.ptr = nullptr;
priorityQueue.size = 0;

Test* heap = bt.ConvertToHeap();

std::cout << "converted heap queue" << std::endl;

for (int i = 0; i < bt.size; i++)
{
	std::cout << heap[i].a << std::endl;
}

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
```