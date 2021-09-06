#pragma once
#include "Header.h"

class Player
{
	size_t score = 0;
	size_t scoreAll = 0;
	int uno = -1; //-1 - не использовалось, 0 - забыли сказать, 1 - сказали
	BOOL skipStroke = FALSE;
public:
	TCHAR* name = NULL;
	vector<int> c; //цвет, +4 или поменять цвет //0, 1, 2, 3
	vector<int> n; //номер или тип
	size_t size = 0; //кол-во карт
	bool noBot = false;

	void PlayerCopy(Player & pl)
	{
		pl.score = score;
		pl.scoreAll = scoreAll;
		pl.uno = uno;
		pl.skipStroke = skipStroke;
		pl.SetName(name);
		pl.c = c;
		pl.n = n;
		pl.size = size;
		pl.noBot = noBot;
	}

	void SetName(TCHAR * n)
	{
		name = new TCHAR[_tcslen(n) + 1];
		_tcscpy(name, n);
	}

	TCHAR* GetName() const
	{
		return name;
	}

	void DelName()
	{
		delete[] name;
		name = NULL;
	}

	void AddCard(int color, int nums)
	{
		c.push_back(color);
		n.push_back(nums);
		size++;
	}

	void SwapCard(size_t num1, size_t num2)
	{
		int i = *(c.begin() + num2);
		int j = *(n.begin() + num2);
		*(c.begin() + num2) = *(c.begin() + num1);
		*(n.begin() + num2) = *(n.begin() + num1);
		*(c.begin() + num1) = i;
		*(n.begin() + num1) = j;
	}

	void DelCard(size_t num)
	{
		c.erase(c.begin() + num);
		n.erase(n.begin() + num);
		size--;
	}

	void DelCard(int color, int nums)
	{
		for (size_t i = 0; i < size; i++)
		{
			if ((*(c.begin() + i)) == color && (*(n.begin() + i)) == nums)
			{
				c.erase(c.begin() + i);
				n.erase(n.begin() + i);
				size--;
			}
		}
	}

	void CheckScore()
	{
		score = 0;
		for (size_t i = 0; i < c.size(); i++)
		{
			score += GetOneScore(c[i], n[i]);
		}
	}

	int GetOneScore(int c1, int n1) //очки карты
	{
		if ((c1 >= 0 && c1 <= 3) && (n1 >= 0 && n1 <= 9))
			return n1;
		else if (c1 == 4 || c1 == 5)
			return 50;
		else return 20;
	}

	void AddAllScore()
	{
		scoreAll += score;
	}

	void SetScore(size_t s){
		score = s;
	}
	int GetScore() const
	{
		return score;
	}
	int GetAllScore() const{
		return scoreAll;
	}

	int GetColor(size_t num)
	{
		return *(c.begin() + num);
	}
	int GetType(size_t num)
	{
		return *(n.begin() + num);
	}

	void SayUno()
	{
		uno = 1;
	}
	void NoSayUno()
	{
		uno = 0;
	}
	void UnoNormal()
	{
		uno = -1;
	}

	BOOL ForgotUno()
	{
		if (uno == 0) return TRUE;
		return FALSE;
	}

	int GetUno() const {
		return uno;
	}

	void ClearAll()
	{
		score = 0;
		scoreAll = 0;
		uno = -1;
		skipStroke = FALSE;
		if (c.size() > 0)
		{
			c.clear();
			n.clear();
		}
		size = 0;
		delete[] name;
		name = NULL;
		noBot = false;
	}
	void ClearForNextRound(){
		AddAllScore();
		score = 0;
		uno = -1;
		skipStroke = FALSE;
		if (c.size() > 0)
		{
			c.clear();
			n.clear();
		}
		size = 0;
	}
};