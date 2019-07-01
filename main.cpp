
#include "vld.h"

#include "card.h"
#include "dealer.h"
#include "player.h"
#include "translator.h"

int main()
{
	CardManager manager;
	int p1win = 0, p2win = 0, drawn = 0;
	for (int count = 0; count < 10; count++)
	{
		Dealer::ShuffleDec();

		Card* trump = Dealer::GetTrump();
		std::cout << "Trump: " << *icard(&manager, trump) << std::endl;

		Card* temp;
		Player p1("Konstantin", &manager);
		Player p2("Grigory", &manager);

		{
			p1.setwe(p2.getenemy()); 
			p2.setwe(p1.getenemy());
		}

		PlayerAbstract* player1;
		PlayerAbstract* player2;
		PlayerAbstract* tmp;

		if (count % 2)
		{
			p1.YouTurn(true);
			p2.YouTurn(false);
			player1 = &p1;
			player2 = &p2;

		}
		else
		{
			p1.YouTurn(false);
			p2.YouTurn(true);
			player1 = &p2;
			player2 = &p1;

		}

		//	Player *player = p2;

		while (player1->INeedCard())
		{
			Dealer::GetCard(temp);
			player1->TakeOneCard(temp);
		}
		while (player2->INeedCard())
		{
			Dealer::GetCard(temp);
			player2->TakeOneCard(temp);
		}

		int triknum = 1;
		while (
			(player1->GetCardNum() && player2->GetCardNum()) 
							|| 
			Dealer::getcurrentCard() < 52
			)
		{
			std::cout << "Round " << triknum++ << ": " << std::endl;
			while (player1->INeedCard())
			{
				Dealer::GetCard(temp);
				player1->TakeOneCard(temp);
			}
			while (player2->INeedCard())
			{
				Dealer::GetCard(temp);
				player2->TakeOneCard(temp);
			}

			player1->ShowCards();
			player2->ShowCards();
			while (Dealer::NextTrikEnable())
			{
				//player1->ShowCards();
				//player2->ShowCards();

				player1->PutCard();
				//Dealer::ShowTable();
				player2->GetHeadTrick();
				//Dealer::ShowTable();
			}
			Dealer::ShowTable();
			int kht = Dealer::GetCurrentHeadTrik();
			// §á§â§à§Ó§Ö§â§Ú§ä?§Ó§ã?§á§à §á§Ñ§â§Ñ?
			if (Dealer::CheckHeadTrick()) // §Ü§Ñ§â§ä?§à§ä§Ò§Ú§ä§í §Ó§Ö§â§ß?
			{
				Card* plastDefCard = Dealer::GetLastDefendCard();
				Card* plastCard = Dealer::GetLastCard();
				if (plastDefCard != nullptr)
				{
					// §Ö§ã§İ§Ú §á§à§ã§İ§Ö§Õ? §Ü§Ñ§â§ä?§à§ä§Ò§à§ñ  "§á§Ñ? - §Ó§ä§à§â§à§Û §Ù§Ñ§Ò§Ú§â§Ñ§Ö§ä §Ü§Ñ§â§ä?§ã§Ö§Ò§Ö
					if (Dealer::RankIndex(plastDefCard) == PAS)
					{
						player2->TakeCards();
						Dealer::ClearTable();
					}
					// §Ö§ã§İ§Ú §á§à§ã§İ§Ö§Õ?§ñ §Ü§Ñ§â§ä??§Ü§à§ä§à§â§à?§ç§à§Õ§Ú§İ§Ú - §á§Ñ?§Ú§İ?§ß§Ö?§Ü§Ñ§â§ä - §á§Ö§â§Ö§ç§à?§ç§à§Õ§Ñ, §à§ä§Ò§à?
					else if ((Dealer::RankIndex(plastCard) == PAS) ||
						(Dealer::RankIndex(plastCard) == NOCARD) ||
						(Dealer::GetCurrentHeadTrik() == 6)
						)
					{
						Dealer::ClearTable();
						player2->YouTurn(true);
						player1->YouTurn(false);
						tmp = player2;
						player2 = player1;
						player1 = tmp;
					}
				}
			}
			else
			{
				std::cout << "error";
			}
		}

		if (!player1->GetCardNum() && !player2->GetCardNum())
		{
			drawn++;
			std::cout << "\ndrawn game\n";
		}
		else if (!player1->GetCardNum())
		{
			std::cout << "\np1 - win\n";
			if (player1 == &p1)
				p1win++;
			else
				p2win++;
		}

		else if (!player2->GetCardNum())
		{
			std::cout << "\np2 - win\n";
			if (player2 == &p2)
				p2win++;
			else
				p1win++;
		}

		// §Ö§ã§İ§Ú §á§à§ã§İ§Ö§Õ?§ñ §Ü§Ñ§ä§â?- §á§Ñ?- §Ó§ä§à§â§à§Û §Ù§Ñ§Ò§Ú§â§Ñ§Ö§ä

		player1->ShowCards();
		player2->ShowCards();

		std::cout << "\nKonstantin - " << p1win << " Grigory - " << p2win << " drawn " << drawn;
	}

	return 0;
}
