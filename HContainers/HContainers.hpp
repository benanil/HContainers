#pragma once
#include <cstring>
#include <stdlib.h>

enum class HArrayResult : int
{
	None, Success, Fail, IndexBoundsOutOfArray, NotFinded, Size0
};

namespace Compare
{
	template<typename T> bool Less(T a, T b) { return a < b; }
	template<typename T> bool Greater(T a, T b) { return a > b; }
	template<typename T> bool Equal(T a, T b) { return a == b; }
	template<typename T> bool GreaterEqual(T a, T b) { return a >= b; }
	template<typename T> bool LessEqual(T a, T b) { return a <= b; }

	/*for qsort*/ template<typename T>
	int QLess(const void* a, const void* b) { return *(T*)a < *(T*)b; }
	/*for qsort*/ template<typename T>
	int QGreater(const void* a, const void* b) { return *(T*)a > *(T*)b; }
}

template<typename T>
class Array
{
public:
	~Array() {
		if (ptr) { free(ptr); ptr = nullptr; }
	}
	Array() : size(0), capacity(32) {
		ptr = (T*)malloc(sizeof(T) * capacity);
		memset(ptr, 0, sizeof(T) * capacity);
	}
	Array(int _size) : size(0), capacity(_size + 32) {
		ptr = (T*)malloc(sizeof(T) * capacity);
		memset(ptr, 0, sizeof(T) * capacity);
	}

	T& operator[](int index) { return ptr[index]; }
	const T& operator[](int index) const { return ptr[index]; }

	T* begin() { return ptr; }
	T* end() { return ptr + size; }
	const T* cbegin() const { return ptr; }
	const T* cend() const { return ptr + size; }
	T GetLast() { return ptr[size - 1]; }

	void Add(T value) {
		if (size + 1 == capacity)
		{
			capacity += 32;
			T* old = ptr;
			ptr = (T*)malloc(capacity * sizeof(T));
			memcpy(ptr, old, capacity * sizeof(T));
			free(old);
		}
		ptr[size] = value;
		++size;
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
			T* old = ptr;
			ptr = (T*)malloc(capacity * sizeof(T));
			memcpy(ptr, old, size * sizeof(T));
		}

		memmove(ptr + index + 1, ptr + index, sizeof(T) * abs(index - size));
		ptr[index] = value;
		++size;
	}

	void Clear() const { memset(ptr, 0, sizeof(T) * capacity); size = 0; }

	HArrayResult Remove(T value)
	{
		for (int i = 0; i < size; ++i)
			if (value == ptr[i])
			{
				RemoveAtIndex(i); return HArrayResult::Success;
			};

		return HArrayResult::NotFinded;
	}

	void AddFront(T value) {
		if (size + 1 >= capacity) { capacity += 32; ptr = (T*)realloc(ptr, capacity * sizeof(T)); }
		ptr[0] = value;
		memmove(ptr, ptr + 1, size * sizeof(T));
		++size;
	}

	void RemoveBack() { memset(&ptr[--size], 0, sizeof(T)); }
	void RemoveFront() { memmove(ptr, ptr + 1, size * sizeof(T)); memset(&ptr[--size], 0, sizeof(T)); }

	void RemoveAtIndex(int index) {
		memset(&ptr[index], 0, sizeof(T));
		memmove(ptr + index, ptr + index + 1, size * sizeof(T));
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
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	T* ConvertToHeap()
	{
		T* result = (T*)malloc(sizeof(T) * size);
		memset(result, 0, sizeof(T) * size);

		int maxNodePerBranch = log2(NextPowerOf2(size));

		Node** nodeArray = (Node**)malloc(sizeof(Node*) * maxNodePerBranch);
		memset(nodeArray, 0, sizeof(Node*) * maxNodePerBranch);
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

			if (Compare::LessEqual<T>(value, node->data)) {
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
		//if (ptr) { delete ptr; ptr = nullptr; } 
	}
	PriarotyQueue() : size(0), capacity(32) {
		_Initialize();// malloc(capacity) and memset(ptr,0, capacity)
	}
	PriarotyQueue(int _size) : size(0), capacity(_size + 32) {
		_Initialize();// malloc(capacity) and memset(ptr,0, capacity)
	}
	// send sorted array
	PriarotyQueue(T* begin, int _size) : ptr(begin), size(_size), capacity(32) { }

	T& operator[](int index) { return ptr[index]; }
	const T& operator[](int index) const { return ptr[index]; }
	T* begin() { return ptr; }
	T* end() { return ptr + size; }
	const T* cbegin() const { return ptr; }
	const T* cend() const { return ptr + size; }
	T GetLast() { return ptr[size - 1]; }

	void Add(T value)
	{
		if (Compare::Greater<T>(value, GetLast())) { _AddAtEnd(value); return; }
		for (int i = size - 1, j = 1; i >= 0; --i, ++j)
		{
			if (Compare::Greater<T>(value, ptr[i])) {
				memmove(ptr + i + 1, ptr + i, sizeof(T) * j);
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

	void Clear() const { memset(ptr, 0, sizeof(T) * capacity); }

	HArrayResult Remove(T value)
	{
		for (int i = 0; i < size; ++i)
			if (Compare::Equal<T>(value, ptr[i])) {
				RemoveAtIndex(i); return HArrayResult::Success;
			}

		return HArrayResult::NotFinded;
	}

	void RemoveBack() { memset(&ptr[--size], 0, sizeof(T)); }
	void RemoveFront() { memmove(ptr, ptr + 1, size * sizeof(T)); memset(&ptr[--size], 0, sizeof(T)); }

	void RemoveAtIndex(int index) {
		memset(&ptr[index], 0, sizeof(T));
		memmove(ptr + index, ptr + index + 1, size * sizeof(T));
		--size;
	}
private:
	void _Initialize() {
		ptr = (T*)malloc(sizeof(T) * capacity);
		memset(ptr, 0, sizeof(T) * capacity);
	}
	void _AddAtEnd(T value) {
		if (size + 1 == capacity) {
			capacity += 32;
			T* old = ptr;
			ptr = (T*)malloc(capacity * sizeof(T));
			memcpy(ptr, old, capacity * sizeof(T));
			free(old);
		}
		ptr[size++] = value;
	}
public:
	T* ptr;
	int size;
	int capacity;
};

