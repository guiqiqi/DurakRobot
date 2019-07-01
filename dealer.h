#pragma once

enum { PAS = 300, NOCARD = 400 };
//PAS - ���ڧ٧ߧѧ� �������ܧ� ���է� (�ߧ֧�֧� ���էڧ�� �ڧݧ� �ܧ����)
//NOCARD - ���ڧ٧ߧѧ� �������ܧ� ���է� (�ߧ֧� �ܧѧ�� �ߧ� ���ܧѧ�)

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
	static Card* trump;  // �ܧѧ��� - �ܧ�٧���
	static Card* noCard, * pasCard; // �ܧѧ��� - ���ڧ٧ߧѧ� "�ߧ֧� �ܧѧ��" �� "��ѧ�"
	static Card deck[maxSuits * maxRanks]; //�ܧ�ݧ�է�

	static bool tableRanks[maxRanks];   // ��ѧߧԧ� �ܧѧ��, ���ڧ�����ӧ���ڧ� �ߧ� ����ݧ�

	static int currentHeadTrik; //�ߧ�ާ֧� ���է� �� �ܧ�ߧ�
	static Card* headTrick[2][maxTrick]; // ����� [0] - ���� �ڧԧ��ܧ�, [1] - ���ҧ�� �ܧѧ���
	static void GenerateDeck();



public:

	//��֧�֧ާ֧�ѧ�� �ܧ�ݧ�է� - �ڧߧڧ�ڧѧݧڧ٧ڧ��֧� �ܧ�ݧ�է� �� �ӧ�� ��֧�֧ާ֧ߧߧ��.
	//�ӧ�ҧڧ�ѧ֧� �ܧ�٧���
	static void ShuffleDec();
	//�ӧ٧��� �ܧѧ��� �ڧ� �ܧ�ݧ�է�. �ӧ�٧ӧ�ѧ�ѧ֧� true, �֧�ݧ� �ܧѧ��� �֧�� ����ѧݧڧ��.
	static bool GetCard(Card*& outCard);
	//�ӧ�٧ӧ�ѧ�ѧ֧� ��֧ܧ��֧ԧ� �ܧ�٧��� �� �����ܧ���� Card (�ڧާ֧֧� �٧ߧѧ�֧ߧڧ� ���ݧ�ܧ� �ާѧ���).
	static Card* GetTrump();
	// �ӧ�٧ӧ�ѧ�ѧ֧� ��ڧ�ݧ� �ӧ��֧է�ڧ� �ڧ� �ܧ�ݧ�է� �ܧѧ��
	static int getcurrentCard();

	//�ӧ�٧ӧ�ѧ�ѧ֧� ��ܧѧ٧ѧ�֧ݧ� �ߧ� �����

	static Card* (*GetheadTrick())[maxTrick];

	//�ӧ�٧ӧ�ѧ�ѧ֧� �����ܧ�ӧ�� �ݧڧ�֧�ѧ� ���է֧�اѧ�ڧ� �ާѧ��� �ڧݧ� �է����ڧߧ��ӧ� �ܧѧ���
	static const char* SuitName(const Card* card);
	static const char* RankName(const Card* card);

	// * �ӧ�٧ӧ�ѧ�ѧ֧� �ڧߧէ֧ܧ� �����ӧ֧���ӧ���ڧ� �ާѧ��� �ڧݧ� �է����ڧߧ��ӧ� �ܧѧ���
	static int SuitIndex(const Card* card);
	static int RankIndex(const Card* card);
	// �ӧ�٧ӧ�ѧ�ѧ֧� �ߧ�ާ֧� ���է� (0-5)
	static int GetCurrentHeadTrik();
	// �����ӧ֧��֧� �ӧ�٧ާ�ا֧� �ݧ� ��ݧ֧է���ڧ� ���� (���է��<6,���ҧڧӧѧ��ڧۧ�� �ڧԧ��� �ߧ� ���ѧ��ӧѧ�)
	static bool NextTrikEnable();


	// * �ӧ�ӧ�էڧ� �ߧ� ��ܧ�ѧ� �ܧѧ��� �ڧݧ� �����
	static void ShowCard(const Card* card);
	static void ShowTable();
	//* �ӧ�٧ӧ�ѧ�ѧ֧� ��ܧѧ٧ѧ�֧ݧ� �ߧ� �ܧѧ��� "��ѧ�" �ڧݧ� "�ߧ֧� �ܧѧ���"
	static Card* GetPas();
	static Card* GetNocard();
	// �ӧ�٧ӧ�ѧ�ѧ֧� ��ܧѧ٧ѧ�֧ݧ� �ߧ� ����ݧ֧էߧ�� �ܧѧ��� �� �ܧ������ ���էڧݧ� �� �ܧ������ ���ҧڧӧѧݧڧ��
	static Card* GetLastCard();
	static Card* GetLastDefendCard();


	//���ڧ�ѧ֧� �����
	static void ClearTable();
	//����ӧ֧��֧� �ܧѧ��� �ߧ� ����ݧ� �ߧ� �ܧ���֧ܧ�ߧ����
	static bool CheckHeadTrick();

	//�ѧ�ѧܧ�ӧѧ�� �ڧԧ��ܧ� �ܧѧ���� card
	static void Attack(Card* card);
	//���ܧ���� �ѧ�ѧܧ����� �ܧѧ���, �ܧѧ���� card
	static void Defend(Card* card);

	~Dealer();
};