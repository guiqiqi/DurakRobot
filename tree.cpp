
#include "tree.h"
#include "data.h"

Node::Node(void) :
	_degree(0), data(nullptr)
{}

Node::Node(const Data& data) :
	_degree(0), data(new Data(data)) {}

Node::~Node(void) {
	delete this->data;
	this->_degree = 0;
	for (auto& iter : this->children)
		delete iter;
}

std::vector<Node*>::const_iterator Node::begin(void) const {
	return this->children.begin();
}

std::vector<Node*>::const_iterator Node::end(void) const {
	return this->children.end();
}

// Add from initialized node pointer
void Node::add(Node*& child) {
	child->_father = this;
	this->children.push_back(child);
	this->_degree += 1;
}

// Remove child elements from the specified location
void Node::remove(const int index) {
	this->children.erase(this->children.begin() + index);
}

// Add from a Data copy
void Node::add(const Data& data) {
	Node* child = new Node(data);
	child->_father = this;
	this->children.push_back(child);
	this->_degree += 1;
}

// Return data of current node
Data* Node::get(void) const {
	return this->data;
}

// Return degree of current node
int Node::degree(void) const {
	return this->_degree;
}

// Return father node for current node
Node* Node::father(void) const {
	return this->_father;
}

// Access support by index
Node* Node::operator[] (int index) {
	return this->children[index];
}

const Node* Node::operator[] (int index) const {
	return this->children[index];
}

/* ----------------------------------------------------- */

Tree::Tree(Node* root) : root(root) {}
Tree::~Tree(void) {
	delete this->root;
}

// Find if specified node in tree
bool Tree::exist(const Node* target) {
	BFSTraverse traverser(this->root);

	while (traverser.status()) {
		Node* current = traverser.yield();
		if (current == root)
			return true;
	}

	return false;
}

std::stack<Node*> Tree::search(const Node* node) {
	if (this->exist(node)) {
		return this->path(node);
	}
	else {
		return std::stack<Node*>();
	}
}

std::stack<Node*> Tree::path(const Node* node) {
	std::stack<Node*> path;
	Node* current = const_cast<Node*>(node);
	while (true) {
		path.push(current);
		current = current->father();
		if (current == nullptr) break;
	}
	return path;
}

// Give an iterator that generates a root node to a leaf node
Tree::Parser::Parser(Node* root) : traverser(DFSTraverse(root)) {}
bool Tree::Parser::status(void) const { return this->running; }

std::stack<Node*> Tree::Parser::yield(void) {
	while (this->traverser.status()) {
		Node* current = traverser.yield();
		if (current == nullptr) break;

		// If current has no child - it is a leaf node
		if (!current->degree())
			return Tree::path(current);
	}

	// If all paths has been traversed, return empty list
	this->running = false;
	return std::stack<Node*>();
}

Tree::Parser Tree::leaves(void) {
	return Tree::Parser(this->root);
}
