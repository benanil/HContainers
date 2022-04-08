#pragma once
// #include <stdlib.h>
#include <cstdlib>

#define HS_ARRAY_IMPL() T& operator[](int index) { return ptr[index]; }                        \
                        const T& operator[](int index) const { return ptr[index]; }            \
                        T* begin() { return ptr; }								               \
                        T* end() { return ptr + size; }							               \
                        const T* cbegin() const { return ptr; }					               \
                        const T* cend() const { return ptr + size; }				           \
                        T GetLast() { return ptr[size - 1]; }                                  \
                        T GetFirst() { return ptr[0]; }                                        \
					    void Clear()  { /*std::_Fill_zero_memset(ptr, capacity); size = 0;*/ }

enum class HArrayResult : int
{
	None, Success, Fail, IndexBoundsOutOfArray, NotFinded, Size0
};

namespace Compare
{
	template<typename T> bool Less(T a, T b) { return a < b; }
	template<typename T> bool Equal(T a, T b) { return a == b; }
	template<typename T> bool Greater(T a, T b) { return !Less(a, b) && !Equal(a, b); }
	template<typename T> bool GreaterEqual(T a, T b) { return !Less(a, b); }
	template<typename T> bool LessEqual(T a, T b) { return Less(a, b) && Equal(a, b); }

	/*for qsort*/ template<typename T>
	int QLess(const void* a, const void* b) { return *(T*)a < *(T*)b; }
	/*for qsort*/ template<typename T>
	int QGreater(const void* a, const void* b) { return *(T*)a > *(T*)b; }
}

template<class T>
class LinkedList
{
public:
	typedef void(*IterateFunc)(T*);

	template<class UserClass> struct ClassIterator
	{
		typedef void(*_Func)(UserClass*, T*);
		UserClass* userClass;
		_Func func;
		inline ClassIterator(UserClass* _class, _Func _func) : userClass(_class), func(_func) {}
		inline void Invoke(T* data) { func(userClass, data); };
	};

	struct Node {
		T* data;
		Node* next;
		Node(T* _data, Node* _next) : data(_data), next(_next) {  }
	};

	Node* rootNode, * endNode;
	int nodeCount;

	LinkedList() : rootNode(nullptr), endNode(nullptr), nodeCount(0) { }

	LinkedList(T* firstClass) : rootNode(new Node(firstClass, nullptr)), nodeCount(1) { endNode = rootNode; }

	LinkedList(Node* _rootNode) : rootNode(_rootNode), endNode(_rootNode), nodeCount(1) { }
	// size must bigger than 0
	LinkedList(T* begin, int size) : rootNode(new Node(*begin)), endNode(new Node(*(begin + size - 1))), nodeCount(size)
	{
		T* end = begin + size;
		for (T* end = begin + size - 1, ++begin; begin < end; ++begin) AddBack(*begin);
	}

	~LinkedList() { IterateRecDestroy(rootNode); }

	// not recomended for iterating. for iterating Iterate function is more efficient
	T* operator[](int index) const
	{
		int startIndex = 0;
		return FindDataByIndexRec(rootNode, index, startIndex);
	}

#define _Template template<typename Derived, typename std::enable_if < \
		std::is_base_of<T, Derived>{} || std::is_same<T, Derived>{}, bool > ::type = true >

	_Template void AddFront(Derived* data)
	{
		Node* newNode = new Node(dynamic_cast<T*>(data), nullptr);
		if (endNode) {
			endNode->next = newNode;
		}
		else {
			endNode = newNode;
			rootNode = newNode;
		}
		endNode = newNode;
		nodeCount++;
	}

	// sets data as first node(root node)
	_Template void AddBack(Derived* data)
	{
		Node* oldRoot = rootNode;
		rootNode = new Node(dynamic_cast<T*>(data), oldRoot);
		nodeCount++;
	}

	_Template void Remove(Derived* ptr)
	{
		Node* currentNode = rootNode;

		if (rootNode->data == ptr) {
			rootNode = rootNode->next;
			delete currentNode;
			currentNode = rootNode;
			return;
		}

		do {
			if (currentNode->next->data == ptr)
			{
				Node* removedNode = currentNode->next;
				currentNode->next = removedNode->next;
				delete removedNode;
				break;
			}
			currentNode = currentNode->next;
		} while (currentNode->next != nullptr);
	}

	_Template bool TryGetData(Derived** component)
	{
		*component = FindNodeByType<Derived>();
		return component[0];
	}

#undef _Template
	// returns removed data
	T* RemoveFront()
	{
		if (nodeCount == 0) return nullptr;
		T* oldEndNodeData = endNode->data;
		Node* oldNode = endNode;
		--nodeCount;
		endNode = FindSecondEndNodeRec(rootNode); // new end node
		endNode->next = nullptr;
		delete oldNode;
		return oldEndNodeData;
	}

	// returns removed data
	// removes first node(root node)
	T* RemoveBack()
	{
		if (!rootNode) return nullptr;
		T* oldRootNodeData = rootNode->data;
		Node* oldNode = rootNode;
		--nodeCount;
		rootNode = rootNode->next;
		delete oldNode;
		return oldRootNodeData;
	}

	template<class Desired> Desired* FindNodeByType() const
	{
		return FindNodeByTypeRec<Desired>(rootNode);
	}

	T* FindNodeFromPtr(T* ptr) const
	{
		Node* currentNode = rootNode;
		if (currentNode->data == ptr) {
			return currentNode->data;
		}

		while (currentNode->next != nullptr) {
			currentNode = currentNode->next;
			if (currentNode->data == ptr) {
				return currentNode->data;
			}
		}
		return nullptr;
	}

	// this function is more performant than reaching every node by index
	void Iterate(IterateFunc func) const
	{
		Node* currentNode = rootNode;
		func(currentNode->data);

		while (currentNode->next != nullptr) {
			currentNode = currentNode->next;
			func(currentNode->data);
		}
	}

	template<class UserClass>
	void IterateClass(ClassIterator<UserClass> iterator) const
	{
		Node* currentNode = rootNode;
		iterator.Invoke(currentNode->data);

		while (currentNode->next != nullptr) {
			currentNode = currentNode->next;
			iterator.Invoke(currentNode->data);
		}
	}

private:

	bool HasNodeFindByPtrRec(Node* node, T* data)
	{
		if (node->next == nullptr) return false;
		if (node->data == data)    return true;
		return HasNodeFindByPtrRec(node->next);
	}

	// finds this[nodeCount-2] not last one but one before lastNode
	Node* FindSecondEndNodeRec(Node* node)
	{
		if (node->next == nullptr) return nullptr;
		if (node->next->next == nullptr) return node;
		return FindSecondEndNodeRec(node->next);
	}

	Node* FindNodeByIndexRec(Node* node, int targetIndex, int& index) const
	{
		if (index == targetIndex || node->next == nullptr) return node;
		return FindNodeByIndexRec(node->next, targetIndex, ++index);
	}

	T* FindDataByIndexRec(Node* node, int targetIndex, int& index) const
	{
		if (index == targetIndex || node->next == nullptr) return node->data;
		return FindDataByIndexRec(node->next, targetIndex, ++index);
	}

	template<class Desired> Desired* FindNodeByTypeRec(Node* node) const
	{
		if (node->data != nullptr && dynamic_cast<Desired*>(node->data))
			return dynamic_cast<Desired*>(node->data);

		if (node->next != nullptr) return FindNodeByTypeRec<Desired>(node->next);

		return nullptr;
	}

	void IterateRecDestroy(Node* node)
	{
		if (!node) return;
		if (node->next != nullptr)
			IterateRecDestroy(node->next);
		node->data->~T();
		std::free(node->data);
		std::free(node);
	}
};

template<typename T>
class Array
{
public:
	~Array() 
	{
		if (ptr) {  Clear();  std::free(ptr);  ptr = nullptr;  }
	}
	Array() : size(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T)); }
	Array(int _size) : size(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T)); }

	// initialize operator[] begin(), end(), GetFirst(), GetLast(), Clear()
	HS_ARRAY_IMPL()

	void Add(T value) {
		
		if (size + 1 == capacity) {
			capacity += 32;
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			std::memset(ptr + capacity - 32, 0, sizeof(T) * 32); // clear generated new area
		}
		ptr[size++] = value;
	}

	void SetRange(int start, int end, T value) {
		for (; start < end; ++start) ptr[start] = value;
	}

	void SetRange(T* start, T* end, T value) {
		for (; start < end; ++start) *start = value;
	}

	void AddAtIndex(int index, T value) {
		if (size + 2 >= capacity) {
			capacity += 32;
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
		}

		std::memmove(ptr + index + 1, ptr + index, sizeof(T) * std::abs(index - size));
		ptr[index] = value;
		++size;
	}

	HArrayResult Remove(T value)
	{
		for (int i = 0; i < size; ++i)
			if (value == ptr[i]) {
				RemoveAtIndex(i); return HArrayResult::Success;
			}
		return HArrayResult::NotFinded;
	}

	void AddFront(T value) {
		if (size + 1 >= capacity)  {
			capacity += 32; 
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
		}
		ptr[0] = value;
		std::memmove(ptr, ptr + 1, size * sizeof(T));
		++size;
	}

	void RemoveBack() { std::memset(&ptr[--size], 0, sizeof(T)); }
	void RemoveFront() { std::memmove(ptr, ptr + 1, size * sizeof(T)); std::memset(&ptr[--size], 0, sizeof(T)); }

	void RemoveAtIndex(int index) {
		std::memset(&ptr[index], 0, sizeof(T));
		std::memmove(ptr + index, ptr + index + 1, size * sizeof(T));
		--size;
	}

public:
	T* ptr;
	int size;
	int capacity;
};

// for this tree you need to provide Compare::Equal, Less, GreaterEqual for non primitive types
template<typename T>
class BinaryTree
{
public:
	struct Node {
		T data;
		Node* left, * right;
		Node(T _data) : data(_data), left(nullptr), right(nullptr) {};
	};
public:
	~BinaryTree() { Clear(); }
	BinaryTree() : rootNode(nullptr) {}
	BinaryTree(const BinaryTree& bt) = delete; // copy constructor
	BinaryTree(T firstValue) : rootNode(new Node(firstValue)) {}
	// send sorted array for better approach
	BinaryTree(Array<T> _array) : size(1) {
		rootNode = new Node(_array.GetLast());
		AddRange(_array.begin(), _array.end() - 1);// -1 for skipping root node
	}

	// send sorted array for better approach
	BinaryTree(T* begin, T* end) : size(1) {
		rootNode = new Node(*(end - 1));
		AddRange(begin, end - 1); // -1 for skipping root node
	}

	void AddRange(T* begin, T* end)
	{
		if (!rootNode) { rootNode = new Node(*(end - 1)); size = 1; }

		while (end > begin) Add(*(--end));
	}

	void Add(T value) {
		if (!rootNode) { rootNode = new Node(value); size = 1; return; }
		AddRec(rootNode, value);
		++size;
	}

	inline int NextPowerOf2(unsigned int v) {
		v--;
		v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16;
		v++;
		return v;
	}

	T* ConvertToHeap()
	{
		T* result = (T*)std::calloc(size, sizeof(T));

		int maxNodePerBranch = std::log2(NextPowerOf2(size));

		Node** nodeArray = (Node**)std::calloc(maxNodePerBranch, sizeof(Node*));
		nodeArray[0] = rootNode;

		int currentIndex = 0;
		int leafCount = rootNode != nullptr;

		while (leafCount > 0)
		{
			int newLeafCount = 0;
			for (int i = 0; i < leafCount; ++i)
			{
				result[currentIndex++] = nodeArray[i]->data;
				Node* temp = nodeArray[i];
				if (temp->right) nodeArray[newLeafCount++] = temp->right;
				if (temp->left)  nodeArray[newLeafCount++] = temp->left;
			}
			leafCount = newLeafCount;
		}

		free(nodeArray);
		return result;
	}

	HArrayResult Remove(T value) {
		FindNodeRecord searchRecord = FindNodeByValueWithParentRec(rootNode, nullptr, value);
		if (searchRecord.success)
		{
			if (searchRecord.parent != nullptr) // has parent
			{
				if (searchRecord.parent->left == searchRecord.node) searchRecord.parent->left = nullptr;
				else searchRecord.parent->right = nullptr;
			}
			ClearRec(searchRecord.node);
			return HArrayResult::Success;
		}
		else return HArrayResult::NotFinded;
	}

	HArrayResult Remove(Node* node) {
		Node* parent = FindNodeParent(node, nullptr, node->data);
		if (parent)
		{
			if (parent->left == node) parent->left = nullptr;
			else parent->right = nullptr;
			ClearRec(node);
			return HArrayResult::Success;
		}
		else return HArrayResult::NotFinded;
	}

	bool HasValue(T value) const {
		return FindNodeByValueRec(rootNode, value) != nullptr;
	}

	Node* Search(T value) const { return FindNodeByValueRec(rootNode, value); }

	void Clear() { ClearRec(rootNode); }

private:
	void ClearRec(Node* node)
	{
		if (!node) return;
		if (node->right) ClearRec(node->left);
		if (node->left)  ClearRec(node->right);
		--size;
		delete node;
		node = nullptr;
	}

	Node* FindNodeParent(Node* node, Node* parent, T value) const
	{
		if (node) {
			if (Compare::Equal<T>(value, node->data)) return parent;

			if (Compare::Less<T>(value, node->data)) {
				return FindNodeParent(node->left, node, value);
			}
			else {
				return FindNodeParent(node->right, node, value);
			}
		}
		return nullptr;
	}

	Node* FindNodeByValueRec(Node* node, T value) const
	{
		if (node)
		{
			if (Compare::Equal<T>(value, node->data)) return node;

			if (Compare::Less<T>(value, node->data)) {
				return FindNodeByValueRec(node->left, value);
			}
			else {
				return FindNodeByValueRec(node->right, value);
			}
		}
		return nullptr;
	}

	struct FindNodeRecord {
		Node* node, * parent; bool success;
	};
	FindNodeRecord FindNodeByValueWithParentRec(Node* node, Node* parent, T value) const
	{
		if (node)
		{
			if (Compare::Equal<T>(value, node->data)) {
				return { node, parent, true };
			}

			if (Compare::Less<T>(value, node->data)) {
				return FindNodeByValueWithParentRec(node->left, node, value);
			}
			else {
				return FindNodeByValueWithParentRec(node->right, node, value);
			}
		}
		return { nullptr, nullptr, false };
	}

	void AddRec(Node* node, T value) const {
		if (Compare::Less(value, node->data)) {
			if (node->left) AddRec(node->left, value);
			else node->left = new Node(value);
		}
		else if (Compare::GreaterEqual(value, node->data)) {
			if (node->right) AddRec(node->right, value);
			else node->right = new Node(value);
		}
	}

public:
	int size;
	Node* rootNode;
};

// recommended for integers and floats this container will optimize heap performance
// for this tree you need to provide Compare::Equal<T>, Compare::Greater<T> for non primitive types
template<typename T>
class PriarotyQueue
{
public:
	~PriarotyQueue() {
		if (ptr) {  Clear();  std::free(ptr);  ptr = nullptr;  }
	}
	PriarotyQueue() : size(0), capacity(32), ptr((T*)std::calloc(capacity, sizeof(T))) { }
	PriarotyQueue(int _size) : size(0), capacity(_size), ptr((T*)std::calloc(capacity, sizeof(T))) { }
	// send sorted array
	PriarotyQueue(T* begin, int _size) : ptr(begin), size(_size), capacity(32) { }

	// initialize operator[] begin(), end(), GetFirst(), GetLast(), Clear()
	HS_ARRAY_IMPL()

	void Add(T value)
	{
		if (Compare::Greater<T>(value, GetLast())) { _AddAtEnd(value); return; }
		for (int i = size - 1, j = 1; i >= 0; --i, ++j)
		{
			if (Compare::Greater<T>(value, ptr[i])) {
				std::memmove(ptr + i + 1, ptr + i, sizeof(T) * j);
				ptr[i] = value;
				break;
			}
		}
		++size;
	}

	T Pull() {
		T value = GetLast();
		RemoveBack();
		return value;
	}

	void Clear() const { std::memset(ptr, 0, sizeof(T) * capacity); }

	HArrayResult Remove(T value)
	{
		for (int i = 0; i < size; ++i)
			if (Compare::Equal<T>(value, ptr[i])) {
				RemoveAtIndex(i); return HArrayResult::Success;
			}

		return HArrayResult::NotFinded;
	}

	void RemoveBack() { std::memset(&ptr[--size], 0, sizeof(T)); }
	void RemoveFront() { std::memmove(ptr, ptr + 1, size * sizeof(T)); std::memset(&ptr[--size], 0, sizeof(T)); }

	void RemoveAtIndex(int index) {
		std::memset(&ptr[index], 0, sizeof(T));
		std::memmove(ptr + index, ptr + index + 1, size * sizeof(T));
		--size;
	}
private:
	void _AddAtEnd(T value) {
		if (size + 1 == capacity) {
			capacity += 32;
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
		}
		ptr[size++] = value;
	}
public:
	T* ptr;
	int size;
	int capacity;
};

template<typename T>
class Stack
{
public:
	~Stack() { if (ptr) {  Clear();  std::free(ptr);  ptr = nullptr;  } }

	Stack()
	: size(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T));  }
	Stack(int _size)
	: size(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T));  }

	// initialize operator[] begin(), end(), GetFirst(), GetLast(), Clear()
	HS_ARRAY_IMPL()

	void Push(T value) {
		if (size + 1 == capacity) {
			capacity += 32;
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
		}
		ptr[size++] = value;
	}

	T Pop() { return ptr[--size]; }
	bool TryPop(T& out) {
		if (size > 0) out = ptr[--size];
		return size > 0;
	}

public:
	int size;
	int capacity;
	T* ptr;
};

template<typename T>
class Queue
{
public:
	~Queue() { Clear(); std::free(ptr); ptr = nullptr; }

	Queue() : front(0), rear(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T));  }
	Queue(int _size) : front(0), rear(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T));  }

	void Clear() {
		std::memset(ptr, 0, sizeof(T) * capacity);
		rear = front = 0;
	}

	void Enqueue(T value) {
		if (front + 1 >= capacity)
		{
			capacity += 32;
			const int size = GetSize();
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			std::memmove(ptr, ptr + rear, size * sizeof(T));
			rear = 0;
			front = size;
		}
		ptr[front++] = value;
	}

	void Enqueue(T* begin, T* end)
	{
		const int count = (end - begin) * sizeof(T);

		if (front + count >= capacity)
		{
			capacity += 32 + count;
			const int size = GetSize();
			ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			std::memmove(ptr, ptr + rear, size * sizeof(T));
			rear = 0;
			front = size;
		}
		std::memcpy(ptr + rear + front, begin, count * sizeof(T));
	}

	// returns true if size is enough
	bool Dequeue(T** result, int count)
	{
		if (GetSize() < count) return false;
		*result = (T*)std::malloc(sizeof(T) * count);
		std::memcpy(result, ptr + rear, sizeof(count));
		rear += count;
		return true;
	}

	T Dequeue() {
		return ptr[rear++];
	}
	
	bool TryDequeue(T& out)
	{
		if (GetSize() > 0) out = Dequeue();
		return GetSize();
	}

public:
	T* ptr;
	int GetSize() { return front - rear; }
private:
	int capacity;
	int front = 0;
	int rear  = 0;
};
