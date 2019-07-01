
#include "card.h"

int iCard::compare(const iCard* first, const iCard* second) {
	return (global::iranks[first->_rank] - global::iranks[second->_rank]);
}

iCard::iCard(std::string suit, std::string rank) {
	this->_rank = rank; this->_suit = suit;
}

std::string iCard::rank(void) const {
	return this->_rank;
}

std::string iCard::suit(void) const {
	return this->_suit;
}

bool iCard::operator>(const iCard*& card) {
	int diff = this->compare(this, card);
	if (diff > 0) return true;
	return false;
}
bool iCard::operator<(const iCard*& card) {
	int diff = this->compare(this, card);
	if (diff < 0) return true;
	return false;
}
bool iCard::operator>=(const iCard*& card) {
	int diff = this->compare(this, card);
	if (diff >= 0) return true;
	return false;
}
bool iCard::operator<=(const iCard*& card) {
	int diff = this->compare(this, card);
	if (diff <= 0) return true;
	return false;
}
bool iCard::operator==(const iCard*& card) {
	int diff = this->compare(this, card);
	if (diff == 0) return true;
	return false;
}

std::ostream& operator<<(std::ostream& out, const iCard& card) {
	out << card._suit << ' ' << card._rank;
	return out;
}

CardManager::CardManager(void) {
	for (auto& suit : global::suits) {
		for (auto& rank : global::ranks) {
			iCard* card = new iCard(suit, rank);
			auto key = std::make_pair(suit, rank);
			this->library[key] = card;
			this->all.push_back(card);
		}
	}
}

// Free all memory
CardManager::~CardManager(void) {
	for (auto& card : this->all)
		delete card;
}

// Get copy for all cards
std::vector<iCard*> CardManager::getall(void) {
	return this->all;
}

// Get the memory reference of the specified card
iCard* CardManager::get(std::string& suit, std::string& rank) {
	auto key = std::make_pair(suit, rank);

	// If found card
	if (this->library.find(key) != this->library.end())
		return this->library[key];

	return nullptr;
}

iCard* CardManager::get(std::pair<std::string, std::string>& key) {
	return this->library[key];
}
