//
// ִ��ս��
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#include "stdafx.h"
#include <stdlib.h>
#include "MainWin.h"
#include "File.h"
#include "Misc.h"
#include "Battle.h"
#include "Lexer.h"
#include "less.h"

#define	ATTACK_WAVE		"A5_04103"
#define	FIREBALL_WAVE	"A5_03068"
#define	HEAL_WAVE		"A5_03092"
#define	LEVELUP_WAVE	"A5_06171"

//
// ���캯��
//
CBattleAction::CBattleAction(CMainWin *win, CAction *oldAction):
	CAction(win, oldAction), _map_data(0), map_data(0),
	ViewImage(win->GetViewImage()),
	MixedImage(win->GetMixedImage()),
	BackLayer(win->GetBackLayer())
{
	status = STATUS_NONE;
	turn = PLAYER_TURN;

	// ��ɫ��Ϣ����ʾ
	for (int i=0; i<MAX_CHARACTER; i++) {
		StatusCharacter[i] = 0;
	}

	// ��ǰ���͵Ĳ���
	hFont = CreateFont(-16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "����");
}

//
// ��������
//
CBattleAction::~CBattleAction()
{
	DeleteObject(hFont);

	delete _map_data;
	delete map_data;
}

//
// ��ͣ
//
void CBattleAction::Pause()
{
}

//
// ��ͣ���
//
void CBattleAction::Resume()
{
}

//
// ����������ʱִ�еĴ���
//
void CBattleAction::LButtonDown(UINT modKeys, CPoint point)
{
	// ֻ���ֵ�����Ϸ��ɫ���е���갴ť��Ӧ
	if (turn == PLAYER_TURN) {
		CPoint pos = PointToIndex(point);	// ��Ҫ��ͼ����
		switch (status) {
		  case STATUS_NONE:
			// ѡȡ��Ϸ��ɫ
			if (MapRect.PtInRect(pos) && map_data[pos.y][pos.x].type & PLAYER) {
				SelPos = pos;
				SelChar = (CCharacter *)map_data[SelPos.y][SelPos.x].sprite;
				ShowCommand(point);
			}
			break;

		  case STATUS_COMMAND:
			SelectCommand(point);
			break;

		  case STATUS_MOVE:
			// ��ѡʱ���ƶ���Χ
			if (MapRect.PtInRect(pos) && map_data[pos.y][pos.x].type & MOVEDIST) {
				MovePlayer(SelChar, pos);
				SelChar->move_done = true;
			}
			break;

		  case STATUS_ATTACK:
			// �ѵ�ѡ����Ŀ��
			if (MapRect.PtInRect(pos) && (map_data[pos.y][pos.x].type & (ATTACKDIST | ENEMY)) == (ATTACKDIST | ENEMY)) {
				Attack(SelChar, (CCharacter *)map_data[pos.y][pos.x].sprite, true);
				SelChar->attack_done = true;
			}
			break;

		  case STATUS_MAGIC_ENEMY:
			// ��ѡʱ��ħ����Χ
			if (MapRect.PtInRect(pos) && (map_data[pos.y][pos.x].type & (ATTACKDIST | ENEMY)) == (ATTACKDIST | ENEMY)) {
				Magic(SelChar, (CCharacter *)map_data[pos.y][pos.x].sprite, true);
				SelChar->attack_done = true;
			}
			break;

		  case STATUS_MAGIC_PLAYER:
			// ��ѡʱ��ħ����Χ
			if (MapRect.PtInRect(pos) && (map_data[pos.y][pos.x].type & (ATTACKDIST | PLAYER)) == (ATTACKDIST | PLAYER)) {
				Magic(SelChar, (CCharacter *)map_data[pos.y][pos.x].sprite, true);
				SelChar->attack_done = true;
			}
			break;

		  case STATUS_ANIME:
			break;

		  case STATUS_NEXTTURN:
			// ��ѡTurn End(����)����
			if (TurnEndSprite.PtIn(point)) {
				status = STATUS_NONE;
				HideTurnEnd();
				ChangeTurn(ENEMY_TURN);
			}
			break;
		}
	}
}

//
// ��������Ҽ�ʱִ�еĴ���
//
void CBattleAction::RButtonDown(UINT modKeys, CPoint point)
{
	// �����Ҽ����صĶ���
	if (turn == PLAYER_TURN) {
		switch (status) {
		  case STATUS_NONE:				// ��û��ѡȡ�κζ���ʱ
			status = STATUS_NEXTTURN;	// ��ʾ����һ�֡�(NEXT TURN)
			ShowTurnEnd(point);
			break;

		  case STATUS_COMMAND:			// ��ѡȡ��ָ��ʱ
			status = STATUS_NONE;		// �趨״̬Ϊ���ޡ�
			HideCommand();
			break;

		  case STATUS_MOVE:				// �ƶ�
		  case STATUS_ATTACK:			// ������Χ
		  case STATUS_MAGIC_ENEMY:		// ħ����Χ
		  case STATUS_MAGIC_PLAYER:		// ��ָ����ɫ
			status = STATUS_NONE;		// ָ���ء��ޡ���״̬
			ClearDistCursor();
			break;

		  case STATUS_NEXTTURN:			// ������һ��(Next turn)ʱ
			status = STATUS_NONE;		// ָ���ء��ޡ���״̬
			HideTurnEnd();
			break;
		}
	}
}

//
// �ƶ����ʱ�Ĵ���
//
void CBattleAction::MouseMove(UINT modKeys, CPoint point)
{
	if (turn == PLAYER_TURN) {
		switch (status) {
		  case STATUS_NONE:			// ��ʼ״̬
		  case STATUS_MOVE:			// ��ʾ�ƶ���Χ
		  case STATUS_ATTACK:		// ��ʾ������Χ
		  case STATUS_MAGIC_ENEMY:	// ��ʾħ����Χ
		  case STATUS_MAGIC_PLAYER:	// ��ʾħ����Χ
			SetCursor(PointToIndex(point));
			break;

		  case STATUS_COMMAND:		// ��ʾָ��˵�
			HighlightCommand(point);
			break;

		  case STATUS_NEXTTURN:		// ��ʾ��һ��ָ��˵�
			SelectTurnEnd(TurnEndSprite.PtIn(point)? true: false);
			break;
		}
	}
}

//
// ���¼���ʱ�Ĵ���
//
void CBattleAction::KeyDown(UINT key)
{
}

//
// IDLE����
//
BOOL CBattleAction::IdleAction()
{
	if (turn == ENEMY_TURN) {
		if (status != STATUS_ANIME) {
			if (!enemy_ptr->move_done) {		// ����δ�ƶ������ƶ�
				MoveEnemy();
			}
			else if (!enemy_ptr->attack_done) {	// ����δ�������򹥻�
				AttackEnemy();
			}
			else {
				if (status == STATUS_BATTLE_END) {	// ����
					return FALSE;
				}
				else if (++enemy_ptr == enemy_list.end()) {
					ChangeTurn(PLAYER_TURN);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

//
// ��ʱ���Ĵ���
//
bool CBattleAction::TimedOut(int timerId)
{
	switch (timerId) {
	  case TIMER_MOVE_ID:	// �ƶ�����
		{
			ASSERT(status == STATUS_ANIME);
			MoveAnime &ar = anime.front();
			CCharacter *character = ar.character;
			RemoveSprite(character);
			character->MoveAnime(ar.action, ar.count);
			AddSprite(character);
			if (ar.count++ == 3) {	// 4��ģʽ��ǰ������һ��
				anime.pop_front();
				if (anime.size() == 0) {	// �����ٵ�¼�����������
					CPoint pos = character->GetMapPoint();
					map_data[pos.y][pos.x].type |= (turn == PLAYER_TURN)? PLAYER: ENEMY;
					map_data[pos.y][pos.x].sprite = character;
					status = STATUS_NONE;
					return true;
				}
			}
		}
		break;

	  case TIMER_ATTACK_ID:	// �U��������
		ASSERT(status == STATUS_ANIME);

		if (AttackAnimeCount < CCharacter::ATTACK_STEP) {
			AttackFrom->AttackAnime(AttackAnimeCount++);
			RedrawSprite(AttackFrom);
		}
		else if (AttackAnimeCount < CCharacter::ATTACK_STEP + CCharacter::HIT_STEP) {
			AttackTo->HitAnime(AttackAnimeCount++ - CCharacter::ATTACK_STEP);
			RedrawSprite(AttackTo);
		}
		else {
			AttackFrom->NormalStatus();
			AttackTo->NormalStatus();
			RedrawSprite(AttackFrom);
			RedrawSprite(AttackTo);

			AttackPhase2();
			return true;
		}
		break;

	  case TIMER_NUMBER_ID:	// �ƻ�������
		ASSERT(status == STATUS_ANIME);
		if (AttackAnimeCount < 4) {
			for (int i=0; i<NumberView; i++) {
				Number[i].SetDrawPos(Number[i].GetDrawPos().x, Number[i].GetDrawPos().y - 1);
				CRect	rect;
				Number[i].GetRect(&rect);
				rect.bottom++;
				UpdateRect(rect);
			}
		}
		else if (AttackAnimeCount < 8) {
			for (int i=0; i<NumberView; i++) {
				Number[i].SetDrawPos(Number[i].GetDrawPos().x, Number[i].GetDrawPos().y + 1);
				CRect	rect;
				Number[i].GetRect(&rect);
				rect.top--;
				UpdateRect(rect);
			}
		}
		else {
			int n = NumberView;
			NumberView = 0;
			for (int i=0; i<n; i++) {
				RedrawSprite(Number + i);
			}

			// ��������ȼ�
			if (turn == PLAYER_TURN && AttackFrom->LevelUp()) {
				LevelUp();
			}
			else {
				AttackPhase3();
			}
			return true;
		}
		AttackAnimeCount++;
		break;

	  case TIMER_MAGIC_ID:	// ħ������
		ASSERT(status == STATUS_ANIME);
		switch (magic_param.id) {
		  case MAGIC_HEAL:			// �ָ�
			return MagicHeal();

		  case MAGIC_FIREBALL:		// Fire ball
			return MagicFireBall();
		}
		break;

	  case TIMER_LEVELUP_ID:	// �����ȼ��Ķ���
		ASSERT(status == STATUS_ANIME);
		if (AttackAnimeCount < 8) {
			Number[0].SetDrawPos(Number[0].GetDrawPos().x, Number[0].GetDrawPos().y - 1);
			CRect	rect;
			Number[0].GetRect(&rect);
			rect.bottom++;
			UpdateRect(rect);
		}
		else {
			NumberView = 0;
			RedrawSprite(Number);

			AttackPhase3();
			return true;
		}
		AttackAnimeCount++;
		break;
	}
	return false;
}

//
// ����(Wipe)����ʱ�Ĵ���
//
void CBattleAction::WipeDone()
{
	if (status == STATUS_WAIT_START)	// ��ʼWipe�ȴ�
		status = STATUS_NONE;
}

//
// Wave���Ž���ʱ�Ĵ���
//
void CBattleAction::WaveDone()
{
}

//
// ����ִ��
//
void CBattleAction::Abort()
{
}

//
// ��������¼�
//
void __cdecl CBattleAction::Error(TextReader &reader, const char *fmt, ...)
{
	va_list	args;

	char	str[256];
	int len = sprintf(str, "%s:%d ", reader.GetFileName(), reader.GetLineNo());
	va_start(args, fmt);
	_vsnprintf(str + len, sizeof(str) - len, fmt, args);
	va_end(args);

	Parent->MessageBox(str);
}

// player
bool CBattleAction::PlayerCmd(TextReader &reader, Lexer &lexer)
{
	int number;
	bool b = lexer.GetValue(&number);
	const char *name = lexer.GetString();
	const char *file = lexer.GetString();

	if (name == 0 || file == 0 || !b || lexer.GetString() != 0) {
		Error(reader, "�ķ�����");
		return false;
	}
	CChip chip(name, file);
	if (chip.image == 0) {
		Error(reader, "�޷���ȡCG����");
		return false;
	}
	chip_list.insert(pair<int, CChip>(PlayerNumber(number - 1), chip));

	return true;
}

// enemy
bool CBattleAction::EnemyCmd(TextReader &reader, Lexer &lexer)
{
	int number;
	bool b = lexer.GetValue(&number);
	const char *name = lexer.GetString();
	const char *file = lexer.GetString();

	if (name == 0 || file == 0 || !b || lexer.GetString() != 0) {
		Error(reader, "�ķ�����");
		return false;
	}
	CChip chip(name, file);
	if (chip.image == 0) {
		Error(reader, "�޷���ȡCG����");
		return false;
	}
	chip_list.insert(pair<int, CChip>(EnemyNumber(number - 1), chip));

	return true;
}

// object
bool CBattleAction::ObjectCmd(TextReader &reader, Lexer &lexer)
{
	int number;
	bool b = lexer.GetValue(&number);
	const char *name = lexer.GetString();
	const char *file = lexer.GetString();

	if (name == 0 || file == 0 || !b || lexer.GetString() != 0) {
		Error(reader, "�ķ�����");
		return false;
	}
	CChip chip(name, file);
	if (chip.image == 0) {
		Error(reader, "�޷���ȡCG����");
		return false;
	}
	chip_list.insert(pair<int, CChip>(ObjectNumber(number - 1), chip));

	return true;
}

// mapsize
bool CBattleAction::MapSizeCmd(TextReader &reader, Lexer &lexer)
{
	int width, height;
	if (!lexer.GetValue(&width)
	 || !lexer.GetValue(&height)
	 || lexer.GetString() != 0) {
		Error(reader, "�ķ�����");
		return false;
	}
	MapSize.cx = width;
	MapSize.cy = height;

	MapRect.SetRect(0, 0, width, height);

	// map_data�̗̈�m��
	_map_data = new MapData[width * height];
	map_data = new MapData *[height];

	for (int i=0; i<height; i++) {
		map_data[i] = _map_data + i * width;
	}

	return true;
}

// mapimage
bool CBattleAction::MapImageCmd(TextReader &reader, Lexer &lexer)
{
	const char *name = lexer.GetString();

	if (name == 0 || lexer.GetString() != 0) {
		Error(reader, "�ķ�����");
		return false;
	}
	if (!MapImage.LoadImage(name)) {
		Error(reader, "�޷���ȡCG����");
		return false;
	}
	return true;
}

//
// ��map�ϵ����ָĳ�״̬
//
int CBattleAction::FindMapStatus(const char *find)
{
	struct {
		char str[4];
		unsigned type;
	} table[] = {
		{ "��", NONE },
		{ "��", RESTRICTED_AREA },
		{ "һ", PLAYER | (0 << 8) },
		{ "��", PLAYER | (1 << 8) },
		{ "��", PLAYER | (2 << 8) },
		{ "��", PLAYER | (3 << 8) },
		{ "��", PLAYER | (4 << 8) },
		{ "��", PLAYER | (5 << 8) },
		{ "��", PLAYER | (6 << 8) },
		{ "��", PLAYER | (7 << 8) },
		{ "��", PLAYER | (8 << 8) },
		{ "��", ENEMY | (0 << 8) },
		{ "��", ENEMY | (1 << 8) },
		{ "��", ENEMY | (2 << 8) },
		{ "��", ENEMY | (3 << 8) },
		{ "��", ENEMY | (4 << 8) },
		{ "��", ENEMY | (5 << 8) },
		{ "��", ENEMY | (6 << 8) },
		{ "��", ENEMY | (7 << 8) },
		{ "��", ENEMY | (8 << 8) },
		{ "��", OBJECT | (0 << 8) },
		{ "��", OBJECT | (1 << 8) },
		{ "��", OBJECT | (2 << 8) },
		{ "��", OBJECT | (3 << 8) },
		{ "��", OBJECT | (4 << 8) },
		{ "��", OBJECT | (5 << 8) },
		{ "��", OBJECT | (6 << 8) },
		{ "��", OBJECT | (7 << 8) },
		{ "��", OBJECT | (8 << 8) },
	} ;
	for (int i=0; i<(sizeof(table) / sizeof(table[0])); i++) {
		if (strcmp(find, table[i].str) == 0)
			return table[i].type;
	}
	return -1;
}

// map
bool CBattleAction::MapCmd(TextReader &reader, Lexer &lexer)
{
	if (map_data == 0) {	// ��ʱ���Ѿ�Ҫ������ͼ�Ĵ�С
		Error(reader, "�����趨mapsize");
		return false;
	}

	for (int y=0; y<MapSize.cy; y++) {
		const char *str = reader.GetString();
		if (str == 0) {
			Error(reader, "�ķ�����");
			return false;
		}
		for (int x=0; x<MapSize.cx; x++) {
			char chip[3];
			chip[0] = str[x * 2 + 0];
			chip[1] = str[x * 2 + 1];
			chip[2] = 0;

			int idx = FindMapStatus(chip);
			if (idx < 0) {
				Error(reader, "�ķ�����");
				return false;
			}
			map_data[y][x].type = (unsigned char)idx;
			if (idx != RESTRICTED_AREA && idx != NONE) {
				map<int, CChip>::iterator p = chip_list.find(idx);
				if (p == chip_list.end()) {		// ��δ��¼
					Error(reader, "�����ɫ��δ��¼");
					return false;
				}
				else {
					if ((idx & PLAYER) != 0) {
						player_list.push_back(CCharacter(p->second.image, x, y, CCharacter::DEPTH_CHAR));
						CCharacter &s = player_list.back();

						// ����������������״̬
						if (!Parent->GetParam().GetStatus(p->second.name.c_str(), &s.status)) {	// �޵�״̬
							Error(reader, "�Ҳ����������%s��״̬��Ϣ");
							return false;
						}

						sprite.insert(&s);
						s.SetDirection(CCharacter::RIGHT);
						map_data[y][x].sprite = &s;

						StatusCharacter[idx >> 8] = &s;	// ��ʾ�����������Ϣ
					}
					else if ((idx & ENEMY) != 0) {
						enemy_list.push_back(CCharacter(p->second.image, x, y, CCharacter::DEPTH_CHAR));
						CCharacter &s = enemy_list.back();

						// ����������������״̬
						if (!Parent->GetParam().GetStatus(p->second.name.c_str(), &s.status)) {	// �޵�״̬
							Error(reader, "�Ҳ����з�����%s��״̬��Ϣ");
							return false;
						}

						sprite.insert(&s);
						s.SetDirection(CCharacter::LEFT);
						map_data[y][x].sprite = &s;
					}
					else if ((idx & OBJECT) != 0) {
						object_list.push_back(CMapSprite(p->second.image, x, y, CMapSprite::DEPTH_OBJECT));
						CMapSprite &s = object_list.back();
						sprite.insert(&s);
						map_data[y][x].sprite = &s;
					}
				}
			}
		}
	}
	return true;
}

//
// �����ͼ���嵵
//
bool CBattleAction::LoadMap(const char *name)
{
	typedef bool (CBattleAction::*cmd_t)(TextReader &, Lexer &);
	typedef pair<const char *, cmd_t>	CmdTab;
	typedef map<const char *, cmd_t, ic_less>	cmdmap;
	cmdmap	cmd_table;

	cmd_table.insert(CmdTab("player",	&CBattleAction::PlayerCmd));
	cmd_table.insert(CmdTab("enemy",	&CBattleAction::EnemyCmd));
	cmd_table.insert(CmdTab("object",	&CBattleAction::ObjectCmd));
	cmd_table.insert(CmdTab("mapsize",	&CBattleAction::MapSizeCmd));
	cmd_table.insert(CmdTab("mapimage",	&CBattleAction::MapImageCmd));
	cmd_table.insert(CmdTab("map",		&CBattleAction::MapCmd));

	char	path[_MAX_PATH];
	sprintf(path, MAPPATH "%s.txt", name);

	TextReader	reader(path);
	if (!reader) {
		Parent->MessageBox("�޷���ȡ��ͼ�ļ���");
		return false;
	}

	const char *str;
	while ((str = reader.GetString()) != 0) {
		Lexer	lexer(str);

		if (lexer.NumToken()) {
			const char *cmd = lexer.GetString(0);
			if (stricmp(cmd, "end") == 0)	// ����
				break;

			cmdmap::iterator p = cmd_table.find(cmd);
			if (p != cmd_table.end()) {
				if (!(this->*p->second)(reader, lexer))
					return false;
			}
			else {
				Error(reader, "�ķ�����");
				return false;
			}
		}
	}
	return true;
}

//
// ����ս����ͼ������
//
bool CBattleAction::Load(const char *name)
{
	// ���Բ���֮ǰ��״̬����Ϊerror
	// Ӧ�ò��������״̬������ΪASSERT
	ASSERT(map_data == 0);
	ASSERT(_map_data == 0);

	// �����ͼ����
	if (!LoadMap(name))
		return false;

	// ����CG����
	if (!Cursor.LoadImage("cursor")
	 || !Command.LoadImage("command")
	 || !TurnEnd.LoadImage("turnend")
	 || !StatusBase.LoadImage("status")
	 || !PopupImageBase.LoadImage("popup")
	 || !PopupParts.LoadImage("popup_parts")
	 || !FireBallImage.LoadImage("fireball")
	 || !HealImage.LoadImage("heal")
	 || !Explosion.LoadImage("explosion"))
		return false;
	// ����ͼ��
	CClientDC	dc(Parent);
	if (!PopupImage.Create(dc, PopupImageBase.Width(), PopupImageBase.Height()))
		return false;

	// �趨sprite
	CursorSprite.Set(&Cursor, CPoint(0, 0), CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_CURSOR);

	{
		for (int i=0; i<MAX_COMMAND; i++) {
			CommandSprite[i].Set(&Command, CPoint(0, 0), CPoint(0, i * 22), CSize(70, 22), 0);
			CommandSprite[i].Show(false);
		}
	}
	TurnEndSprite.Set(&TurnEnd, CPoint(0, 0), CPoint(0, 0), CSize(70, 22), 0);
	PopupSprite.Set(&PopupImage, CPoint(0, 0), CPoint(0, 0), PopupImage.Size(), 0);

	// �����״̬
	int total_width = StatusBase.Width() * MAX_CHARACTER + 3 * (MAX_CHARACTER - 1);
	int x = (WindowWidth - total_width) / 2;
	int y = WindowHeight - StatusBase.Height() - 10;
	{
		for (int i=0; i<MAX_CHARACTER; i++) {
			if (!Status[i].Create(dc, StatusBase.Width(), StatusBase.Height()))
				return false;
			StatusSprite[i].Set(Status + i, CPoint(x, y), CPoint(0, 0), StatusBase.Size(), 0);
			x += StatusBase.Width() + 3;
			SetPlayerStatus(i);
		}
	}

	CursorPos.x = -1;
	CursorPos.y = -1;
	SelPos.x = -1;
	SelPos.y = -1;
	SelChar = 0;
	status = STATUS_WAIT_START;
	turn = PLAYER_TURN;
	NumberView = 0;

	StatusChar = 0;

	// ����¼�
	Parent->ClearMessage();
	// ����ص�
	Parent->ClearOverlap();

	// ��������ͼ��
	DrawMap(BackLayer);

	// Wipe in
	Parent->WipeIn();

	return true;
}

//
// ������ͼ����
//
void CBattleAction::DrawMap(CImage *image, const CRect &rect)
{
	// ����ͼ�б���������ʾ֮
	if (MapImage.IsOK()) {
		image->Copy(&MapImage, rect);
	}
	// ��ʾsprite
	for (mapset::iterator p = sprite.begin(); p != sprite.end(); p++) {
		(*p)->Draw(image, rect);
	}
	// ��ʾ��ɫ��״̬
	for (int i=0; i<MAX_CHARACTER; i++) {
		StatusSprite[i].Draw(image, rect);
	}
	// ��ʾ���λ�õ�״̬
	if (StatusChar != 0) {
		PopupSprite.Draw(image, rect);
	}
	// �ձ����ʾ��ͼ���(sprite)
	for (int j=0; j<NumberView; j++)
		Number[j].Draw(image, rect);

	// ��ʾ�˵�
	switch (status) {
	  case STATUS_COMMAND:
		{
			for (int i=0; i<MAX_COMMAND; i++) {
				if (CommandSprite[i].IsShow())
					CommandSprite[i].Draw(image, rect);
			}
		}
		break;

	  case STATUS_NEXTTURN:
		TurnEndSprite.Draw(image, rect);
		break;
	}
}

//
// ��ʾ��ͼ��������ͼ��
//
void CBattleAction::DrawMap(CImage *image)
{
	DrawMap(image, CRect(0, 0, WindowWidth, WindowHeight));
}

//
// ��������״̬��ͼ��
//
void CBattleAction::SetPlayerStatus(int player)
{
	Status[player].Copy(&StatusBase);
	if (StatusCharacter[player] != 0) {
		CStatus &status = StatusCharacter[player]->status;
		CImageDC dc(Status + player);
		HFONT	oldFont = dc.SelectObject(hFont);
		dc.SetTextColor(WhitePixel);
		dc.SetBkMode(TRANSPARENT);

		char str[256];
		dc.ExtTextOut(10, 4, 0, 0, status.name, strlen(status.name), NULL);
		dc.ExtTextOut(10, 22, 0, 0, str, sprintf(str, "�ȼ�������%7d", status.level), NULL);
		dc.ExtTextOut(10, 40, 0, 0, str, sprintf(str, "����ֵ����%7d", status.experience), NULL);
		dc.ExtTextOut(10, 58, 0, 0, str, sprintf(str, "HPֵ����%3d/%3d", status.hit_point, status.max_hit_point), NULL);
		dc.ExtTextOut(10, 76, 0, 0, str, sprintf(str, "MPֵ����%3d/%3d", status.magic_point, status.max_magic_point), NULL);

		dc.SelectObject(oldFont);
	}
}

//
// ��ʾ��ɫ��״̬
//
void CBattleAction::ChangePlayerStatus(CCharacter *ch)
{
	for (int i=0; i<MAX_CHARACTER; i++) {
		if (StatusCharacter[i] == ch) {
			SetPlayerStatus(i);
			RedrawSprite(StatusSprite + i);
		}
	}
}

//
// ��ʾͻ��ʽ״̬˵��
//
void CBattleAction::ChangeStatus(CPoint point)
{
	if (MapRect.PtInRect(point) && map_data[point.y][point.x].type & (PLAYER | ENEMY)) {
		if (StatusChar != (CCharacter *)map_data[point.y][point.x].sprite) {
			StatusChar = 0;

			RedrawSprite(&PopupSprite);		// ɾ��֮ǰ����ʾ
			StatusChar = (CCharacter *)map_data[point.y][point.x].sprite;
			CStatus &status = StatusChar->status;

			// ���״̬��ʾ�����
			PopupImage.Copy(&PopupImageBase);	// ����ԭʼͼ��
			PopupImage.DrawText(hFont, 1, 1, status.name, RGB(0, 0, 0));
			PopupImage.DrawText(hFont, 0, 0, status.name);

			int hp_len = status.hit_point * 76 / status.max_hit_point;
			int mp_len = status.magic_point * 76 / status.max_magic_point;
			PopupImage.Copy(&PopupParts, CRect(21, 17, 21 + hp_len, 20), CPoint(0, 20));
			PopupImage.Copy(&PopupParts, CRect(21, 32, 21 + mp_len, 35), CPoint(0, 23));

			char hp_str[4], mp_str[4], mhp_str[4], mmp_str[4];
			sprintf(hp_str, "%03d", status.hit_point);
			sprintf(mp_str, "%03d", status.magic_point);
			sprintf(mhp_str, "%03d", status.max_hit_point);
			sprintf(mmp_str, "%03d", status.max_magic_point);

			for (int i=0; i<3; i++) {
				PopupImage.Copy(&PopupParts, CRect(42 + i * 8, 21, 50 + i * 8, 31), CPoint((hp_str[i] - '0') * 8, 0));
				PopupImage.Copy(&PopupParts, CRect(74 + i * 8, 21, 82 + i * 8, 31), CPoint((mhp_str[i] - '0') * 8, 0));
				PopupImage.Copy(&PopupParts, CRect(42 + i * 8, 36, 50 + i * 8, 46), CPoint((mp_str[i] - '0') * 8, 10));
				PopupImage.Copy(&PopupParts, CRect(74 + i * 8, 36, 82 + i * 8, 46), CPoint((mmp_str[i] - '0') * 8, 10));
			}
			point = IndexToPoint(point);
			point.x += (MAPGRID_WIDTH - PopupImage.Width()) / 2;
			point.y -= 64;

			// �����ڷ�Χ�ڣ�����ڴ�����
			if (point.x < 0) {
				point.x = 0;
			}
			else if (point.x >= WindowWidth - PopupImage.Width()) {
				point.x = WindowWidth - PopupImage.Width();
			}
			if (point.y < 0) {
				point.y = 0;
			}

			PopupSprite.SetDrawPos(point);
			RedrawSprite(&PopupSprite);		// ��ʾ
		}
	}
	else if (StatusChar != 0) {				// ����ʾ״̬��������������
		StatusChar = 0;
		RedrawSprite(&PopupSprite);			// ɾ��֮ǰ����ʾ
	}
}

//
// �ƶ���ͼ���
//
void CBattleAction::SetCursor(CPoint point)
{
	if (CursorPos != point) {		// ����ͼλ���и���
		ClearCursor();				// ��ɾ��Ŀǰ��ʾ�Ĺ��
		CursorPos = point;			// �����趨�µ�λ��
		DrawCursor();				// ��ʾ�µĹ��
		ChangeStatus(point);		// �����ɫ��״̬
	}
}

//
// ɾ����ͼ���
//
void CBattleAction::ClearCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		RemoveSprite(&CursorSprite);
	}
	CursorPos.x = CursorPos.y = -1;
}

//
// ��ʾ��ͼ���
//
void CBattleAction::DrawCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		CursorSprite.SetMapPoint(CursorPos);
		AddSprite(&CursorSprite);
	}
}

// ���»���sprite

void CBattleAction::RedrawSprite(CSprite *s)
{
	CRect	rect;
	s->GetRect(&rect);
	UpdateRect(rect);
}

// ����sprite

void CBattleAction::AddSprite(CSprite *s)
{
	sprite.insert(s);
	RedrawSprite(s);
}

// ɾ��sprite

void CBattleAction::RemoveSprite(CSprite *s)
{
	if (sprite.erase(s)) {
		RedrawSprite(s);
	}
}

// Ѱ���ƶ���Χ

void CBattleAction::FindDist(int x, int y, int dist, bool first)
{
	if (!first) {
		AddDistCursor(x, y, dist, 0);
		map_data[y][x].type |= MOVEDIST;
		map_data[y][x].move_dist = dist;
	}
	if (dist == 0)
		return;
	if (y > MapRect.top && map_data[y - 1][x].ChkMove(dist))
		FindDist(x, y - 1, dist - 1, false);
	if (y < MapRect.bottom - 1 && map_data[y + 1][x].ChkMove(dist))
		FindDist(x, y + 1, dist - 1, false);
	if (x > MapRect.left && map_data[y][x - 1].ChkMove(dist))
		FindDist(x - 1, y, dist - 1, false);
	if (x < MapRect.right - 1 && map_data[y][x + 1].ChkMove(dist))
		FindDist(x + 1, y, dist - 1, false);
}

// ��Ѱ������Χ

void CBattleAction::FindAttack(int x, int y, int dist, bool first)
{
	if (!first) {
		AddDistCursor(x, y, dist, 1);
		map_data[y][x].type |= ATTACKDIST;
		map_data[y][x].attack_dist = dist;
	}
	if (dist == 0)
		return;
	if (y > MapRect.top && map_data[y - 1][x].ChkAttack(dist))
		FindAttack(x, y - 1, dist - 1, false);
	if (y < MapRect.bottom - 1 && map_data[y + 1][x].ChkAttack(dist))
		FindAttack(x, y + 1, dist - 1, false);
	if (x > MapRect.left && map_data[y][x - 1].ChkAttack(dist))
		FindAttack(x - 1, y, dist - 1, false);
	if (x < MapRect.right - 1 && map_data[y][x + 1].ChkAttack(dist))
		FindAttack(x + 1, y, dist - 1, false);
}

// ��ʾ��Χ

void CBattleAction::AddDistCursor(int x, int y, int dist, int type)
{
	if ((map_data[y][x].type & (MOVEDIST | ATTACKDIST)) != 0)	// �ѵ�¼�Ĳ���
		return;

	cursor_list.push_back(CMapSprite(&Cursor, x, y, CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_AREA));
	CMapSprite &s = cursor_list.back();
	s.SetSrcPos(0, 32 + type * 32);
	AddSprite(&s);
}

// ɾ����ʾ��Χ

void CBattleAction::ClearDistCursor()
{
	while (cursor_list.size()) {
		CMapSprite &cr = cursor_list.front();
		CPoint pos = cr.GetMapPoint();
		RemoveSprite(&cr);
		map_data[pos.y][pos.x].type &= ~(MOVEDIST | ATTACKDIST);
		map_data[pos.y][pos.x].move_dist = 0;
		map_data[pos.y][pos.x].attack_dist = 0;
		cursor_list.pop_front();
	}
}

// �����ƶ�

void CBattleAction::MovePlayer(CCharacter *character, CPoint point)
{
	CPoint pos = character->GetMapPoint();
	map_data[pos.y][pos.x].move_dist = character->status.move_dist;
	MoveCharacter(point.x, point.y, map_data[point.y][point.x].move_dist, character);

	map_data[pos.y][pos.x].type = NONE;
	map_data[pos.y][pos.x].sprite = 0;
	map_data[pos.y][pos.x].move_dist = 0;

	SetCursor(CPoint(-1, -1));
	ClearDistCursor();
	StartAnime();
}

// ��¼������ƶ�����

void CBattleAction::MoveCharacter(int x, int y, int dist, CCharacter *character)
{
	if (map_data[y][x].sprite != character) {
		dist++;
		if (y > MapRect.top && map_data[y - 1][x].move_dist == dist) {
			MoveCharacter(x, y - 1, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::DOWN));
		}
		else if (y < MapRect.bottom - 1 && map_data[y + 1][x].move_dist == dist) {
			MoveCharacter(x, y + 1, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::UP));
		}
		else if (x > MapRect.left && map_data[y][x - 1].move_dist == dist) {
			MoveCharacter(x - 1, y, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::RIGHT));
		}
		else if (x < MapRect.right - 1 && map_data[y][x + 1].move_dist == dist) {
			MoveCharacter(x + 1, y, dist, character);
			anime.push_back(MoveAnime(character, CCharacter::LEFT));
		}
	}
}

// ִ���ƶ�����

void CBattleAction::StartAnime()
{
	if (anime.size()) {
		Parent->SetTimer(TIMER_MOVE_ID, TIMER_MOVE_TIME);
		status = STATUS_ANIME;
	}
	else {
		status = STATUS_NONE;
	}
}

// ����ֵ��з�

void CBattleAction::ChangeTurn(int _turn)
{
	status = STATUS_NONE;
	turn = _turn;
	switch (turn) {
	  case PLAYER_TURN:
		for_each(player_list.begin(), player_list.end(), mem_fun_ref(&CCharacter::ClearStatus));
		break;

	  case ENEMY_TURN:
		for_each(enemy_list.begin(), enemy_list.end(), mem_fun_ref(&CCharacter::ClearStatus));
		enemy_ptr = enemy_list.begin();
		break;
	}
}

// ��ʾ����˵�

CPoint CBattleAction::CommandIndex(int idx)
{
	// �ƶ�
	switch (idx) {
	  case COMMAND_MOVE:		// �ƶ�
		return CPoint(SelChar->move_done? 140: 0, 0);

	  case COMMAND_ATTACK:		// ֱ�ӹ���
		return CPoint(SelChar->attack_done? 140: 0, 22);

	  default:					// ħ��
		idx -= COMMAND_MAGIC_FIRST;
		if (SelChar->IsMagicOk(idx)) {
			return CPoint(SelChar->attack_done? 140: 0,
				SelChar->GetMagicParam(idx).id * 22 + 44);
		}
		return CPoint(-1, -1);
	}
}

void CBattleAction::ShowCommand(CPoint point)
{
	if (point.x > 320)
		point.x -= 70 + 24;
	else
		point.x += 24;

	status = STATUS_COMMAND;
	select = COMMAND_NO_SELECT;

	for (int i=0; i<MAX_COMMAND; i++) {
		CPoint src = CommandIndex(i);
		if (src.x >= 0) {
			CommandSprite[i].SetDrawPos(point.x, point.y);
			CommandSprite[i].SetSrcPos(src.x, src.y);
			CommandSprite[i].Show();
			RedrawSprite(CommandSprite + i);
			point.y += 23;
		}
	}
}

// ѡ������˵�--��������

void CBattleAction::HighlightCommand(int newsel, bool selection)
{
	if (newsel != COMMAND_NO_SELECT) {
		CSprite *sp = CommandSprite + newsel;
		sp->SetSrcPos(selection? 70: 0, sp->GetSrcPos().y);
		RedrawSprite(sp);
	}
}

void CBattleAction::HighlightCommand(CPoint point)
{
	int newsel = FindCommand(point);
	if (select != newsel) {
		HighlightCommand(select, false);
		select = newsel;
		HighlightCommand(select, true);
	}
}

// ɾ������˵�

void CBattleAction::HideCommand()
{
	for (int i=0; i<MAX_COMMAND; i++) {
		if (CommandSprite[i].IsShow()) {
			CommandSprite[i].Show(false);
			RedrawSprite(CommandSprite + i);
		}
	}
}

// ѡ������˵�

void CBattleAction::SelectCommand(CPoint point)
{
	int sel = FindCommand(point);
	switch (sel) {
	  case COMMAND_NO_SELECT:
		break;

	  case COMMAND_MOVE:
		HideCommand();
		status = STATUS_MOVE;
		FindDist(SelPos.x, SelPos.y, SelChar->status.move_dist);
		break;

	  case COMMAND_ATTACK:
		HideCommand();
		status = STATUS_ATTACK;
		FindAttack(SelPos.x, SelPos.y, SelChar->status.attack_dist);
		break;

	  default:
		HideCommand();
		magic_param = SelChar->GetMagicParam(sel - COMMAND_MAGIC_FIRST);
		if (magic_param.type == MAGIC_TYPE_SELF) {
			status = STATUS_MAGIC_PLAYER;
			FindAttack(SelPos.x, SelPos.y, magic_param.dist, false);
		}
		else {
			status = STATUS_MAGIC_ENEMY;
			FindAttack(SelPos.x, SelPos.y, magic_param.dist);
		}
		break;
	}
}

// ������˵���λ�ÿ�ʼ��Ѱ

int CBattleAction::FindCommand(CPoint point)
{
	if (!SelChar->move_done && CommandSprite[COMMAND_MOVE].PtIn(point))
		return COMMAND_MOVE;

	if (!SelChar->attack_done) {
		for (int i=COMMAND_ATTACK; i<MAX_COMMAND; i++) {
			if (CommandSprite[i].PtIn(point))
				return i;
		}
	}
	return COMMAND_NO_SELECT;
}

// ��ʾ TURN END �˵�

void CBattleAction::ShowTurnEnd(CPoint point)
{
	if (point.x > 320)
		point.x -= 70 + 24;
	else
		point.x += 24;

	TurnEndSprite.SetDrawPos(point.x, point.y);
	TurnEndSprite.SetSrcPos(0, 0);
	status = STATUS_NEXTTURN;
	sel_turnend = false;
	RedrawSprite(&TurnEndSprite);
}

// ���� TURN END �˵�

void CBattleAction::HideTurnEnd()
{
	RedrawSprite(&TurnEndSprite);
}

// ѡȡ TURN END �˵�

void CBattleAction::SelectTurnEnd(bool sel)
{
	if (sel_turnend != sel) {
		sel_turnend = sel;
		TurnEndSprite.SetSrcPos(sel? 70: 0, 0);
		RedrawSprite(&TurnEndSprite);
	}
}

// ����

void CBattleAction::Attack(CCharacter *from, CCharacter *to, bool player)
{
	// ɾ�����
	SetCursor(CPoint(-1, -1));

	// ɾ����Χ��ʾ
	ClearDistCursor();
	CPoint pos = from->GetMapPoint();
	map_data[pos.y][pos.x].type &= ~ATTACKDIST;
	map_data[pos.y][pos.x].attack_dist = 0;

	CSize diff = from->GetMapPoint() - to->GetMapPoint();

	// �������﷽��
	if (abs(diff.cx) > abs(diff.cy)) {
		if (diff.cx < 0) {
			from->SetDirection(CCharacter::RIGHT);
			to->SetDirection(CCharacter::LEFT);
		}
		else {
			from->SetDirection(CCharacter::LEFT);
			to->SetDirection(CCharacter::RIGHT);
		}
	}
	else {
		if (diff.cy < 0) {
			from->SetDirection(CCharacter::DOWN);
			to->SetDirection(CCharacter::UP);
		}
		else {
			from->SetDirection(CCharacter::UP);
			to->SetDirection(CCharacter::DOWN);
		}
	}
	RedrawSprite(from);
	RedrawSprite(to);

	// �����Ķ�����ʼ
	AttackFrom = from;
	AttackTo = to;
	AttackPlayerToEnemy = player;
	AttackAnimeCount = 0;

	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_ATTACK_ID, TIMER_ATTACK_TIME);

	// ������Ч
	Parent->StartWave(ATTACK_WAVE);
}

// �����׶�2

void CBattleAction::AttackPhase2()
{
	// �жϹ���
	// debug�ã�׷�ٱ���
	TRACE("��������:[%s]\n", AttackFrom->status.name);
	TRACE(" ������ : %4d\n", AttackFrom->status.attack_power);
	TRACE(" ������ : %4d\n", AttackFrom->status.defence_power);
	TRACE(" ʣ��HP : %4d\n", AttackFrom->status.hit_point);
	TRACE("�ط�����:[%s]\n", AttackTo->status.name);
	TRACE(" ������ : %4d\n", AttackTo->status.attack_power);
	TRACE(" ������ : %4d\n", AttackTo->status.defence_power);
	TRACE(" ʣ��HP : %4d\n", AttackTo->status.hit_point);

	// �������ֻ����������ѣ���û��ʹ��������
	// �ƻ��� = ���������� * ���������� / �ط�������
	int ap = AttackFrom->status.attack_power;
	int dp = AttackTo->status.defence_power == 0? 1: AttackTo->status.defence_power;
	int attack = ap * ap / dp;
	if (attack <= 0)	// ���1������ƻ���
		attack = 1;

	// ��ʾ�ƻ���
	char damage[8];
	NumberView = sprintf(damage, "%d", attack);
	CPoint view = AttackTo->GetDrawPos();
	view.x += (AttackTo->GetSize().cx - NumberView * 8) / 2;
	view.y -= 12;
	for (int i=0; i<NumberView; i++) {
		damage[i] -= '0';
		Number[i].Set(&PopupParts, view, CPoint(damage[i] * 8, 0), CSize(8, 10), 0);
		view.x += 8;
		RedrawSprite(Number + i);
	}

	if (attack > AttackTo->status.hit_point)
		attack = AttackTo->status.hit_point;

	TRACE(" �ƻ��� : %4d\n", attack);

	AttackTo->status.hit_point -= attack;

	TRACE(" ʣ��HP : %4d\n", AttackTo->status.hit_point);

	// ���㾭��ֵ
	// ÿ�ι������л��� 10 point��������ܵ�����+20 point
	// ����еȼ����ʱ
	// �����ȼ� �� �ط��ȼ�  ��������õľ���ֵ
	// �����ȼ� �� �ط��ȼ�  �������õľ���ֵ
	int experience = 10;
	if (AttackTo->status.hit_point == 0)
		experience += 20;
	int level_diff = AttackFrom->status.level - AttackTo->status.level;
	if (level_diff > 0) {	// ���������Ķ���
		experience >>= level_diff;
	}
	else if (level_diff) {
		experience <<= -level_diff;
	}

	TRACE(" ����ֵ = %d + %d = %d\n",
		AttackFrom->status.experience, experience,
		AttackFrom->status.experience + experience);

	AttackFrom->status.experience += experience;

	// �ƻ�������ʾ��ʼ
	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_NUMBER_ID, TIMER_NUMBER_TIME);
}

// �����׶�3

void CBattleAction::AttackPhase3()
{
	// �������Ĵ���
	status = STATUS_NONE;

	// ��Ϸ��״̬��ʾ�ı任
	ChangePlayerStatus(AttackFrom);
	ChangePlayerStatus(AttackTo);

	// �������ж�
	if (AttackTo->status.hit_point == 0) {	// HPֵû���˾͵�������
		CPoint po = AttackTo->GetMapPoint();
		map_data[po.y][po.x].sprite = 0;
		map_data[po.y][po.x].type &= ~(PLAYER | ENEMY | OBJECT);
		RemoveSprite(AttackTo);		// ��sprite�嵥��ɾ��

		//
		// �ж�ս������
		//
		// ��������״����ȫ����û��
		// ������Ϸ��ɫ�������ж�
		//
		if (turn == PLAYER_TURN) {
			list<CCharacter>::iterator p = find(enemy_list.begin(), enemy_list.end(), *AttackTo);
			if (&*p != AttackTo) {
				TRACE("error\n");
			}
			ASSERT(p != enemy_list.end());
			enemy_list.erase(p);
			if (enemy_list.empty()) {	// ȫ����û
				TRACE("��Ӯ\n");
				BattleEnd(true);
			}
		}
		else {
			// ��������ǰҪ��״̬�����ȥ
			Parent->GetParam().SetStatus(AttackTo->status.name, &AttackTo->status);

			list<CCharacter>::iterator p = find(player_list.begin(), player_list.end(), *AttackTo);
			if (&*p != AttackTo) {
				TRACE("error\n");
			}
			ASSERT(p != player_list.end());
			player_list.erase(p);
			if (player_list.empty()) {	// ȫ����û
				TRACE("����\n");
				BattleEnd(false);
			}
		}
	}
}

// �����ȼ�

void CBattleAction::LevelUp()
{
	NumberView = 1;
	CPoint view = AttackFrom->GetDrawPos();
	view.x += (AttackFrom->GetSize().cx - 32) / 2;
	view.y -= 12;
	Number[0].Set(&PopupParts, view, CPoint(0, 32), CSize(32, 9), 0);
	RedrawSprite(Number);

	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_LEVELUP_ID, TIMER_LEVELUP_TIME);

	// ������Ч
	Parent->StartWave(LEVELUP_WAVE);
}

// ս������

void CBattleAction::BattleEnd(bool iswin)
{
	// �������ж���
	status = STATUS_BATTLE_END;

	// ��ʤ��������浽����
	Parent->SetValue("battle", iswin? 1: 0);

	// �Ȱ������״̬�����ȥ
	for (list<CCharacter>::iterator p = player_list.begin(); p != player_list.end(); ++p) {
		Parent->GetParam().SetStatus(p->status.name, &p->status);
	}

	// ����ʾ�е����ݴ��浽�����������Ա������¼�
	BackLayer->Copy(ViewImage);
	Parent->Invalidate();

	// CBattleAction����
	Parent->ScriptDone();
}

// ħ��

void CBattleAction::Magic(CCharacter *from, CCharacter *to, bool player)
{
	// ɾ�����
	SetCursor(CPoint(-1, -1));

	// �����ʾ��Χ
	ClearDistCursor();
	CPoint pos = from->GetMapPoint();
	map_data[pos.y][pos.x].type &= ~ATTACKDIST;
	map_data[pos.y][pos.x].attack_dist = 0;

	CSize diff = from->GetMapPoint() - to->GetMapPoint();

	// ���������ɫ�ķ���
	if (abs(diff.cx) > abs(diff.cy)) {
		if (diff.cx < 0) {
			from->SetDirection(CCharacter::RIGHT);
			to->SetDirection(CCharacter::LEFT);
		}
		else {
			from->SetDirection(CCharacter::LEFT);
			to->SetDirection(CCharacter::RIGHT);
		}
	}
	else {
		if (diff.cy < 0) {
			from->SetDirection(CCharacter::DOWN);
			to->SetDirection(CCharacter::UP);
		}
		else {
			from->SetDirection(CCharacter::UP);
			to->SetDirection(CCharacter::DOWN);
		}
	}
	RedrawSprite(from);
	RedrawSprite(to);

	// ����������ʼ
	AttackFrom = from;
	AttackTo = to;
	AttackPlayerToEnemy = player;
	AttackAnimeCount = 0;
	AttackAnimeState = 0;

	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_MAGIC_ID, TIMER_MAGIC_TIME);

	// ����Ч����
	switch (magic_param.id) {
	  case MAGIC_HEAL:			// �ָ�
		Parent->StartWave(HEAL_WAVE);
		break;

	  case MAGIC_FIREBALL:		// Fire ball
		Parent->StartWave(FIREBALL_WAVE);
		break;
	}
}

// ħ������(Heal)

bool CBattleAction::MagicHeal()
{
	switch (AttackAnimeState) {
	  case 0:		// ��1�׶�  ħ������
		if (AttackFrom->MagicAnime(AttackAnimeCount++)) {
			AttackAnimeCount = 0;
			AttackAnimeState++;

			MagicSprite[0].Set(&HealImage, CPoint(0, 0), CPoint(0, 0),
				CSize(HealImage.Width() / 7, HealImage.Height() / 2), 10);	// ǰ
			MagicSprite[1].Set(&HealImage, CPoint(0, 0), CPoint(0, HealImage.Height() / 2),
				CSize(HealImage.Width() / 7, HealImage.Height() / 2), -10);	// ���
			MagicSprite[0].SetMapPoint(AttackTo->GetMapPoint());
			MagicSprite[1].SetMapPoint(AttackTo->GetMapPoint());
			AddSprite(MagicSprite);
			AddSprite(MagicSprite + 1);
		}
		RedrawSprite(AttackFrom);
		break;

	  case 1:		// ��2�׶�  Ч������
		if (AttackAnimeCount < 7) {
			MagicSprite[0].SetSrcPos(
				MagicSprite[0].GetSize().cx * AttackAnimeCount,
				MagicSprite[0].GetSrcPos().y);
			MagicSprite[1].SetSrcPos(
				MagicSprite[1].GetSize().cx * AttackAnimeCount,
				MagicSprite[1].GetSrcPos().y);
			RedrawSprite(MagicSprite);
			RedrawSprite(MagicSprite + 1);
			AttackAnimeCount++;
		}
		else {
			AttackFrom->NormalStatus();
			RedrawSprite(AttackFrom);
			RemoveSprite(MagicSprite);
			RemoveSprite(MagicSprite + 1);
			HealPhase2();
			return true;
		}
	}
	return false;
}

// ��2�׶ε�ħ���ָ�/��Ѫ

void CBattleAction::HealPhase2()
{
	// �ж�ħ��
	// debug�ã�׷�ٱ���
	TRACE("��������:[%s]\n", AttackFrom->status.name);
	TRACE(" ������ : %4d\n", AttackFrom->status.attack_power);
	TRACE(" ������ : %4d\n", AttackFrom->status.defence_power);
	TRACE(" ʣ��HP : %4d\n", AttackFrom->status.hit_point);
	TRACE("�ط�����:[%s]\n", AttackTo->status.name);
	TRACE(" ������ : %4d\n", AttackTo->status.attack_power);
	TRACE(" ������ : %4d\n", AttackTo->status.defence_power);
	TRACE(" ʣ��HP : %4d\n", AttackTo->status.hit_point);

	TRACE("ʹ��ħ�� id    : %d\n", magic_param.id);
	TRACE("         type  : %d\n", magic_param.type);
	TRACE("         dist  : %d\n", magic_param.dist);
	TRACE("         power : %d\n", magic_param.power);
	TRACE("         use_mp: %d\n", magic_param.use_mp);

	// MP������
	AttackFrom->status.magic_point -= magic_param.use_mp;

	// ���˵���ʾ
	char heal[8];
	NumberView = sprintf(heal, "%d", magic_param.power);
	CPoint view = AttackTo->GetDrawPos();
	view.x += (AttackTo->GetSize().cx - NumberView * 8) / 2;
	view.y -= 12;
	for (int i=0; i<NumberView; i++) {
		heal[i] -= '0';
		Number[i].Set(&PopupParts, view, CPoint(heal[i] * 8, 10), CSize(8, 10), 0);
		view.x += 8;
		RedrawSprite(Number + i);
	}

	AttackTo->status.hit_point += magic_param.power;
	if (AttackTo->status.hit_point > AttackTo->status.max_hit_point)
		AttackTo->status.hit_point = AttackTo->status.max_hit_point;

	TRACE(" ʣ��HP : %4d\n", AttackTo->status.hit_point);

	// ����ֵ�ļ���
	AttackFrom->status.experience += 10;

	// ��ʼ��ʾ�ָ���
	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_NUMBER_ID, TIMER_NUMBER_TIME);
}

// ħ������(FireBall)

bool CBattleAction::MagicFireBall()
{
	switch (AttackAnimeState) {
	  case 0:		//��1�׶�  ħ������
		if (AttackFrom->MagicAnime(AttackAnimeCount++)) {
			AttackAnimeCount = 0;
			AttackAnimeState++;

			MagicSprite[0].SetFireBall(&FireBallImage, AttackFrom);
			AddSprite(MagicSprite);
		}
		RedrawSprite(AttackFrom);
		break;

	  case 1:		// ��2�׶�  ������ƶ�
		{
			RemoveSprite(MagicSprite);
			if (MagicSprite[0].MoveFireBall(AttackFrom, AttackTo, AttackAnimeCount)) {
				AddSprite(MagicSprite);
				AttackAnimeCount++;
			}
			else {
				AttackAnimeCount = 0;
				AttackAnimeState++;
				MagicSprite[0].Set(&Explosion, CPoint(0, 0), CPoint(0, 0),
					CSize(Explosion.Width() / 11, Explosion.Height()), 10);
				MagicSprite[0].SetMapPoint(AttackTo->GetMapPoint());
				AddSprite(MagicSprite);
			}
		}
		break;

	  case 2:		// ��3�׶�  ��ը
		if (AttackAnimeCount < 11) {
			MagicSprite[0].SetSrcPos(
				MagicSprite[0].GetSize().cx * AttackAnimeCount,
				MagicSprite[0].GetSrcPos().y);
			RedrawSprite(MagicSprite);
			AttackAnimeCount++;
		}
		else {
			AttackFrom->NormalStatus();
			RemoveSprite(MagicSprite);
			RedrawSprite(AttackFrom);
			FireBallPhase2();
			return true;
		}
	}
	return false;
}

// ��2�׶εĹ���ħ��

void CBattleAction::FireBallPhase2()
{
	// �ж�ħ��
	// debug�ã�׷�ٱ���
	TRACE("��������:[%s]\n", AttackFrom->status.name);
	TRACE(" ������ : %4d\n", AttackFrom->status.attack_power);
	TRACE(" ������ : %4d\n", AttackFrom->status.defence_power);
	TRACE(" ʣ��HP : %4d\n", AttackFrom->status.hit_point);
	TRACE("�ط�����:[%s]\n", AttackTo->status.name);
	TRACE(" ������ : %4d\n", AttackTo->status.attack_power);
	TRACE(" ������ : %4d\n", AttackTo->status.defence_power);
	TRACE(" ʣ��HP : %4d\n", AttackTo->status.hit_point);

	TRACE("ʹ��ħ�� id    : %d\n", magic_param.id);
	TRACE("         type  : %d\n", magic_param.type);
	TRACE("         dist  : %d\n", magic_param.dist);
	TRACE("         power : %d\n", magic_param.power);
	TRACE("         use_mp: %d\n", magic_param.use_mp);

	// MP������
	AttackFrom->status.magic_point -= magic_param.use_mp;
	int ap = magic_param.power;
	int dp = AttackTo->status.resistance == 0? 1: AttackTo->status.resistance;
	int attack = ap * ap / dp;
	if (attack <= 0)	// �ƻ�������1��
		attack = 1;

	// ���˵���ʾ
	char damage[8];
	NumberView = sprintf(damage, "%d", attack);
	CPoint view = AttackTo->GetDrawPos();
	view.x += (AttackTo->GetSize().cx - NumberView * 8) / 2;
	view.y -= 12;
	for (int i=0; i<NumberView; i++) {
		damage[i] -= '0';
		Number[i].Set(&PopupParts, view, CPoint(damage[i] * 8, 0), CSize(8, 10), 0);
		view.x += 8;
		RedrawSprite(Number + i);
	}

	if (attack > AttackTo->status.hit_point)
		attack = AttackTo->status.hit_point;

	TRACE(" ���� : %4d\n", attack);

	AttackTo->status.hit_point -= attack;

	TRACE(" ʣ��HP : %4d\n", AttackTo->status.hit_point);

	// ���㾭��ֵ
	// ÿ�ι������л��� 10 point��������ܵ�����+20 point
	// ����еȼ����ʱ
	// �����ȼ� �� �ط��ȼ�  ��������õľ���ֵ
	// �����ȼ� �� �ط��ȼ�  �������õľ���ֵ
	int experience = 10;
	if (AttackTo->status.hit_point == 0)
		experience += 20;
	int level_diff = AttackFrom->status.level - AttackTo->status.level;
	if (level_diff > 0) {	// ���������Ķ���
		experience >>= level_diff;
	}
	else if (level_diff) {
		experience <<= -level_diff;
	}

	TRACE(" ����ֵ = %d + %d = %d\n",
		AttackFrom->status.experience, experience,
		AttackFrom->status.experience + experience);

	AttackFrom->status.experience += experience;

	// ��ʼ��ʾ����
	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_NUMBER_ID, TIMER_NUMBER_TIME);
}

// ��Ѱ������Χ(�з��ƶ�ʹ��)

void CBattleAction::FindAttackCalc(int x, int y, int dist)
{
	map_data[y][x].type |= ATTACKDIST;
	map_data[y][x].attack_dist = dist;
	if (dist == 0)
		return;
	if (y > MapRect.top && map_data[y - 1][x].ChkAttack(dist))
		FindAttackCalc(x, y - 1, dist - 1);
	if (y < MapRect.bottom - 1 && map_data[y + 1][x].ChkAttack(dist))
		FindAttackCalc(x, y + 1, dist - 1);
	if (x > MapRect.left && map_data[y][x - 1].ChkAttack(dist))
		FindAttackCalc(x - 1, y, dist - 1);
	if (x < MapRect.right - 1 && map_data[y][x + 1].ChkAttack(dist))
		FindAttackCalc(x + 1, y, dist - 1);
}

// �о�������ƶ�

void CBattleAction::MoveEnemy()
{
	TRACE("[%s]���ƶ�\n", enemy_ptr->status.name);
	enemy_ptr->move_done = true;			// ���ƶ�
	attack_target = 0;						// �����������

	// ����ֱ�ӹ����趨����
	magic_attack = false;
	int attack_dist = enemy_ptr->status.attack_dist;

	// �ж�������й���ħ�������ʹ��
	int magic_id = enemy_ptr->status.FindMagic(MAGIC_FIREBALL);
	if (magic_id >= 0 && enemy_ptr->IsMagicOk(magic_id)) {
		magic_attack = true;
		magic_param = enemy_ptr->GetMagicParam(magic_id);
		attack_dist = magic_param.dist;
	}

	CPoint mp = enemy_ptr->GetMapPoint();
	FindDist(mp.x, mp.y, enemy_ptr->status.move_dist);
	Parent->UpdateWindow();

	// Ϊ���ú���Ĵ����������Ŀǰλ��Ҳ�������ƶ���Χ֮��
	map_data[mp.y][mp.x].type |= MOVEDIST;
	map_data[mp.y][mp.x].move_dist = enemy_ptr->status.move_dist;

	int eval_point = INT_MIN;			// ���ֵ���
	CPoint move = mp;					// �ƶ�λ��

	for (list<CCharacter>::iterator p = player_list.begin(); p != player_list.end(); p++) {
		CPoint pos = p->GetMapPoint();
		FindAttackCalc(pos.x, pos.y, attack_dist);
		map_data[pos.y][pos.x].type &= ~ATTACKDIST;

		CRect rect(pos.x, pos.y, pos.x + 1, pos.y + 1);
		rect.InflateRect(attack_dist, attack_dist);
		rect &= MapRect;

		for (int y=rect.top; y<rect.bottom; y++) {
			for (int x=rect.left; x<rect.right; x++) {
				// �ж��Ƿ��п��ƶ����ɹ�����λ��
				if (map_data[y][x].ChkType(ATTACKDIST | MOVEDIST)) {
					// ���ֱ�׼Ϊ��
					// 1. �ƶ�����Խ��Խ��
					// 2. ��������ԽԶԽ��
					// 3. �Է���HPԽСԽ��
					// 4. �Է��Ĺ�����Խ��Խ��
					// ����ԭ��Ʒ�
					int point = (attack_dist - map_data[y][x].attack_dist) * 20
						+ map_data[y][x].move_dist * 50
						- p->status.hit_point * 5
						+ p->status.attack_power;

					// ���÷ֽϸߣ������
					if (eval_point < point) {
						eval_point = point;		// �µĵ÷�
						attack_target = &*p;	// �趨����Ŀ��
						move.x = x;				// �µ��ƶ�
						move.y = y;
					}
				}
				// �����ҵ����
				map_data[y][x].attack_dist = 0;
				map_data[y][x].type &= ~ATTACKDIST;
			}
		}
	}

	if (attack_target == 0) {		// �޷��ƶ����ɹ�����λ��
		CRect rect(mp.x, mp.y, mp.x + 1, mp.y + 1);
		rect.InflateRect(enemy_ptr->status.move_dist, enemy_ptr->status.move_dist);
		rect &= MapRect;

		eval_point = INT_MAX;		// ����÷�

		for (list<CCharacter>::iterator p = player_list.begin(); p != player_list.end(); p++) {
			CPoint pos = p->GetMapPoint();
			for (int y=rect.top; y<rect.bottom; y++) {
				for (int x=rect.left; x<rect.right; x++) {
					if (map_data[y][x].ChkType(MOVEDIST)) {
						int dist = abs(x - pos.x) + abs(y - pos.y);
						if (eval_point > dist) {
							eval_point = dist;
							move.x = x;
							move.y = y;
						}
					}
				}
			}
		}
	}

	// ���Ŀǰλ���е��ƶ���Χ���
	map_data[mp.y][mp.x].type &= ~MOVEDIST;
	map_data[mp.y][mp.x].move_dist = 0;

	::Sleep(70);				// �ȴ�ֱ�������ƶ���Χ����ʾ

	if (mp == move) {			// ���ƶ�
		ClearDistCursor();
	}
	else {
		MovePlayer(&*enemy_ptr, move);
	}
}

// �о�����Ĺ���

void CBattleAction::AttackEnemy()
{
	TRACE("[%s]�Ĺ���\n", enemy_ptr->status.name);
	enemy_ptr->attack_done = true;	// �ѹ���

	if (attack_target) {
		CPoint mp = enemy_ptr->GetMapPoint();

		if (magic_attack) {	// ħ������
			FindAttack(mp.x, mp.y, magic_param.dist);
			Parent->UpdateWindow();
			Sleep(100);
			Magic(&*enemy_ptr, attack_target, false);
		}
		else {				// ֱ�ӹ���
			FindAttack(mp.x, mp.y, enemy_ptr->status.attack_dist);
			Parent->UpdateWindow();
			Sleep(100);
			Attack(&*enemy_ptr, attack_target, false);
		}
	}
}
