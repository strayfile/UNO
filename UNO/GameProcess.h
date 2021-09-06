#pragma once
#include "Player.h"
#include "WindowDraw.h"

class GameProcess
{
	Player deck; //колода, управляемая ботом - не вытащенные карты
	Player cards; //колода, в которую кладут карты
	vector<Player*> pl; //колоды игроков
	BOOL direction = TRUE;
	size_t nowmove = 0;

	short Games = 0;
	BOOL specCard = 0;
	BOOL needUpdate = TRUE;

	vector<TCHAR*> plNames;
	vector<vector<int>> roundTable;
public:
	HWND hwnd;
	BOOL VsBot = TRUE;
	BOOL checkColor = FALSE;
	BOOL plGo = FALSE;
	BOOL needUno = FALSE;
	int nowcolor = 0; //0 - 3
	BOOL startGame = TRUE;
	TCHAR* mesAboutPlayers = NULL;
	BOOL botGo = FALSE;
	time_t StartTime;
	BOOL nowTimer = false;

	void Start(size_t plNum, BOOL vsWho)
	{
		VsBot = vsWho;
		for (size_t i = 0; i < plNum; i++)  //создание колод игроков(без карт)
		{
			Player* p = new Player;
			pl.push_back(p);
		}
		pl[0]->noBot = true;
		FirstDeck();
		SwapCards(deck);
		GiveOutCards();
		CheckScores();
	}
	void StartRound(){
		FirstDeck();
		SwapCards(deck);
		GiveOutCards();
		CheckScores();
	}
	void StartGame(WindowDraw & wd) //начало процесса игры
	{
		if (!startGame)
			wd.DrawGameField(pl, VsBot, nowcolor);
		GiveOutFirstCard();
		nowcolor = *(cards.c.begin());
		if (!startGame)
			wd.UpdateDetails(pl, direction, nowmove, cards, checkColor, nowcolor);
	}

	void End(WindowDraw & wd, bool end = false)
	{
		wd.DestroyGameField();
		deck.ClearAll();
		cards.ClearAll();
		while (pl.size() != 0)
		{
			delete pl[0];
			pl.erase(pl.begin());
		}
		if (nowTimer)
			KillTimer(hwnd, 1);
		if (mesAboutPlayers != NULL)
		{
			delete[] mesAboutPlayers;
			mesAboutPlayers = NULL;
		}
		direction = TRUE;
		nowmove = 0;
		Games = 0;
		specCard = 0;
		needUpdate = TRUE;
		DeleteRoudTable();
		DeletePlayersNames(end);
		VsBot = TRUE;
		checkColor = FALSE;
		plGo = FALSE;
		needUno = FALSE;
		nowcolor = 0;
		startGame = TRUE;
		botGo = FALSE;
		StartTime = time(NULL);
		nowTimer = false;
	}
	void EndRound(WindowDraw & wd)
	{
		wd.DestroyGameField();
		deck.ClearAll();
		cards.ClearAll();
		CheckScores();
		SetEndRoundScores();
		AddRoundTable();
		if (nowTimer)
			KillTimer(hwnd, 1);
		nowTimer = false;
		for (size_t i = 0; i < pl.size(); i++)
		{
			pl[i]->ClearForNextRound();
		}
		direction = TRUE;
		if (VsBot)
			nowmove = 0;
		if (mesAboutPlayers != NULL)
		{
			delete[] mesAboutPlayers;
			mesAboutPlayers = NULL;
		}
		specCard = 0;
		needUpdate = TRUE;
		plGo = TRUE;              //?
		botGo = FALSE;
		checkColor = FALSE;
		needUno = FALSE;
		nowcolor = 0;
		specCard = FALSE;
		nowmove = 0;
		startGame = TRUE;
		StartTime = time(NULL);
	}

	void ResetDeck()
	{
		if (deck.size < 5)
		{
			for (size_t i = 0; i < cards.size - 5; i++)
			{
				deck.AddCard(cards.c[i], cards.n[i]);
				cards.DelCard(i);
			}
			SwapCards(deck);
		}
	}

	void FirstDeck() //первая колода карт
	{
		for (size_t c = 0; c <= 5; c++) //цвет / поменять цвет и + 4
		{
			if (c <= 3)
			{
				short f = 0;
				for (size_t n = 0; n <= 12; n++)
				{
					deck.AddCard(c, n);
					if (n == 12 && f == 0)
					{
						n = 0;
						f++;
					}
				}
			}
			else if (c == 4 || c == 5)
				for (size_t i = 0; i < 4; i++)
					deck.AddCard(c, 0);
		}
	}
	void SwapCards(Player & plcards) //перемешивание карт в колоде
	{
		int r;
		for (size_t i = 0; i < plcards.size; i++) //0 - 107
		{
			r = rand() % plcards.size;
			plcards.SwapCard(i, r);
		}
	}
	void GiveOutFirstCard()
	{
		for (size_t i = 0; i < deck.size; i++)
		{
			if (!IsSpecCard(deck.c[i], deck.n[i]))
			{
				cards.AddCard(deck.c[i], deck.n[i]);
				deck.DelCard(0);
				break;
			}
		}
	}
	void GiveOutCards() //начальная раздача карт игрокам
	{
		for (size_t i = 0; i < 8; i++) //по 8 карт
			for (size_t j = 0; j < pl.size(); j++) //каждому игроку
				GiveOutCard((*(pl.begin() + j))); //номер игрока
	}
	void GiveOutCard(Player * player)
	{
		player->AddCard((*(deck.c.begin())), (*(deck.n.begin())));
		deck.DelCard(0);
	}
	
	void PutCard(size_t plNum, size_t num, BOOL interrupt) //положить карту игрока в колоду игры
	{
		int c = (*(pl.begin() + plNum))->GetColor(num); //цвет карты, которую кладут
		int n = (*(pl.begin() + plNum))->GetType(num); //номер
		if (IsCorrectCard(*(pl.begin() + plNum), c, n, interrupt)) //проверка, что такую карту можно положить на предыдущую
		{
			cards.AddCard(c, n);
			(*(pl.begin() + plNum))->DelCard(num);
		}
	}

	BOOL HaveNeedColorCard(Player* pl, int color)
	{
		for (auto it : pl->c)
			if (it == color && it <= 3)
				return TRUE;
		return FALSE;
	}
	BOOL IsCorrectCard(Player* pl, int c, int n, BOOL spcard)
	{
		int c1 = cards.GetColor(cards.size - 1);
		int n1 = cards.GetType(cards.size - 1);
		if (!spcard) {
			if (c1 <= 3 && n1 <= 9)
			{
				if (c1 == c)
					if (n <= 12)
						return TRUE;
				if (n == n1) return TRUE;
				if (c == 5 || (c == 4 && !HaveNeedColorCard(pl, c1)))
					return TRUE;
			}
			if (c1 <= 3 && (n1 == 10 || n1 == 11 || n1 == 12))
			{
				if (c1 == c)
					if (n <= 12)
						return TRUE;
				if (n == n1) return TRUE;
				if (c == 5 || (c == 4 && !HaveNeedColorCard(pl, c1)))
					return TRUE;
			}
			if (c1 == 5)
			{
				if (nowcolor == c)
					if (n <= 12)
						return TRUE;
				if (c == 5 || (c == 4 && !HaveNeedColorCard(pl, c1)))
					return TRUE;
			}
			if (c1 == 4)
			{
				if (nowcolor == c)
					if (n <= 12)
						return TRUE;
				if (c == 5 || (c == 4 && !HaveNeedColorCard(pl, c1)))
					return TRUE;
			}
		}
		else if (spcard){
			if (c1 <= 3 && n1 == 10) {
				if (c <= 3 && n == 10)
					return TRUE;
			}
			if (c1 <= 3 && n1 == 12) {
				if ((c <= 3 && n == 12) || c == 4)
					return TRUE; //иногда карту можно перебить +4(в каких-то правилах(и так даже лучше -_-))
			}
			if (c1 == 4 && c == 4)
				return TRUE;
		}
		return FALSE;
	}
	BOOL IsSpecCard(int c, int n)
	{
		if (c <= 3 && n <= 9)
			return FALSE;
		return TRUE;
	}

	void ChangeDirection() //смена направления игры
	{
		if (direction)
			direction = FALSE;
		else direction = TRUE;
	}
	void CheckToSpecCardEffect(WindowDraw & wd)
	{
		int c1 = *(cards.c.begin() + (cards.c.size() - 1));
		int n1 = *(cards.n.begin() + (cards.n.size() - 1));
		if (specCard == 1 && !IsPlayerHaveNeedCard(pl[nowmove], specCard))
		{
			if (c1 <= 3 && n1 == 10) {
				ResetMes(TEXT(" не может отбить и пропускает ход.\n"), pl[nowmove]->name);
				UpdateAfterMove(wd);
			}
			else if (c1 <= 3 && n1 == 12)
			{
				GiveOutCard(pl[nowmove]);
				GiveOutCard(pl[nowmove]);
				ResetMes(TEXT(" берет 2 карты и пропускает ход.\n"), pl[nowmove]->name);
				UpdateAfterMove(wd);
			}
			else if (c1 == 4)
			{
				GiveOutCard(pl[nowmove]);
				GiveOutCard(pl[nowmove]);
				GiveOutCard(pl[nowmove]);
				GiveOutCard(pl[nowmove]);
				ResetMes(TEXT(" берет 4 карты и пропускает ход.\n"), pl[nowmove]->name);
				UpdateAfterMove(wd);
			}
			specCard = 0;
			if (VsBot)
				wd.MessageAboutPlayers(NULL, GetNowPlayerName(), mesAboutPlayers);
		}
	}

	BOOL Move(INT id, WindowDraw & wd)
	{
		id -= ID_СARDS;
		int c = pl[nowmove]->c[id];
		int n = pl[nowmove]->n[id];
		int c1 = *(cards.c.begin() + (cards.c.size() - 1));
		int n1 = *(cards.n.begin() + (cards.n.size() - 1));

		BOOL y = IsCorrectCard(pl[nowmove], c, n, specCard);
		if(y){
			//wd.DestroyMessageAboutPlayers();
			for (size_t i = 0; i < pl.size(); i++)
			{
				if (pl[i]->ForgotUno()){
					GiveOutCard(pl[nowmove]);
					SayUno();
					Update(wd);
					ResetMes(TEXT(" не заметил, что у предыдущего игрока последняя карта. Берет 1 карту.\n"), pl[nowmove]->name);
				}
			}
			PutCard(nowmove, id, 0);
			if (pl[nowmove]->size == 1) {
				needUno = TRUE;
				StartTime = time(NULL);
				SetTimer(hwnd, 1, 100, NULL);
				nowTimer = true;
			}
			if (specCard == 0 && !IsSpecCard(c, n))
				specCard = 0;
			else if (specCard == 0 && (c <= 3 && n == 10))
				specCard = 1;
			else if (specCard == 0 && (c <= 3 && n == 11))
			{
				ChangeDirection();
				if (!needUno)
					needUpdate = TRUE;
				ResetMes(TEXT(" меняет направление.\n"), pl[nowmove]->name);
			}
			else if (specCard == 0 && (c <= 3 && n == 12))
				specCard = 1;
			else if (specCard == 0 && (c == 5))
			{
				checkColor = TRUE;
				needUpdate = FALSE;
				wd.SetNeedColor(1);
			}
			else if (specCard == 1 && (c1 <= 3 && (n1 == 10 || n1 == 12)))
					specCard = 1;
			else if (c == 4)
			{
				checkColor = TRUE;
				needUpdate = FALSE;
				wd.SetNeedColor(1);
				specCard = 1;
			}
			
			if (needUpdate && !needUno)
				UpdateAfterMove(wd);
			else {
				if (!checkColor)
					if (!needUno)
						NextPlayer();
				CheckScores();
			}
			ResetDeck();
			if (nowcolor != c && c <= 3)
			{
				nowcolor = c;
				wd.ChangeNowColor(nowcolor);
			}
			return TRUE;
		}
		return FALSE;
	}
	void BotMove(WindowDraw & wd) {
		wd.DestroyMessageAboutPlayers();
		BotPutCards(wd);
		ResetDeck();
		UpdateAfterMove(wd);
		CheckToSpecCardEffect(wd);
	}
	void BotPutCards(WindowDraw & wd){
		if (IsPlayerHaveNeedCard(pl[nowmove], specCard)) {
			for (size_t i = 0; i < pl.size(); i++)
			{
				if (pl[i]->ForgotUno()){
					SayUno();
					ResetMes(TEXT(" заметил, что у игрока последняя карта.\n"), pl[nowmove]->name);
					Update(wd);
					break;
				}
			}
			int c = 0, n = 0;
			for (size_t i = 0; i < pl[nowmove]->c.size(); i++) {
				if (IsCorrectCard(pl[nowmove], pl[nowmove]->c[i], pl[nowmove]->n[i], specCard)){
					c = pl[nowmove]->GetColor(i);
					n = pl[nowmove]->GetType(i);
					PutCard(nowmove, i, specCard);
					break;
				}
			}
			if (pl[nowmove]->size == 1) {
				BotSayUno(wd); //бот говорит уно
				Update(wd);
			}
			if (specCard == 0 && !IsSpecCard(c, n))
				specCard = 0;
			else if (specCard == 0 && (c <= 3 && n == 10))
				specCard = 1;
			else if (specCard == 0 && (c <= 3 && n == 11))
			{
				ChangeDirection();
				ResetMes(TEXT(" меняет направление.\n"), pl[nowmove]->name);
				wd.MessageAboutPlayers(NULL, GetNextPlayer()->name, mesAboutPlayers);
				ResetMes();
			}
			else if (specCard == 0 && (c <= 3 && n == 12))
				specCard = 1;
			else if (specCard == 0 && (c == 5)) //выбор следующего цвета по присутствующим в колоде бота
			{
				nowcolor = GetColorForBot((*pl[nowmove]));
			}
			else if (specCard == 0 && c == 4)
			{
				specCard = 1;
			}
			else{
				int c1 = *(cards.c.begin() + (cards.c.size() - 1));
				int n1 = *(cards.n.begin() + (cards.n.size() - 1));
				if (specCard == 1 && ((c1 <= 3 && (n1 == 10 || n1 == 12)) || c1 == 4))
					specCard = 1;
			}
			if (nowcolor != c){  //если бот изменил цвет
				if (c <= 3) {
					nowcolor = c;
					wd.ChangeNowColor(nowcolor);
				}
				else {
					ResetMes(TEXT(" изменил цвет.\n"), GetNowPlayerName());
					wd.MessageAboutPlayers(NULL, GetNextPlayer()->name, mesAboutPlayers);
					ResetMes();
				}
			}
		}
		else {
			if (pl[nowmove]->size == 1){
				BotSayUno(wd);
				Update(wd);
			}
			int dCheck = DeckChecked(wd);
			if (dCheck != 1){ //если после взятия карты из колоды есть чем ходить
				BotPutCards(wd);
			}
			else {
				wd.MessageAboutPlayers(GetNowPlayerName(), GetNextPlayer()->name, TEXT(" нечем ходить.\n"));
				ResetMes();
			}
		}
	}

	int GetColorForBot(Player & bot) //поиск цвета, который будет полезен для бота при следующем ходе
	{
		int s = 0;
		for (size_t i = 0; i < bot.c.size(); i++){
			if (bot.c[i] <= 3){
				return bot.c[i];
			}
		}
		return rand() % 4;
	}

	void BotPutCard() {
		for (size_t i = 0; i < pl[nowmove]->c.size(); i++) {
			if (pl[nowmove]->c[i] == cards.c[(cards.c.size()) - 1]) {
				PutCard(nowmove, i, specCard);
				break;
			}
		}
	}

	void UpdateAfterMove(WindowDraw & wd)
	{
		NextPlayer();
		CheckScores();
	}
	void Update(WindowDraw & wd){
		wd.UpdateDetails(pl, direction, nowmove, cards, checkColor, nowcolor);
	}

	Player* GetNextPlayer(){
		if (direction) {
			if (VsBot)
			{
				if (nowmove < pl.size() - 1)
					return pl[nowmove + 1];
				else return pl[0];
			}
			else return pl[1];
		}
		else
		{
			if (VsBot)
			{
				if (nowmove > 0)
					return pl[nowmove - 1];
				else return pl[pl.size() - 1];
			}
			else return pl[pl.size() - 1];
		}
	}
	Player* GetPrevPlayer() {
		if (!direction){
			if (VsBot)
			{
				if (nowmove < pl.size() - 1)
					return pl[nowmove + 1];
				else return pl[0];
			}
			else return pl[1];
		}
		else {
			if (VsBot)
			{
				if (VsBot)
				{
					if (nowmove > 0)
						return pl[nowmove - 1];
					else return pl[pl.size() - 1];
				}
			}
			return pl[pl.size() - 1];
		}
	}
	TCHAR* GetNowPlayerName() {
		return (*(pl.begin() + nowmove))->name;
	}
	TCHAR* GetPrevPlayerName(){
		if (!direction)
			return (*(pl.begin() + 1))->name;
		else return (*(pl.begin() + (pl.size() - 1)))->name;
	}

	void NextPlayer()
	{
		if (!VsBot) {
			if (direction) {
				Player* p = new Player;
				pl.push_back(p);
				(*(pl.begin()))->PlayerCopy((*p));
				pl.erase(pl.begin());
			}
			else {
				Player* p = new Player;
				pl.insert(pl.begin(), p);
				(*(pl.begin() + (pl.size() - 1)))->PlayerCopy((*p));
				pl.erase(pl.begin() + (pl.size() - 1));
			}
		}
		else {
			if (direction) {
				if (nowmove < pl.size() - 1)
					nowmove++;
				else nowmove = 0;
			}
			else {
				if (nowmove > 0)
					nowmove--;
				else nowmove = pl.size() - 1;
			}
		}
		if (pl[nowmove]->size == 1) pl[nowmove]->SayUno();
	}

	void CheckScores()
	{
		for (size_t i = 0; i < pl.size(); i++)
			(*(pl.begin() + i))->CheckScore();
	}
	void SetEndRoundScores(){
		size_t scoreWin = 0;
		int n = 0;
		for (size_t i = 0; i < pl.size(); i++){
			if (pl[i]->size == 0)
				n = i;
			else {
				scoreWin += pl[i]->GetScore();
				pl[i]->SetScore(0);
			}
		}
		pl[n]->SetScore(scoreWin);
	}

	int DeckChecked(WindowDraw & wd)
	{
		if (!IsPlayerHaveNeedCard((pl[nowmove]), specCard))
		{
			if (pl[nowmove]->size == 1)
				pl[nowmove]->UnoNormal();
			GiveOutCard(pl[nowmove]);
			CheckScores();
			if (pl[nowmove]->noBot)
				wd.UpdateDetails(pl, direction, nowmove, cards, checkColor, nowcolor);
			if (!IsPlayerHaveNeedCard((*(pl.begin())), specCard)){
				return 1;
			}
			else return 0;
		}
		return -1;
	}

	BOOL IsPlayerHaveNeedCard(Player * pl, BOOL sCard)
	{
		for (size_t i = 0; i < pl->size; i++)
		{
			if (IsCorrectCard(pl, pl->c[i], pl->n[i], sCard))
				return TRUE;
		}
		return FALSE;
	}

	void OutOfMenuPause(WindowDraw & wd){
		wd.DestroyPauseMenu();
		OutOfPause(wd);
	}
	void OutOfPause(WindowDraw & wd){
		wd.DrawGameField(pl, VsBot, nowcolor);
		wd.UpdateDetails(pl, direction, nowmove, cards, checkColor, nowcolor);
	}
	void OutOfPlPause(WindowDraw & wd, BOOL isItStart)
	{
		wd.DestroyPlPause(isItStart);
		OutOfPause(wd);
	}

	void SetPlayersNames(vector<HWND> &plnames) {
		DeletePlayersNames();

		for (size_t i = 0; i < pl.size() && i < plnames.size(); i++)
		{
			TCHAR * t = new TCHAR[13];
			GetWindowText(plnames[i], t, 13);
			(*(pl.begin() + i))->SetName(t);
			int l = _tcslen((*(pl.begin() + i))->GetName()) + 1;
			TCHAR* tmp = new TCHAR[l];
			for (int j = 0; j < l + 1; j++)
				tmp[j] = t[j];
			plNames.push_back(t);
		}
	}
	void DeletePlayersNames(bool end = false)
	{
		if (!end){
			for (auto it : plNames)
				delete[] it;
		}
		if (plNames.size() > 0)
			plNames.erase(plNames.begin(), plNames.begin() + plNames.size());
	}
	void ClearPlayersNames()
	{
		for (auto it : pl)
			it->DelName();
	}
	BOOL IsCorrectNames(vector<HWND> &plnames) {
		TCHAR** tmp = new TCHAR*[plnames.size()];
		for (size_t i = 0; i < pl.size() && i < plnames.size(); i++)
		{
			size_t l = GetWindowTextLength(plnames[i]);
			if (l < 2){
				delete[] tmp;
				return FALSE;
			}
			TCHAR * t = new TCHAR[l + 1];
			GetWindowText(plnames[i], t, 12);
			if (i != 0){
				for (size_t j = 0; j < i; j++){
					if (_tcscmp(t, tmp[j]) == 0)
					{
						delete[] tmp;
						delete[] t;
						return FALSE;
					}
				}
			}
			tmp[i] = t;
		}
		delete[] tmp;
		return TRUE;
	}

	int WinnerScores(){
		int wS = 0;
		for (size_t i = 0; i < pl.size(); i++) {
			wS += pl[i]->GetScore();
		}
		return wS;
	}
	Player& Winner(){
		for (size_t i = 0; i < pl.size(); i++)
			if (pl[i]->GetScore() == 0)
				return (*pl[i]);
	}
	void AddRoundTable(){
		if (roundTable.size() == 0) {
			for (size_t i = 0; i < pl.size(); i++){
				vector<int> tmp(6);
				for (size_t j = 0; j < tmp.size(); j++){
					tmp[j] = -1;
				}
				roundTable.push_back(tmp);
			}
		}
		for (size_t i = 0; i < pl.size(); i++){
			for (size_t j = 0; j < pl.size(); j++){
				if (_tcscmp(pl[i]->GetName(), plNames[j]) == 0)
				{
					roundTable[j][Games - 1] = pl[i]->GetScore();
					if (Games == 5){
						pl[i]->ClearForNextRound();
						roundTable[j][Games] = pl[i]->GetAllScore();
					}
					break;
				}
			}
		}
	}
	void DeleteRoudTable(){
		if (roundTable.size() > 0)
			roundTable.erase(roundTable.begin(), roundTable.begin() + roundTable.size());
	}

	void ResetMes(TCHAR* mes = NULL, TCHAR* name = NULL){
		if (mes == NULL) {
			delete[] mesAboutPlayers;
			mesAboutPlayers = NULL;
		}
		else {
			delete[] mesAboutPlayers;
			mesAboutPlayers = new TCHAR[_tcslen(mes) + _tcslen(name) + 1];
			mesAboutPlayers[0] = '\0';
			_tcscat(mesAboutPlayers, name);
			_tcscat(mesAboutPlayers, mes);
		}
	}
	TCHAR* AddToPauseMes(TCHAR* pMes){
		if (mesAboutPlayers != NULL) {
			TCHAR* buf = new TCHAR[_tcslen(mesAboutPlayers) + _tcslen(pMes) + 1];
			buf[0] = '\0';
			_tcscat(buf, mesAboutPlayers);
			_tcscat(buf, pMes);
			delete[] mesAboutPlayers;
			mesAboutPlayers = NULL;
			return buf;
		}
		return pMes;
	}

	void SetUno(WindowDraw & wd, bool say) //сказал уно или не успел
	{
		if (!plGo){
			for (size_t i = 0; i < pl.size(); i++)
			{
				if (i != nowmove && pl[i]->ForgotUno()){
					SayUno();
					Update(wd);
					return;
				}
			}
		}
		else {
			if (say)
				SayUno();
			else pl[nowmove]->NoSayUno();
			needUno = FALSE;

			TCHAR * tmpn = new TCHAR[_tcslen(GetNowPlayerName()) + 1];
			_tcscpy(tmpn, GetNowPlayerName());
			UpdateAfterMove(wd);
			CheckToSpecCardEffect(wd);
			TCHAR* buf = NULL;
			if (mesAboutPlayers == NULL) {
				buf = new TCHAR[_tcslen(tmpn) + 37 + 1];
				buf[0] = '\0';
			}
			else {
				if (say)
					buf = new TCHAR[_tcslen(mesAboutPlayers) + _tcslen(tmpn) + 14 + 1];
				else buf = new TCHAR[_tcslen(mesAboutPlayers) + _tcslen(tmpn) + 21 + 1];
				buf[0] = '\0';
				_tcscat(buf, mesAboutPlayers);
			}
			if (say)
				ResetMes(TEXT(" нажал \"UNO\".\n"), tmpn);
			else ResetMes(TEXT(" забыл нажать \"UNO\".\n"), tmpn);
			_tcscat(buf, mesAboutPlayers);
			ResetMes(TEXT(""), buf);
			if (!needUno) {
				plGo = FALSE;
				if (!CheckToEnd(wd)) {
					if (!VsBot){
						wd.DrawPlPause(NULL, GetNowPlayerName(), mesAboutPlayers, 0, FALSE); //пауза после нажатия уно игроком
						ResetMes();
					}
					else {
						if (IsNowBot()){
							botGo = TRUE;
							nowTimer = true;
							SetTimer(hwnd, 1, 1000, NULL);
						}
						else {
							botGo = FALSE;
							Update(wd);
						}
					}
				}
			}
		}
	}
	void SayUno(){
		if (pl[nowmove]->size == 1)
			pl[nowmove]->SayUno();
		for (size_t i = 0; i < pl.size(); i++){
			if (i != nowmove){
				if (pl[i]->GetUno() == 0 || (pl[i]->GetUno() == 1 && pl[i]->size != 1)) {
					if (pl[i]->GetUno() == 0){ //добавить 2 карты игроку
						GiveOutCard(pl[i]);
						GiveOutCard(pl[i]);
						ResetMes(TEXT(" берет 2 карты.\n"), pl[nowmove]->name);
					}
					pl[i]->UnoNormal(); //вернуть как обычно
				}
			}
		}
	}
	void BotSayUno(WindowDraw & wd){
		int u = rand() % 2;
		if (u == 1){
			pl[nowmove]->SayUno();
			ResetMes(TEXT(" нажал \"UNO\".\n"), pl[nowmove]->name);
		}
		else {
			pl[nowmove]->NoSayUno();
			ResetMes(TEXT(" забыл нажать \"UNO\".\n"), pl[nowmove]->name);
		}
		wd.MessageAboutPlayers(NULL, GetNowPlayerName(), mesAboutPlayers);
		ResetMes();
	}

	BOOL IsEndRound() {
		for (auto it : pl)
			if (it->size == 0) return TRUE;
		return FALSE;
	}
	BOOL CheckToEnd(WindowDraw & wd) { //проверка количества карт на завершение игры
		if (IsEndRound()){
			if (VsBot)
				wd.DestroyMessageAboutPlayers();
			Games++;
			EndRound(wd);
			wd.DrawRoundEndPause(pl, Games, roundTable, plNames);
			return TRUE;
		}
		return FALSE;
	}

	INT GetCardsCount(size_t nowpl = 0) const
	{
		return pl[nowpl]->size;
	}
	BOOL IsNowBot(){
		return !pl[nowmove]->noBot;
	}
	size_t GetPlayerCount(){
		return pl.size();
	}
	vector<TCHAR*> GetPlayerNames() const
	{
		return plNames;
	}
	vector<int> GetPlayerScores() const
	{
		vector<int> tmp;
		for (auto it : roundTable)
			tmp.push_back(it[5]);
		return tmp;
	}

	void PlayersToTable(){
		for (size_t i = 0; i < pl.size() - 1; i++)
		{
			for (size_t j = i + 1; j < pl.size(); j++)
			{
				if (roundTable[i][5] < roundTable[j][5])
				{
					TCHAR* tmp = new TCHAR[_tcslen(plNames[i]) + 1];
					for (size_t x = 0; x < _tcslen(plNames[i]) + 1; x++)
						tmp[x] = plNames[i][x];
					delete[] plNames[i];
					plNames[i] = new TCHAR[_tcslen(plNames[j]) + 1];
					for (size_t x = 0; x < _tcslen(plNames[j]) + 1; x++)
						plNames[i][x] = plNames[j][x];
					delete[] plNames[j];
					plNames[j] = new TCHAR[_tcslen(tmp) + 1];
					for (size_t x = 0; x < _tcslen(tmp) + 1; x++)
						plNames[j][x] = tmp[x];

					vector<int> t = roundTable[i];
					roundTable[i] = roundTable[j];
					roundTable[j] = t;
				}
			}
		}
	}
	void NextRandomRound(WindowDraw & wd){
		Games++;
		int n = rand() % pl.size();
		pl[n]->SetScore(rand() % 190 + 10);
		for (size_t j = 0; j < pl.size(); j++){
			if (j != n)
				pl[j]->SetScore(0);
		}
		AddRoundTable();
		for (auto it : pl)
			it->ClearForNextRound();
		wd.DestroyRoundEndPause();
		wd.DrawRoundEndPause(pl, Games, roundTable, plNames);
	}
	void EndNowRound(WindowDraw & wd){
		int r = rand() % pl.size();
		while (pl[r]->c.size() != 0)
			pl[r]->DelCard(0);
		CheckToEnd(wd);
	}
};

