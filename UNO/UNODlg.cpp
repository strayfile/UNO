#include "UNODlg.h"

UNODlg* UNODlg::ptr = NULL;

UNODlg::UNODlg(void)
{
	ptr = this;
}

void UNODlg::Cls_OnClose(HWND hwnd)
{
	DestroyWindow(hwnd);
	PostQuitMessage(0);
}

BOOL UNODlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	srand(unsigned(time(0)));
	hDialog = hwnd;
	gp.hwnd = hwnd;
	wd.hDialog = hDialog;
	wd.hInst = GetModuleHandle(NULL);
	HICON hIcon = LoadIcon(wd.hInst, MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	wd.ReadOptions();
	wd.SetBack();
	wd.SetHeadMenu();
	return TRUE;
}

void UNODlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == ID_MENU_PAUSE) //����� ����
	{
		wd.DestroyGameField();
		wd.SetPauseMenu();
	}
	if (id == ID_CONTINUE) //����������� ���� ����� �����
	{
		gp.OutOfMenuPause(wd);
	}
	if (id == ID_MENU_EXIT || id == ID_PAUSEEXIT) //����� �� �������� ����
	{
		if (ID_PAUSEEXIT)
			wd.DestroyPauseMenu();
		gp.End(wd); //������� ������ ����
		gp.ClearPlayersNames();
		wd.SetHeadMenu();
	}
	if (id >= ID_�ARDS && id <= (ID_�ARDS + gp.GetCardsCount()) && !gp.checkColor && !gp.botGo) //������� �� �����
	{
		if (!gp.checkColor && !gp.needUno){
			if (gp.Move(id, wd))
			{
				if (!gp.checkColor)
					gp.plGo = TRUE;
				gp.Update(wd);
				if (!gp.checkColor && !gp.needUno) {
					gp.plGo = FALSE;
					if (gp.VsBot) gp.botGo = TRUE;
					gp.CheckToSpecCardEffect(wd);
					if (!gp.CheckToEnd(wd)) {
						if (!gp.VsBot) {
							wd.DrawPlPause(NULL, gp.GetNowPlayerName(), gp.mesAboutPlayers, 0, FALSE);
							gp.ResetMes();
						}
						else{
							wd.MessageAboutPlayers(NULL, gp.GetNowPlayerName(), gp.mesAboutPlayers);
							gp.ResetMes();
							if (gp.IsNowBot()){
								gp.nowTimer = true;
								SetTimer(hwnd, 1, 1000, NULL);
							}
							else {
								gp.botGo = FALSE;
								gp.Update(wd);
							}
						}
					}
				}
			}
		}
	}
	if (id == ID_DECK && !gp.checkColor && !gp.needUno && !gp.botGo) //������� �� ������
	{
		int dCheck = gp.DeckChecked(wd);
		if (dCheck == 1) //���� ����� ������ ����� �� ������ ����� ������
		{
			gp.UpdateAfterMove(wd); //��������� �����
			if (!gp.VsBot)
				wd.DrawPlPause(gp.GetPrevPlayerName(), gp.GetNowPlayerName(), TEXT(" ����� ������.\n"), 0, FALSE);
			else {
				wd.MessageAboutPlayers(gp.GetPrevPlayerName(), gp.GetNowPlayerName(), TEXT(" ����� ������.\n"));
				if (gp.IsNowBot()){
					gp.botGo = TRUE;
					gp.nowTimer = true;
					SetTimer(hwnd, 1, 1000, NULL);
				}
				else {
					gp.botGo = FALSE;
					gp.Update(wd);
				}
			}
		}
		else if (dCheck == 0){
			gp.Update(wd);
		}
	}
	if (id >= ID_COLORS1 && id <= ID_COLORS4 && gp.checkColor && !gp.botGo) //����� ����� ��������� ����
	{
		gp.plGo = TRUE;
		wd.SetNeedColor(0);
		gp.checkColor = FALSE;
		if (id == ID_COLORS1)
			gp.nowcolor = 0;
		if (id == ID_COLORS2)
			gp.nowcolor = 1;
		if (id == ID_COLORS3)
			gp.nowcolor = 2;
		if (id == ID_COLORS4)
			gp.nowcolor = 3;
		TCHAR * tmpn = new TCHAR[_tcslen(gp.GetNowPlayerName()) + 1];
		_tcscpy(tmpn, gp.GetNowPlayerName());
		if (!gp.needUno) {
			gp.UpdateAfterMove(wd);
			gp.CheckToSpecCardEffect(wd);
		}
		else {
			gp.needUno = TRUE;
			gp.StartTime = time(NULL);
			gp.nowTimer = true;
			SetTimer(hwnd, 1, 100, NULL);
		}
		
		TCHAR* buf = NULL;
		if (gp.mesAboutPlayers == NULL) {
			buf = new TCHAR[_tcslen(tmpn) + 15 + 1];
			buf[0] = '\0';
		}
		else {
			buf = new TCHAR[_tcslen(gp.mesAboutPlayers) + _tcslen(tmpn) + 15 + 1];
			buf[0] = '\0';
			_tcscat(buf, gp.mesAboutPlayers);
		}
		gp.ResetMes(TEXT(" ������� ����.\n"), tmpn);
		_tcscat(buf, gp.mesAboutPlayers);
		gp.ResetMes(TEXT(""), buf);
		if (!gp.needUno) {
			gp.plGo = FALSE;
			if (!gp.CheckToEnd(wd)) {
				if (!gp.VsBot){
					wd.DrawPlPause(NULL, gp.GetNowPlayerName(), gp.mesAboutPlayers, 0, FALSE); //����� ����� ������ ����� �������
					gp.ResetMes();
				}
				else {
					wd.MessageAboutPlayers(NULL, gp.GetNowPlayerName(), gp.mesAboutPlayers);
					if (gp.IsNowBot()){
						gp.botGo = TRUE;
						gp.nowTimer = true;
						SetTimer(hwnd, 1, 1000, NULL);
					}
					else {
						gp.botGo = FALSE;
						gp.Update(wd);
					}
				}
			}
		}
	}
	if (id == ID_UNO && !gp.checkColor) //������ ���
	{
		gp.SetUno(wd, true);
	}
	if (id == ID_MENU3_CHECK1) //���� ������ ����
	{
		gp.VsBot = TRUE;
		wd.RedrawPlayMenu(1);
	}
	else if (id == ID_MENU3_CHECK2) //���� ������ ��������
	{
		gp.VsBot = FALSE;
		wd.RedrawPlayMenu(0);
	}

	if (id == ID_NEXT_PLAYER) //��������� �����
	{
		if (gp.startGame) {
			if (gp.IsCorrectNames(wd.plnames))
				gp.SetPlayersNames(wd.plnames);
			else return;
		}
		gp.OutOfPlPause(wd, gp.startGame);
		if (gp.startGame)
			gp.startGame = FALSE;
	}
	if (id == ID_NEXT_ROUND) //��������� �����
	{
		wd.DestroyRoundEndPause();
		gp.StartRound();
		gp.startGame = TRUE;
		gp.StartGame(wd);
		if (!gp.VsBot)
			wd.DrawPlPause(NULL, gp.GetNowPlayerName(), TEXT(""), 0, FALSE);
		else gp.OutOfPause(wd);
		gp.startGame = FALSE;
	}
	if (id == ID_MAGIC){
		gp.NextRandomRound(wd);
	}
	if (id == ID_END_NOW_ROUND){
		gp.EndNowRound(wd);
	}
	if (id == ID_END_GAME)
	{
		wd.DestroyRoundEndPause();
		gp.PlayersToTable();
		AddToRecordsTable(gp.GetPlayerNames(), gp.GetPlayerScores());
		gp.End(wd, true); //������� ������ ����
		gp.ClearPlayersNames();
		wd.SetHeadMenu();
	}
	if (id == ID_MENU0_PLAY || id == ID_MENU0_OPTIONS || id == ID_MENU0_HELP) //����� �� �������� ����
	{
		wd.DestroyHeadMenu();
	}
	else if (id == ID_MENU1_BACK || id == ID_MENU1_SAVE) //����� �� ��������
	{
		ClearRecords();
		wd.DestroyOptMenu();
		wd.SetHeadMenu();
	}
	else if (id == ID_MENU2_BACK) //����� �� "������ ����"
	{
		wd.DestroyHelpMenu();
		wd.SetHeadMenu();
	}
	else if (id == ID_MENU3_BACK || id == ID_MENU3_PLAY) //����� �� "������"
	{
		if (id == ID_MENU3_BACK) //�����
		{
			wd.DestroyPlayMenu();
			wd.SetHeadMenu();
		}
		else //���������� ���� ���� - ������
		{
			INT p = SendMessage(wd.spinplcount, UDM_GETPOS32, 0, 0);
			if (p < 2)
				p = 2;
			gp.Start(p, gp.VsBot); //���-�� �������, ��� ������ �������/�����
			wd.DestroyPlayMenu();
			gp.StartGame(wd);
			wd.DrawPlPause(NULL, NULL, NULL, p, TRUE);
		}
	}
	if (id == ID_MENU0_PLAY) //������
	{
		wd.SetPlayMenu(0);
	}
	else if (id == ID_MENU0_OPTIONS) //�������
	{
		ReadRecordsTable();
		wd.SetOptMenu(players, scores);
	}
	else if (id == ID_CLEAR_RECORDS) //c������� ����������
	{
		ClearRecords();
		SaveRecordsTable();
		wd.ClearOptMenu();
	}
	else if (id == ID_MENU0_HELP) //������/�������
	{
		wd.SetHelpMenu();
	}
	else if (id == ID_MENU0_EXIT) //�����
		EndDialog(hDialog, 0);
}

void UNODlg::Cls_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	wd.SetSpinPlCount(SendMessage(wd.spinplcount, UDM_GETPOS32, 0, 0));
}

void UNODlg::Cls_OnTimer(HWND hwnd, UINT id)
{
	if (gp.needUno){
		time_t unotime = time(NULL) - gp.StartTime;
		struct tm Time = *(localtime(&unotime));
		if (Time.tm_sec > 3){
			KillTimer(hwnd, 1);
			gp.nowTimer = false;
			gp.SetUno(wd, false);
		}
	}
	else if (gp.VsBot){
		KillTimer(hwnd, 1);
		gp.nowTimer = false;
		Sleep(400);
		if (gp.botGo){
			gp.BotMove(wd);
			gp.Update(wd);
			if (!gp.CheckToEnd(wd)) {
				if (gp.IsNowBot()) {
					gp.nowTimer = true;
					SetTimer(hwnd, 1, 1000, NULL);
				}
				else gp.botGo = FALSE;
			}
		}
	}
}

BOOL CALLBACK UNODlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
		HANDLE_MSG(hwnd, WM_VSCROLL, ptr->Cls_OnVScroll);
		HANDLE_MSG(hwnd, WM_TIMER, ptr->Cls_OnTimer);
	}
	return FALSE;
}

void UNODlg::AddToRecordsTable(vector<TCHAR*> pl, vector<int> sc){
	ReadRecordsTable();
	players.push_back(pl);
	scores.push_back(sc);
	SortPlayersInRecordsTable();
	SaveRecordsTable();
}

void UNODlg::SaveRecordsTable(){
	ofstream fout("records.txt", ios_base::out | ios::trunc);
	if (fout.is_open())
	{
		for (size_t i = 0; i < players.size() && i < scores.size() && i < 6; i++){
			for (size_t j = 0; j < players[i].size() && j < scores[i].size(); j++)
			{
				int length = WideCharToMultiByte(CP_ACP, 0, players[i][j], -1, NULL, 0, 0, 0);
				char *name = new char[length];
				WideCharToMultiByte(CP_ACP, 0, players[i][j], -1, name, length, 0, 0);
				fout.write(name, length - 1);
				delete[] name;
				fout.write("\n", 1);
				TCHAR t[11] = { 0 };
				_itot_s(scores[i][j], t, 10);

				length = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, 0, 0);
				char *score = new char[length];
				WideCharToMultiByte(CP_ACP, 0, t, -1, score, length, 0, 0);
				fout.write(score, length - 1);
				delete[] score;
				fout.write("\n", 1);
			}
			fout.write("\n\n", 1);
		}
		fout.close();
		ClearRecords();
	}
}

void UNODlg::ClearRecords(){
	if (players.size() != 0){
		for (auto pl1 : players){
			for (auto pl2 : pl1){
				delete[] pl2;
			}
		}
		players.erase(players.begin(), players.begin() + players.size());
	}
	if (scores.size() != 0)
		scores.erase(scores.begin(), scores.begin() + scores.size());
}

void UNODlg::ReadRecordsTable(){
	ifstream fin("records.txt", ios_base::in);
	if (fin.is_open())
	{
		char str[51];
		while (!fin.eof())
		{
			fin.getline(str, 50);
			vector<TCHAR*> tmp1;
			vector<int> tmp2;
			while (str[0] != '\0'){
				int length = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
				wchar_t *p1 = new wchar_t[length];
				MultiByteToWideChar(CP_ACP, 0, str, -1, p1, length);
				tmp1.push_back(p1);
				fin.getline(str, 50);
				tmp2.push_back(atoi(str));
				fin.getline(str, 50);
			}
			if (tmp1.size() != 0 && tmp2.size() != 0)
			{
				players.push_back(tmp1);
				scores.push_back(tmp2);
			}
			if (players.size() > 6)
				break;
		}
		fin.close();
	}
}

void UNODlg::SortPlayersInRecordsTable(){
	for (size_t i = 0; i < scores.size() - 1; i++)
	{
		for (size_t j = i + 1; j < scores.size(); j++)
		{
			if (scores[i][0] < scores[j][0])
			{
				vector<TCHAR*> tmp1 = players[i];
				players[i] = players[j];
				players[j] = tmp1;

				vector<int> tmp2 = scores[i];
				scores[i] = scores[j];
				scores[j] = tmp2;
			}
		}
	}
}