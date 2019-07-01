#pragma once

#include "header.h"

struct Card;

class PlayerAbstract
{
protected:


public:
	virtual ~PlayerAbstract() {};
	virtual void YouTurn(bool) = 0;
	virtual void PutCard() = 0;
	virtual void TakeCards() = 0;
	virtual void GetHeadTrick() = 0;
	virtual void TakeOneCard(Card*& nc) = 0;
	virtual void ShowCards() = 0;
	virtual bool INeedCard() = 0;
	virtual int GetCardNum() = 0;
};

// Support for iCard*'s hash
namespace std {
	template<>
	class hash<iCard*> {
	public:
		std::size_t operator()(const iCard* ptr) const {
			size_t sptr = (size_t)ptr;
			return sptr & (0X7FFFFFFF);
		}
	};
}

class Player : public PlayerAbstract {

private:
	// Mapping table between external card and internal card
	iPlayer* weinenemy;
	std::unordered_map<iCard*, Card*> cardmap;
	bool ending = false;

protected:
	iCard* trump = nullptr;
	Rule* analyser = nullptr;
	Ranker* ranker = nullptr;
	CardManager* manager = nullptr;
	Counter<global::players>* counter = nullptr;
	iPlayer* enemy = nullptr;
	Thinker* thinker = nullptr;

	iPlayer* we;
	
public:
	Player(const char* name, CardManager* manager);
	~Player(void);
	void YouTurn(bool);
	void PutCard(void);
	void TakeCards(void);
	void GetHeadTrick(void);
	void TakeOneCard(Card*& nc);
	void ShowCards(void);
	bool INeedCard(void);
	int GetCardNum(void);

public:
	void setwe(iPlayer* enemy); // Get our player's address
	iPlayer* getenemy(void) const; // Set the address of the imaginary enemy
};
