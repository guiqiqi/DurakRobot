#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <map>
#include <cmath>
#include <ctime>
#include <stack>
#include <array>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <unordered_map>

namespace global {
	constexpr auto players = 2;

	/* Constants for card.h */
	static const std::array<std::string, 4> suits = { "Clubs", "Spades", "Hearts", "Diamonds" };
	static const std::array<std::string, 13> ranks = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };
	static std::unordered_map<std::string, short> iranks = {
		{"2", 0}, {"3", 1}, {"4", 2}, {"5", 3}, {"6", 4}, {"7", 5}, {"8", 6},
		{"9", 7}, {"10", 8}, {"Jack", 9}, {"Queen", 10}, {"King", 11}, {"Ace", 12}
	};

	constexpr auto suits_count = 4;
	constexpr auto ranks_count = 13;
	constexpr auto total = suits_count * ranks_count; // Total number of cards

	/* Constants for rule.h */
	constexpr bool attack = true;
	constexpr bool defend = false;

	/* Constants for situation.h */
	constexpr auto fullcard = 6;
	constexpr auto our = true;
	constexpr auto enemys = false;

	/* Constants for thinker.h */
	constexpr auto depth = 4;
	constexpr auto possibility_factor = 0.5;
	constexpr auto rank_factor = 1.2;

	/* Constants for ranker.h */
	constexpr auto unconstrained_factor = 0.1;
	constexpr auto trump_factor = 5;
	constexpr auto repetition_factor = 0.15;
}

struct Data;
class Node;
class Tree;

class Rule;
class Ranker;
class iCard;
class iPlayer;
class Thinker;
class Situation;
class CardManager;

template <int count>
class Counter;

static std::string clubs = global::suits[0];
static std::string spades = global::suits[1];
static std::string hearts = global::suits[2];
static std::string diamonds = global::suits[3];
static std::string c2 = global::ranks[0];
static std::string c3 = global::ranks[1];
static std::string c4 = global::ranks[2];
static std::string c5 = global::ranks[3];
static std::string c6 = global::ranks[4];
static std::string c7 = global::ranks[5];
static std::string c8 = global::ranks[6];
static std::string c9 = global::ranks[7];
static std::string c10 = global::ranks[8];
static std::string jack = global::ranks[9];
static std::string queen = global::ranks[10];
static std::string king = global::ranks[11];
static std::string ace = global::ranks[12];
