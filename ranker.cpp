
#include "card.h"
#include "ranker.h"
#include "counter.h"
#include "iplayer.h"

// Scale the function value field to [1,2]
double Ranker::zipvalue(double value) {
	return atan(value) / (0.5 * M_PI) + 1; 
}

Ranker::Ranker(Counter<global::players>* counter, const iCard* trump) :
	_counter(counter), _trump(trump) {}

// Add weight to the trump card
double Ranker::trump(const iCard* card) {
	if (card->suit() == this->_trump->suit())
		return global::trump_factor;
	return 1.0;
}

// Calc absolute rank for card
double Ranker::absolute(const iCard* card) {
	std::string rank = card->rank();
	double raw = global::iranks[rank];
	return global::unconstrained_factor * raw;
}

// Calc for progress coefficent
double Ranker::progress(void) {
	double all = global::total;
	double left = this->_counter->unknown().size();
	double progress = all / left;
	return this->zipvalue(progress);
}

// Calc repeat rank for attack and defend
double Ranker::repeat(iPlayer* player, const iCard* target) {
	int count = 0;
	std::string rank = target->rank();
	std::vector<iCard*>& inhand = player->hand();
	for (auto& card : inhand)
		if (card->rank() == rank)
			count++;

	return global::repetition_factor * count;
}
