
#include "card.h"
#include "tree.h"
#include "data.h"
#include "counter.h"
#include "iplayer.h"
#include "thinker.h"

Thinker::Thinker(iPlayer* we, iPlayer* enemy,
	Counter<global::players>* situation) :
	we(we), enemy(enemy), situation(situation)
{}

/* 
	Calculate the weight of the path and 
	the sum of the probabilities;
	The probability value is in the interval[0, 1],
	multiplied by the weight, and the path sum is obtained.

	Record our weight loss as a negative number and 
	the enemy's record as a positive number. 
	The larger the calculated result value, the better.
*/
double Thinker::ranksum(bool starting, std::stack<Node*>& opath) {
	// make a copy for path
	double sum = 0; bool status = starting;
	std::stack<Node*> path(opath);

	while (!path.empty()) {
		Node* node = path.top();
		Data* data = node->get();
		double loss = data->possibility * data->rank;

		if (status)
			sum -= loss;
		else
			sum += loss;

		path.pop();
		status = !status;
	}
	
	return sum;
}

iCard* Thinker::byrank(iPlayer*& player) {
	std::vector<iCard*> list = player->analysis();

	// If list is empty return nullptr
	if (list.empty()) return nullptr;

	iCard* result = list[0];
	double minrank = player->rank(list[0]);

	for (auto& card : list) {
		double rank = player->rank(card);
		std::cout << *card << " - " << rank << std::endl;
		if (rank < minrank) {
			minrank = rank;
			result = card;
		}
	}

	return result;
}

Tree* Thinker::maketree(bool status, iCard* card) {
	TreeMaker maker(status, card, this->we, this->enemy, this->situation);
	Tree* tree = maker.make();

	this->we->set_counter(this->situation);
	this->enemy->set_counter(this->situation);

	return tree;
}

void Thinker::showpath(std::stack<Node*> path, double rank) {
	while (!path.empty()) {
		Node* node = path.top();
		Data* data = node->get();
		std::cout << *(data->card) << " - ";
		path.pop();
	}
	std::cout << rank << std::endl;
}

/*
	If we are offensive:
		If there is no card on the table:
			Give the card with the least weight
		If there is a card on the table:
			Analyze the situation based on all possibilities
	If we are defensive:
		Analyze the situation based on 
		the cards the enemy has already played.
*/
iCard* Thinker::attack(void) {
	return this->byrank(this->we); 
}

iCard* Thinker::attack(int) {
	// record all ways could be
	std::map<double, iCard*> record;

	std::vector<iCard*> list = this->we->analysis();
	// if no cards can be play, return nullptr
	if (list.empty()) return nullptr;

	for (auto& card : list) {
		Tree* tree = this->maketree(global::attack, card);
		Tree::Parser parser = tree->leaves();

		while (parser.status()) {
			std::stack<Node*> path = parser.yield();
			if (path.empty()) continue;

			double rank = this->ranksum(global::attack, path);
			this->showpath(path, rank);
			record[rank] = path.top()->get()->card;
		}

		delete tree;
	}

	// Return the branch with the highest weight
	auto it = record.end(); it--;
	return it->second;
}

iCard* Thinker::defend(iCard* action) {
	std::map<double, iCard*> record;
	if (action == nullptr) return nullptr;

	Tree* tree = this->maketree(global::defend, action);
	Tree::Parser parser = tree->leaves();

	while (parser.status()) {
		std::stack<Node*> path = parser.yield();

		double rank = this->ranksum(global::defend, path);

		// if there is no possible cards
		// it means that we could not handle attack in this prediction
		if (path.empty()) continue;
		path.pop(); // for defend mode we need find second one
		if (path.empty()) continue;

		this->showpath(path, rank);

		record[rank] = path.top()->get()->card;
	}

	delete tree;
	// if no cards can be play, return nullptr
	if (record.empty()) return nullptr;

	// Return the branch with the lowest weight
	//auto it = record.end(); it--;
	auto it = record.begin();
	return it->second;
}

Node* TreeMaker::complex(iCard*& card, iPlayer*& player) {
	// This function use card to create Node in ***heap***
	double rank, possibility;
	if (card != nullptr) {
		rank = player->rank(card) * global::rank_factor;
		possibility = player->posibility(card) * global::possibility_factor;
	} else {
		rank = 0.0;
		possibility = 0.0;
	}
	Data data(card, rank, possibility);
	Node* node = new Node(data);
	return node;
}

TreeMaker::TreeMaker(
	bool mode, iCard* rootcard, iPlayer* attacker, 
	iPlayer* defender, Counter<global::players>* situation
) {
	this->trashbin.clear();
	this->attacker = attacker;
	this->defender = defender;

	// generate root node
	if (mode == global::attack)
		this->root = this->complex(rootcard, attacker);
	else
		this->root = this->complex(rootcard, defender);

	// generate prediction branch
	Prediction* prediction = new Prediction;
	prediction->layer = 0;
	prediction->mode = mode;
	prediction->card = rootcard;
	prediction->father = nullptr;
	prediction->situation = new Counter<global::players>(*situation);
	this->trashbin.push_back(prediction->situation);

	// add prediction to tasks queue
	this->tasks.push(prediction);
}

TreeMaker::~TreeMaker(void) {
	// release all Counter's memory
	for (auto& counter : this->trashbin)
		delete counter;
	this->trashbin.clear();
}

/* Using BFS algorithm to generate pretree */
Tree* TreeMaker::make(void) {
	// Produce with tasks queue
	while (!this->tasks.empty()) {

		// Get prediction job
		Prediction* prediction = this->tasks.front();
		bool mode = prediction->mode;
		int layer = prediction->layer;
		iCard* card = prediction->card;
		Node* father = prediction->father;
		Counter<global::players>* situation = prediction->situation;
		this->tasks.pop();

		// Get player
		iPlayer* player; iPlayer* passive;
		if (mode == global::attack) {
			player = this->attacker;
			passive = this->defender;
		}
		else {
			player = this->defender;
			passive = this->attacker;
		}

		// Make node and insert to father while not root
		Node* child;
		if (father == nullptr) {
			child = this->root;
		}
		else {
			child = this->complex(card, player);
			father->add(child);
		}

		// Make predictions and add new tasks
		{
			this->attacker->set_counter(situation);
			this->defender->set_counter(situation);
			player->hit(card);

			/*
				If current prediction layer miner global::depth,
				analyze possible cards in the current situation;
				and also exchange offense and defense.

				Otherwise, stop prediction.
			*/
			if (layer < this->maxlayer)
				for (auto& card : passive->analysis()) {
					Prediction* prediction = new Prediction;
					prediction->card = card;
					prediction->mode = !mode;
					prediction->layer = layer + 1;
					prediction->father = child;
					prediction->situation = \
						new Counter<global::players>(*situation);
					this->trashbin.push_back(prediction->situation);
					this->tasks.push(prediction);
				}
		}

		// Release memeory
		delete prediction;
	}

	Tree* tree = new Tree(this->root);
	return tree;
}
