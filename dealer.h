#pragma once

enum { PAS = 300, NOCARD = 400 };
//PAS - §á§â§Ú§Ù§ß§Ñ§Ü §á§â§à§á§å§ã§Ü§Ñ §ç§à§Õ§Ñ (§ß§Ö§é§Ö§Þ §ç§à§Õ§Ú§ä§î §Ú§Ý§Ú §Ü§â§í§ä§î)
//NOCARD - §á§â§Ú§Ù§ß§Ñ§Ü §á§â§à§á§å§ã§Ü§Ñ §ç§à§Õ§Ñ (§ß§Ö§ä §Ü§Ñ§â§ä §ß§Ñ §â§å§Ü§Ñ§ç)

static const char* suits[] = { "Clubs", "Spades", "Hearts", "Diamonds" };
static const char* suitsSymb[] = { "\x5", "\x6", "\x3", "\x4" };
static const char* ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };

struct Card
{
private:
	int suit;
	int rank;

	bool operator >(const Card& card);
	friend class Dealer;

	Card(int suit = -1, int rank = -1);
public:

};


class Dealer
{
public:
	static const int maxSuits = 4;
	static const int maxRanks = 13;
	static const int maxTrick = 6;
private:
	static int currentCard;
	static Card* trump;  // §Ü§Ñ§â§ä§Ñ - §Ü§à§Ù§í§â§î
	static Card* noCard, * pasCard; // §Ü§Ñ§â§ä§Ñ - §á§â§Ú§Ù§ß§Ñ§Ü "§ß§Ö§ä §Ü§Ñ§â§ä" §Ú "§á§Ñ§ã"
	static Card deck[maxSuits * maxRanks]; //§Ü§à§Ý§à§Õ§Ñ

	static bool tableRanks[maxRanks];   // §â§Ñ§ß§Ô§Ú §Ü§Ñ§â§ä, §á§â§Ú§ã§å§ä§ã§ä§Ó§å§ð§ë§Ú§ç §ß§Ñ §ã§ä§à§Ý§Ö

	static int currentHeadTrik; //§ß§à§Þ§Ö§â §ç§à§Õ§Ñ §Ó §Ü§à§ß§å
	static Card* headTrick[2][maxTrick]; // §ã§ä§à§Ý [0] - §ç§à§Õ §Ú§Ô§â§à§Ü§Ñ, [1] - §à§ä§Ò§à§Û §Ü§Ñ§â§ä§í
	static void GenerateDeck();



public:

	//§á§Ö§â§Ö§Þ§Ö§ê§Ñ§ä§î §Ü§à§Ý§à§Õ§å - §Ú§ß§Ú§è§Ú§Ñ§Ý§Ú§Ù§Ú§â§å§Ö§ä §Ü§à§Ý§à§Õ§å §Ú §Ó§ã§Ö §á§Ö§â§Ö§Þ§Ö§ß§ß§í§Ö.
	//§Ó§í§Ò§Ú§â§Ñ§Ö§ä §Ü§à§Ù§í§â§ñ
	static void ShuffleDec();
	//§Ó§Ù§ñ§ä§î §Ü§Ñ§â§ä§å §Ú§Ù §Ü§à§Ý§à§Õ§í. §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä true, §Ö§ã§Ý§Ú §Ü§Ñ§â§ä§í §Ö§ë§Ö §à§ã§ä§Ñ§Ý§Ú§ã§î.
	static bool GetCard(Card*& outCard);
	//§Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §ä§Ö§Ü§å§ë§Ö§Ô§à §Ü§à§Ù§í§â§ñ §Ó §ã§ä§â§å§Ü§ä§å§â§Ö Card (§Ú§Þ§Ö§Ö§ä §Ù§ß§Ñ§é§Ö§ß§Ú§Ö §ä§à§Ý§î§Ü§à §Þ§Ñ§ã§ä§î).
	static Card* GetTrump();
	// §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §é§Ú§ã§Ý§à §Ó§í§ê§Ö§Õ§ê§Ú§ç §Ú§Ù §Ü§à§Ý§à§Õ§í §Ü§Ñ§â§ä
	static int getcurrentCard();

	//§Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §å§Ü§Ñ§Ù§Ñ§ä§Ö§Ý§î §ß§Ñ §ã§ä§à§Ý

	static Card* (*GetheadTrick())[maxTrick];

	//§Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §ã§ä§â§à§Ü§à§Ó§í§Û §Ý§Ú§ä§Ö§â§Ñ§Ý §ã§à§Õ§Ö§â§Ø§Ñ§ë§Ú§Û §Þ§Ñ§ã§ä§î §Ú§Ý§Ú §Õ§à§ã§ä§à§Ú§ß§ã§ä§Ó§à §Ü§Ñ§â§ä§í
	static const char* SuitName(const Card* card);
	static const char* RankName(const Card* card);

	// * §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §Ú§ß§Õ§Ö§Ü§ã §ã§à§à§ä§Ó§Ö§ä§ã§ä§Ó§å§ð§ë§Ú§Û §Þ§Ñ§ã§ä§Ú §Ú§Ý§Ú §Õ§à§ã§ä§à§Ú§ß§ã§ä§Ó§å §Ü§Ñ§â§ä§í
	static int SuitIndex(const Card* card);
	static int RankIndex(const Card* card);
	// §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §ß§à§Þ§Ö§â §ç§à§Õ§Ñ (0-5)
	static int GetCurrentHeadTrik();
	// §±§â§à§Ó§Ö§â§ñ§Ö§ä §Ó§à§Ù§Þ§à§Ø§Ö§ß §Ý§Ú §ã§Ý§Ö§Õ§å§ð§ë§Ú§Û §ç§à§Õ (§ç§à§Õ§à§Ó<6,§à§ä§Ò§Ú§Ó§Ñ§ð§ë§Ú§Û§ã§ñ §Ú§Ô§â§à§Ü §ß§Ö §ã§á§Ñ§ã§à§Ó§Ñ§Ý)
	static bool NextTrikEnable();


	// * §Ó§í§Ó§à§Õ§Ú§ä §ß§Ñ §ï§Ü§â§Ñ§ß §Ü§Ñ§â§ä§å §Ú§Ý§Ú §ã§ä§à§Ý
	static void ShowCard(const Card* card);
	static void ShowTable();
	//* §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §å§Ü§Ñ§Ù§Ñ§ä§Ö§Ý§î §ß§Ñ §Ü§Ñ§ä§â§å "§á§Ñ§ã" §Ú§Ý§Ú "§ß§Ö§ä §Ü§Ñ§â§ä§í"
	static Card* GetPas();
	static Card* GetNocard();
	// §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§Ö§ä §å§Ü§Ñ§Ù§Ñ§ä§Ö§Ý§î §ß§Ñ §á§à§ã§Ý§Ö§Õ§ß§ð§ð §Ü§Ñ§â§ä§å §ã §Ü§à§ä§à§â§à§Û §ç§à§Õ§Ú§Ý§Ú §Ú §Ü§à§ä§à§â§à§Û §à§ä§Ò§Ú§Ó§Ñ§Ý§Ú§ã§î
	static Card* GetLastCard();
	static Card* GetLastDefendCard();


	//§à§é§Ú§ë§Ñ§Ö§ä §ã§ä§à§Ý
	static void ClearTable();
	//§á§â§à§Ó§Ö§â§ñ§Ö§ä §Ü§Ñ§â§ä§í §ß§Ñ §ã§ä§à§Ý§Ö §ß§Ñ §Ü§à§â§â§Ö§Ü§ä§ß§à§ã§ä§î
	static bool CheckHeadTrick();

	//§Ñ§ä§Ñ§Ü§à§Ó§Ñ§ä§î §Ú§Ô§â§à§Ü§Ñ §Ü§Ñ§â§ä§à§Û card
	static void Attack(Card* card);
	//§á§à§Ü§â§í§ä§î §Ñ§ä§Ñ§Ü§å§ð§ë§å§ð §Ü§Ñ§â§ä§å, §Ü§Ñ§â§ä§à§Û card
	static void Defend(Card* card);

	~Dealer();
};