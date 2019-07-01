#pragma once

#include "card.h"
#include "iplayer.h"
#include "counter.h"

void Counter<global::players>::moveout(std::vector<iCard*>& set, iCard*& target) {
	std::vector<iCard*>::iterator iter = std::find(set.begin(), set.end(), target);
	if (iter == set.end()) return;
	set.erase(iter);
}

Counter<global::players>::Counter(
	CardManager& manager, iCard* trump) {
	this->_desk.clear();
	this->_unknown = manager.getall();
	this->manager = &manager;
	for (int index = 0; index < global::players; index++) {
		this->players[index] = nullptr;
		this->count[index] = 0;
		this->inhand[index] = std::vector<iCard*>();
	}

	// Moveout trump
	this->moveout(this->_unknown, trump);
}

Counter<global::players>::Counter(const Counter<global::players>& counter) {
	this->_desk = counter._desk;
	this->_unknown = counter._unknown;;
	this->players = counter.players;
	this->count = counter.count;
	this->inhand = counter.inhand;
}

int Counter<global::players>::join(iPlayer* player) {
	int index = 0;
	for (; index < global::players; index++)
		if (this->players[index] == nullptr) {
			this->players[index] = player;
			return index;
		}
	return index;
}

iPlayer* Counter<global::players>::player(int index) const {
	if (index > int(this->players.max_size()) - 1)
		return nullptr;
	return this->players[index];
}

iPlayer* Counter<global::players>::player(std::string name) const {
	for (iPlayer* player : this->players)
		if (player->name() == name)
			return player;
	return nullptr;
}

void Counter<global::players>::grab(iPlayer* player) {
	int index = player->index();
	std::vector<iCard*>& inhand = this->inhand[index];
	std::vector<iCard*>& desk = this->_desk;

	this->count[index] += desk.size();
	for (auto& card : desk) inhand.push_back(card);
	desk.clear();
}

void Counter<global::players>::replenish(iPlayer* player, int count){
	int index = player->index();
	int current = this->count[index];
	if (count > current)
		this->count[index] = count;
}

void Counter<global::players>::get(iPlayer* player, iCard* card) {
	int index = player->index();
	std::vector<iCard*>& inhand = this->inhand[index];

	this->count[index] += 1;
	inhand.push_back(card);
	this->moveout(this->_unknown, card);
}

void Counter<global::players>::hit(iPlayer* player, iCard* card) {
	int index = player->index();
	std::vector<iCard*>& inhand = this->inhand[index];

	this->moveout(inhand, card); // for us
	this->moveout(this->_unknown, card); // for enemy
	this->count[index] -= 1;
	this->_desk.push_back(card);
}

void Counter<global::players>::get(iPlayer* player, std::string& rank, std::string& suit) {
	int index = player->index();
	iCard* card = this->manager->get(rank, suit);
	std::vector<iCard*>& inhand = this->inhand[index];
	
	this->count[index] += 1;
	inhand.push_back(card);
	this->moveout(this->_unknown, card);
}

void Counter<global::players>::hit(iPlayer* player, std::string& rank, std::string& suit) {
	int index = player->index();
	iCard* card = this->manager->get(rank, suit);
	std::vector<iCard*>& inhand = this->inhand[index];

	this->moveout(inhand, card); // for us
	this->moveout(this->_unknown, card); // for enemy
	this->count[index] -= 1;
	this->_desk.push_back(card);
}

std::vector<iCard*>& Counter<global::players>::desk(void) { return this->_desk; }
std::vector<iCard*>& Counter<global::players>::unknown(void) { return this->_unknown; }
void Counter<global::players>::clear(void) { this->_desk.clear(); }
std::vector<iCard*>& Counter<global::players>::hand(iPlayer* player) { return this->inhand[player->index()]; }
int Counter<global::players>::left(iPlayer* player) { return this->count[player->index()]; }
