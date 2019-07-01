#pragma once

#include "header.h"

class iPlayer {
private:
	int _index; // our index
	bool _we; // its our player
	bool _turn; // our turn or not
	bool _status; // attack or defend
	std::string _name;

	Rule* analyser;
	Ranker* ranker;
	Counter<global::players>* counter;

public:
	int total(void); // return total count of our cards
	std::vector<iCard*>& desk(void); // return ref for desk cards
	std::vector<iCard*>& hand(void); // return ref for inhand's cards
	std::vector<iCard*>& unknown(void); // return ref for unknown cards

public:
	iPlayer(bool we, bool turn, bool status, std::string name);
	bool us(void) const; // return if it's our player
	bool turn(void) const; // return if it's our turn
	bool status(void) const; // return status for player
	int index(void) const; // return index
	std::string name(void) const; // return name
	void set_index(int);
	void set_counter(Counter<global::players>* counter); // reset counter for think
	void set_analyser(Rule* analyser); // set analyser
	void set_ranker(Ranker* ranker); // set ranker

public:
	void grab(void); // grab all cards
	void replenish(void); // get some cards we dont know
	void get(iCard*); // get one card
	void hit(iCard*); // hit one card
	void get(std::string&, std::string&); // get one card
	void hit(std::string&, std::string&); // hit one card

public:
	std::vector<iCard*> analysis(void); // analysis we could play
	double posibility(const iCard* card); // return for one card's posibility
	double rank(const iCard* card); // return for one card's rank

public:
	void transform(void); // takein/out our turn's control
	void toggle(void); // change our attack/defend mode
};
	