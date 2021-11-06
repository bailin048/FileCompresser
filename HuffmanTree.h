#pragma once

#include <queue>
#include <vector>
#include <functional>
using namespace std;

template <class W>
struct HuffmanTreeNode {
	HuffmanTreeNode<W>* left;
	HuffmanTreeNode<W>* right;
	HuffmanTreeNode<W>* parent;
	W weight;

	HuffmanTreeNode(const W& w = W()) :left(nullptr), right(nullptr), weight(w) ,parent(nullptr)
	{}
};

template <class W>
struct Com {
	typedef HuffmanTreeNode<W> Node;
	bool operator()(const Node* left, const Node* right) {
		return left->weight > right->weight;
	}
};

template <class W>
class HuffmanTree {
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree() :root(nullptr)
	{}

	~HuffmanTree() { Destroy(root); }

	void CreateHuffmanTree(const W array[], size_t size,const W& invalid) {
		//小堆
		std::priority_queue<Node*, vector<Node*>, Com<W>> q;
		//1.先使用所给权值创建只有根节点的二叉树森林
		for (size_t i = 0; i < size; ++i) {
			if(array[i]!=invalid) 
				q.push(new Node(array[i]));
		}
		//2.合并树
		while (q.size() > 1) {
			Node* left = q.top();
			q.pop();

			Node* right = q.top();
			q.pop();

			Node* parent = new Node(left->weight + right->weight);
			parent->left = left;
			parent->right = right;
			left->parent = parent;
			right->parent = parent;
			q.push(parent);
		}
		root = q.top();
	}

	void Destroy(Node*& proot) {
		if (proot != nullptr) {
			Destroy(proot->left);
			Destroy(proot->right);
			delete proot;
			proot = nullptr;
		}
	}

	Node* GetRoot() { return root; }
private:
	Node* root; 
};
