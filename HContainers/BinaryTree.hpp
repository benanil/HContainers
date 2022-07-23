#pragma once
#include "Common.hpp"
#include "Array.hpp"

namespace HS
{
		// for this tree you need to provide Compare::Equal, Compare::Less for non primitive types
	template<typename T>
	class BinaryTree
	{
	public:
		struct Node {
			T data;
			Node* left, * right;
			Node() : left(nullptr), right(nullptr) {}
			Node(T _data) : data(_data), left(nullptr), right(nullptr) {};
		};
		typedef void(*IterateFunc)(Node*);

		template<class UserClass> struct ClassIterator
		{
			typedef void(*_Func)(UserClass*, Node*);
			UserClass* userClass;
			_Func func;
			inline ClassIterator(UserClass* _class, _Func _func) : userClass(_class), func(_func) {}
			inline void Invoke(T* data) { func(userClass, data); };
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
		void AddNode(Node* node) {
			if (!node || node == rootNode) return;
			if (!rootNode) { rootNode = node; size = 1; return; }
			AddNodeRec(rootNode, node);
			++size;
		}

		inline int NextPowerOf2(unsigned int v) {
			v--;
			v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16;
			v++;
			return v;
		}
		// converts to sorted heap
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
		// traverse all nodes
		void Iterate(IterateFunc iterateFunc) const
		{
			IterateRec(iterateFunc, rootNode);
		}
		// traverse all nodes
		template<class UserClass>
		void IterateClass(ClassIterator<UserClass> iterator) const
		{
			IterateClassRec(iterator, rootNode);
		}

		HArrayResult Remove(T value) {
			FindNodeRecord searchRecord = FindNodeByValueWithParentRec(rootNode, nullptr, value);
			if (searchRecord.success)
			{
				if (searchRecord.node == searchRecord.parent->left)  searchRecord.parent->left = nullptr;
				else/*searchRecord.node==searchRecord.parent->right*/ searchRecord.parent->right = nullptr;
				
				if (searchRecord.parent) {
					AddNodeRec(searchRecord.parent, searchRecord.node->left); 
					AddNodeRec(searchRecord.parent, searchRecord.node->right); 
				}
				else if (searchRecord.node == rootNode)
				{
					if (searchRecord.node->right) {
						rootNode = searchRecord.node->right;
						AddNodeRec(searchRecord.parent, searchRecord.node->left);
					}
					else {
						rootNode = searchRecord.node->left;
					}
				}
				else {
					AddNodeRec(rootNode, searchRecord.node->left);
					AddNodeRec(rootNode, searchRecord.node->right);
				}
				delete searchRecord.node;
				--size;
				return HArrayResult::Success;
			}
			else return HArrayResult::NotFinded;
		}

		HArrayResult Remove(Node* node) {
			return Remove(node->data);
		}

		bool HasValue(T value) const {
			return FindNodeByValueRec(rootNode, value) != nullptr;
		}

		Node* Search(T value) const { return FindNodeByValueRec(rootNode, value); }

		void Clear() { ClearRec(rootNode); }

	private:
		template<class UserClass>
		void IterateClassRec(ClassIterator<UserClass> iterator, Node* node) const
		{
			if (!node) return;
			iterator.Invoke(node);
			IterateClassRec(iterator, node->right);
			IterateClassRec(iterator, node->left);
		}
		void IterateRec(IterateFunc iterateFunc, Node* node) const
		{
			if (!node) return;
			IterateRec(iterateFunc, node->right);
			IterateRec(iterateFunc, node->left);
		}
		void ClearRec(Node* node)
		{
			if (!node) return;
			ClearRec(node->left); ClearRec(node->right);
			--size;
			delete node;
			node = nullptr;
		}
		Node* FindNodeParent(Node* node, Node* parent, T value) const
		{
			if (!node) return nullptr;
			if (Compare::Equal<T>(value, node->data)) return parent;

			if (Compare::Greater<T>(value, node->data)) {
				return FindNodeParent(node->left, node, value);
			}
			else {
				return FindNodeParent(node->right, node, value);
			}
		}

		Node* FindNodeByValueRec(Node* node, T value) const
		{
			if (!node) return nullptr;
			
			if (Compare::Equal<T>(value, node->data)) return node;

			if (Compare::Greater<T>(value, node->data)) {
				return FindNodeByValueRec(node->left, value);
			}
			else {
				return FindNodeByValueRec(node->right, value);
			}
		}

		struct FindNodeRecord {
			Node* node, * parent; bool success;
		};
		FindNodeRecord FindNodeByValueWithParentRec(Node* node, Node* parent, T value) const
		{
			if (!node) return { nullptr, nullptr, false };

			if (Compare::Equal<T>(value, node->data)) {
				return { node, parent, true };
			}
			if (Compare::Greater<T>(value, node->data)) {
				return FindNodeByValueWithParentRec(node->left, node, value);
			}
			else {
				return FindNodeByValueWithParentRec(node->right, node, value);
			}
		}
		void AddNodeRec(Node* node, Node* addNode) const {
			if (!node || !addNode) return;
			if (Compare::Greater(addNode->data, node->data)) {
				if (node->left) AddNodeRec(node->left, addNode);
				else node->left = addNode;
			}
			else if (Compare::LessEqual(addNode->data, node->data)) {
				if (node->right) AddNodeRec(node->right, addNode);
				else node->right = addNode;
			}
		}
		void AddRec(Node* node, T value) const {
			if (!node) return;
			if (Compare::Greater(value, node->data)) {
				if (node->left) AddRec(node->left, value);
				else node->left = new Node(value);
			}
			else if (Compare::LessEqual(value, node->data)) {
				if (node->right) AddRec(node->right, value);
				else node->right = new Node(value);
			}
		}

	public:
		int size;
		Node* rootNode;
	};
}