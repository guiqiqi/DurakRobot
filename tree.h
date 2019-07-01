#pragma once

#include "header.h"

/* Tree (data structure) support */
class Node {
private:
	int _degree;
	Data* data;
	Node* _father = nullptr;
	std::vector<Node*> children;

public:
	Node(void);
	Node(const Data& data);
	~Node(void);

	std::vector<Node*>::const_iterator begin(void) const;
	std::vector<Node*>::const_iterator end(void) const;

public:
	Data* get(void) const;
	int degree(void) const;
	Node* father(void) const;

	void add(Node*& child);
	void add(const Data& data);
	void remove(const int index);

	Node* operator[] (int index);
	const Node* operator[] (int index) const;

	//friend class Iterator;
};

/* Traversing support */
template <typename Contaniner>
class Traverse {
protected:
	Node* current;
	bool running = true;
	Contaniner unvisited;

public:
	// Prevent the compiler from generating a default constructor
	Traverse(void) { this->current = nullptr; }

	typedef Node* data;
	Traverse(Node* root) {
		this->unvisited.push(root);
		this->current = root;
	}

	bool status(void) const {
		return this->running;
	}

	virtual void next(void) = 0;

	data get(void) const {
		return this->current;
	}

	data yield(void) {
		try {
			this->next();
			data current = this->current;
			return current;
		}
		catch (std::out_of_range) {
			this->running = false;
			return nullptr;
		}
	}
};

/*
	Traversing the tree using the BFS algorithm
	Traverse all nodes of the tree using the queue's first-in,
	first-out (FIFO) feature level.
*/
class BFSTraverse : public Traverse<std::queue<Node*>> {
	using Traverse::Traverse;
	void next(void) {
		// If queue is empty
		if (!this->unvisited.size())
			throw std::out_of_range("traverse ended.");

		this->current = this->unvisited.front();
		this->unvisited.pop();

		// Traversing the child nodes to the queue
		if (!this->current->degree())
			return;
		for (auto child : *(this->current))
			this->unvisited.push(child);
	}
};

/*
	Traversing the tree using the DFS algorithm
	The principle is the same as above, but the stack is LIFO
*/
class DFSTraverse : public Traverse<std::stack<Node*>> {
	using Traverse::Traverse;
	void next(void) {
		// If list if empty
		if (!this->unvisited.size())
			throw std::out_of_range("traverse ended.");

		this->current = this->unvisited.top();
		this->unvisited.pop();

		// Traversing the child nodes to the queue
		if (!this->current->degree())
			return;
		for (auto child : *(this->current))
			this->unvisited.push(child);
	}
};

/*
	Tree support
	bool exist(Node* target); // Find if specified node in tree
	std::vector<Node*> path(const Node*); // Find path for specified node
*/
class Tree {
private:
	Node* root;

public:
	Tree(Node*);
	~Tree(void);
	bool exist(const Node*);
	std::stack<Node*> search(const Node*);
	static std::stack<Node*> path(const Node*);

	// Give an iterator that generates a root node to a leaf node
	class Parser {
	private:
		bool running = true;
		DFSTraverse traverser;

	public:
		Parser(Node*);
		bool status(void) const;
		std::stack<Node*> yield(void);
	};

	Tree::Parser leaves(void);
};
