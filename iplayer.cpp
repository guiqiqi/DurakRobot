
#include "card.h"
#include "rule.h"
#include "ranker.h"
#include "iplayer.h"
#include "counter.h"

std::vector<iCard*> iPlayer::analysis(void) {
	std::vector<iCard*> total;
	
	// The cards on our hands are known
	if (this->us()) total = this->hand();
	// For enemy need calculate
	else {
		std::vector<iCard*>& hand = this->hand();
		std::vector<iCard*>& unknown = this->unknown();
		total.insert(total.end(), hand.begin(), hand.end());
		total.insert(total.end(), unknown.begin(), unknown.end());
	}
	
	// analysis for possible cards
	std::vector<iCard*>& desk = this->desk();
	if (this->status() == global::attack)
		return this->analyser->attack(desk, total);
	else
		return this->analyser->defend(desk, total);
}

double iPlayer::posibility(const iCard* card) {
	
	std::vector<iCard*>& hand = this->hand();
	std::vector<iCard*>& unknown = this->unknown();
	std::vector<iCard*>::iterator finder;

	// For us if card in hand - 1, otherwise - 0
	if (this->us()) {
		finder = std::find(hand.begin(), hand.end(), card);
		if (finder == hand.end()) return 0.;
		return 1.;
	}

	// For enemy if card in hand - 1
	finder = std::find(hand.begin(), hand.end(), card);
	if (finder != hand.end()) return 1.;

	// If card not in unknown, return 0;
	finder = std::find(unknown.begin(), unknown.end(), card);
	if (finder == unknown.end()) return 0.;

	// Otherwise, we need calculate posibility
	// let :
	//		unknown.size() = a
	//		this.enemy.size() = b
	//		this.enemy_total = c
	//	m = c - b; n = a + m = a + c - b
	//	The problem is equivalent to extracting m from n objects 
	//  and looking for the specified one;
	//  
	//  Its probability is equal to :
	//		C(n-1, m-1)/C(n, m) = m / n = (c - b) / (a + c - b)

	double a = unknown.size();
	double b = hand.size();
	double c = this->total();

	return (c - b) / (a + c - b);
}

double iPlayer::rank(const iCard* card) {

	double init = 1.0;
	init *= ranker->absolute(card);
	init *= ranker->trump(card);
	init *= ranker->progress();

	// If it's enemy's player
	if (!this->us()) return init;

	// if it's our player
	double repeat = ranker->repeat(this, card);
	if (this->status() == global::attack)
		init -= init * repeat;
	else
		init += init * repeat;
	return init;
}

int iPlayer::total(void) { return this->counter->left(this); }
std::vector<iCard*>& iPlayer::desk(void) {return this->counter->desk();}
std::vector<iCard*>& iPlayer::hand(void) { return this->counter->hand(this); }
std::vector<iCard*>& iPlayer::unknown(void) { return this->counter->unknown(); }
bool iPlayer::turn(void) const { return this->_turn; }
bool iPlayer::status(void) const { return this->_status; }
int iPlayer::index(void) const { return this->_index; }
std::string iPlayer::name(void) const { return this->_name; }
void iPlayer::set_index(int index) { this->_index = index; }
void iPlayer::set_counter(Counter<global::players>* counter) { this->counter = counter; }
void iPlayer::set_analyser(Rule* analyser) { this->analyser = analyser; }
void iPlayer::set_ranker(Ranker* ranker) { this->ranker = ranker; }
bool iPlayer::us(void) const { return this->_we; }
iPlayer::iPlayer(bool we, bool turn, bool status, std::string name) :
	_we(we), _turn(turn), _status(status), _name(name), _index(0),
	analyser(nullptr), ranker(nullptr), counter(nullptr) {
	
}

void iPlayer::grab(void) { this->counter->grab(this); }
void iPlayer::replenish(void) { this->counter->replenish(this, global::fullcard); }
void iPlayer::get(iCard* card) { this->counter->get(this, card); }
void iPlayer::hit(iCard* card) { this->counter->hit(this, card); }
void iPlayer::get(std::string& suit, std::string& rank) { this->counter->get(this, suit, rank); }
void iPlayer::hit(std::string& suit, std::string& rank) { this->counter->hit(this, suit, rank); }

void iPlayer::transform(void) { this->_turn = !this->_turn; }
void iPlayer::toggle(void) { this->_status = !this->_status; }
