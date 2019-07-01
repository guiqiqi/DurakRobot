#pragma once

#include "card.h"
#include "header.h"

#include "dealer.h"
#include "card.h"

// translate card* to icard*
static iCard* icard(CardManager* manager, const Card* card) {
	if ((card == Dealer::GetNocard())
		||
		(card == Dealer::GetPas())
		||
		(card == nullptr)
		) {
		return nullptr;
	}
	std::string suit = Dealer::SuitName(card);
	std::string rank = Dealer::RankName(card);
	iCard* target = manager->get(suit, rank);
	return target;
}
