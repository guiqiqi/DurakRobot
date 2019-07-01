#pragma once

#include "header.h"

class Thinker {
private:
	iPlayer* we;
	iPlayer* enemy;
	Counter<global::players>* situation;

private:
	Tree* maketree(bool status, iCard* card);
	static double ranksum(bool starting, std::stack<Node*>& path);
	static iCard* byrank(iPlayer*& player);
	static void showpath(std::stack<Node*> path, double rank);

public:
	Thinker(iPlayer* we, iPlayer* enemy,
		Counter<global::players>* situation);

public:
	iCard* attack(void);
	iCard* attack(int);
	iCard* defend(iCard* action);
};

// Node for prediction, using in task queue
struct Prediction {
	bool mode; // attack or defense
	int layer; // current prediction's layer
	iCard* card; // operate card
	Node* father; // father node
	Counter<global::players>* situation; // layer
};

class TreeMaker {
private:
	Node* root = nullptr; // root node
	int maxlayer = global::depth;
	iPlayer* attacker; iPlayer* defender;

private:
	std::queue<Prediction*> tasks; // tasks queue
	static Node* complex(iCard*& card, iPlayer*& player);
	std::vector<Counter<global::players>*> trashbin;

public:
	TreeMaker(bool mode, iCard* rootcard, iPlayer* attacker, 
		iPlayer* defender, Counter<global::players>* situation);
	~TreeMaker(void);

public:
	Tree* make(void); // remember to delete tree after use!
};
