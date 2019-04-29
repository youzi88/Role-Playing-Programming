//
// 执行战斗
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
// 构造函数
//
CBattleAction::CBattleAction(CMainWin *win, CAction *oldAction):
	CAction(win, oldAction), _map_data(0), map_data(0),
	ViewImage(win->GetViewImage()),
	MixedImage(win->GetMixedImage()),
	BackLayer(win->GetBackLayer())
{
	status = STATUS_NONE;
	turn = PLAYER_TURN;

	// 角色信息的显示
	for (int i=0; i<MAX_CHARACTER; i++) {
		StatusCharacter[i] = 0;
	}

	// 事前字型的产生
	hFont = CreateFont(-16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "宋体");
}

//
// 析构函数
//
CBattleAction::~CBattleAction()
{
	DeleteObject(hFont);

	delete _map_data;
	delete map_data;
}

//
// 暂停
//
void CBattleAction::Pause()
{
}

//
// 暂停解除
//
void CBattleAction::Resume()
{
}

//
// 按下鼠标左键时执行的处理
//
void CBattleAction::LButtonDown(UINT modKeys, CPoint point)
{
	// 只有轮到的游戏角色才有的鼠标按钮反应
	if (turn == PLAYER_TURN) {
		CPoint pos = PointToIndex(point);	// 需要地图座标
		switch (status) {
		  case STATUS_NONE:
			// 选取游戏角色
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
			// 点选时的移动范围
			if (MapRect.PtInRect(pos) && map_data[pos.y][pos.x].type & MOVEDIST) {
				MovePlayer(SelChar, pos);
				SelChar->move_done = true;
			}
			break;

		  case STATUS_ATTACK:
			// 已点选攻击目标
			if (MapRect.PtInRect(pos) && (map_data[pos.y][pos.x].type & (ATTACKDIST | ENEMY)) == (ATTACKDIST | ENEMY)) {
				Attack(SelChar, (CCharacter *)map_data[pos.y][pos.x].sprite, true);
				SelChar->attack_done = true;
			}
			break;

		  case STATUS_MAGIC_ENEMY:
			// 点选时的魔法范围
			if (MapRect.PtInRect(pos) && (map_data[pos.y][pos.x].type & (ATTACKDIST | ENEMY)) == (ATTACKDIST | ENEMY)) {
				Magic(SelChar, (CCharacter *)map_data[pos.y][pos.x].sprite, true);
				SelChar->attack_done = true;
			}
			break;

		  case STATUS_MAGIC_PLAYER:
			// 点选时的魔法范围
			if (MapRect.PtInRect(pos) && (map_data[pos.y][pos.x].type & (ATTACKDIST | PLAYER)) == (ATTACKDIST | PLAYER)) {
				Magic(SelChar, (CCharacter *)map_data[pos.y][pos.x].sprite, true);
				SelChar->attack_done = true;
			}
			break;

		  case STATUS_ANIME:
			break;

		  case STATUS_NEXTTURN:
			// 点选Turn End(轮完)结束
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
// 按下鼠标右键时执行的处理
//
void CBattleAction::RButtonDown(UINT modKeys, CPoint point)
{
	// 按下右键传回的动作
	if (turn == PLAYER_TURN) {
		switch (status) {
		  case STATUS_NONE:				// 若没有选取任何东西时
			status = STATUS_NEXTTURN;	// 显示“下一轮”(NEXT TURN)
			ShowTurnEnd(point);
			break;

		  case STATUS_COMMAND:			// 若选取了指令时
			status = STATUS_NONE;		// 设定状态为“无”
			HideCommand();
			break;

		  case STATUS_MOVE:				// 移动
		  case STATUS_ATTACK:			// 攻击范围
		  case STATUS_MAGIC_ENEMY:		// 魔法范围
		  case STATUS_MAGIC_PLAYER:		// 当指定角色
			status = STATUS_NONE;		// 指定回“无”的状态
			ClearDistCursor();
			break;

		  case STATUS_NEXTTURN:			// 若是下一轮(Next turn)时
			status = STATUS_NONE;		// 指定回“无”的状态
			HideTurnEnd();
			break;
		}
	}
}

//
// 移动鼠标时的处理
//
void CBattleAction::MouseMove(UINT modKeys, CPoint point)
{
	if (turn == PLAYER_TURN) {
		switch (status) {
		  case STATUS_NONE:			// 初始状态
		  case STATUS_MOVE:			// 显示移动范围
		  case STATUS_ATTACK:		// 显示攻击范围
		  case STATUS_MAGIC_ENEMY:	// 显示魔法范围
		  case STATUS_MAGIC_PLAYER:	// 显示魔法范围
			SetCursor(PointToIndex(point));
			break;

		  case STATUS_COMMAND:		// 显示指令菜单
			HighlightCommand(point);
			break;

		  case STATUS_NEXTTURN:		// 显示下一轮指令菜单
			SelectTurnEnd(TurnEndSprite.PtIn(point)? true: false);
			break;
		}
	}
}

//
// 按下键盘时的处理
//
void CBattleAction::KeyDown(UINT key)
{
}

//
// IDLE处理
//
BOOL CBattleAction::IdleAction()
{
	if (turn == ENEMY_TURN) {
		if (status != STATUS_ANIME) {
			if (!enemy_ptr->move_done) {		// 若尚未移动，则移动
				MoveEnemy();
			}
			else if (!enemy_ptr->attack_done) {	// 若尚未攻击，则攻击
				AttackEnemy();
			}
			else {
				if (status == STATUS_BATTLE_END) {	// 结束
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
// 计时器的处理
//
bool CBattleAction::TimedOut(int timerId)
{
	switch (timerId) {
	  case TIMER_MOVE_ID:	// 移动动画
		{
			ASSERT(status == STATUS_ANIME);
			MoveAnime &ar = anime.front();
			CCharacter *character = ar.character;
			RemoveSprite(character);
			character->MoveAnime(ar.action, ar.count);
			AddSprite(character);
			if (ar.count++ == 3) {	// 4个模式后前进到下一格
				anime.pop_front();
				if (anime.size() == 0) {	// 若不再登录，则结束动画
					CPoint pos = character->GetMapPoint();
					map_data[pos.y][pos.x].type |= (turn == PLAYER_TURN)? PLAYER: ENEMY;
					map_data[pos.y][pos.x].sprite = character;
					status = STATUS_NONE;
					return true;
				}
			}
		}
		break;

	  case TIMER_ATTACK_ID:	// 峌攻击动画
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

	  case TIMER_NUMBER_ID:	// 破坏力动画
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

			// 检查提升等级
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

	  case TIMER_MAGIC_ID:	// 魔法动画
		ASSERT(status == STATUS_ANIME);
		switch (magic_param.id) {
		  case MAGIC_HEAL:			// 恢复
			return MagicHeal();

		  case MAGIC_FIREBALL:		// Fire ball
			return MagicFireBall();
		}
		break;

	  case TIMER_LEVELUP_ID:	// 提升等级的动画
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
// 擦入(Wipe)结束时的处理
//
void CBattleAction::WipeDone()
{
	if (status == STATUS_WAIT_START)	// 开始Wipe等待
		status = STATUS_NONE;
}

//
// Wave播放结束时的处理
//
void CBattleAction::WaveDone()
{
}

//
// 结束执行
//
void CBattleAction::Abort()
{
}

//
// 输出错误事件
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
		Error(reader, "文法错误");
		return false;
	}
	CChip chip(name, file);
	if (chip.image == 0) {
		Error(reader, "无法读取CG资料");
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
		Error(reader, "文法错误");
		return false;
	}
	CChip chip(name, file);
	if (chip.image == 0) {
		Error(reader, "无法读取CG资料");
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
		Error(reader, "文法错误");
		return false;
	}
	CChip chip(name, file);
	if (chip.image == 0) {
		Error(reader, "无法读取CG资料");
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
		Error(reader, "文法错误");
		return false;
	}
	MapSize.cx = width;
	MapSize.cy = height;

	MapRect.SetRect(0, 0, width, height);

	// map_data偺椞堟妋曐
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
		Error(reader, "文法错误");
		return false;
	}
	if (!MapImage.LoadImage(name)) {
		Error(reader, "无法读取CG资料");
		return false;
	}
	return true;
}

//
// 把map上的文字改成状态
//
int CBattleAction::FindMapStatus(const char *find)
{
	struct {
		char str[4];
		unsigned type;
	} table[] = {
		{ "□", NONE },
		{ "■", RESTRICTED_AREA },
		{ "一", PLAYER | (0 << 8) },
		{ "二", PLAYER | (1 << 8) },
		{ "三", PLAYER | (2 << 8) },
		{ "四", PLAYER | (3 << 8) },
		{ "五", PLAYER | (4 << 8) },
		{ "六", PLAYER | (5 << 8) },
		{ "七", PLAYER | (6 << 8) },
		{ "八", PLAYER | (7 << 8) },
		{ "九", PLAYER | (8 << 8) },
		{ "１", ENEMY | (0 << 8) },
		{ "２", ENEMY | (1 << 8) },
		{ "３", ENEMY | (2 << 8) },
		{ "４", ENEMY | (3 << 8) },
		{ "５", ENEMY | (4 << 8) },
		{ "６", ENEMY | (5 << 8) },
		{ "７", ENEMY | (6 << 8) },
		{ "８", ENEMY | (7 << 8) },
		{ "９", ENEMY | (8 << 8) },
		{ "Ⅰ", OBJECT | (0 << 8) },
		{ "Ⅱ", OBJECT | (1 << 8) },
		{ "Ⅲ", OBJECT | (2 << 8) },
		{ "Ⅳ", OBJECT | (3 << 8) },
		{ "Ⅴ", OBJECT | (4 << 8) },
		{ "Ⅵ", OBJECT | (5 << 8) },
		{ "Ⅶ", OBJECT | (6 << 8) },
		{ "Ⅷ", OBJECT | (7 << 8) },
		{ "Ⅸ", OBJECT | (8 << 8) },
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
	if (map_data == 0) {	// 此时，已经要决定地图的大小
		Error(reader, "请先设定mapsize");
		return false;
	}

	for (int y=0; y<MapSize.cy; y++) {
		const char *str = reader.GetString();
		if (str == 0) {
			Error(reader, "文法错误");
			return false;
		}
		for (int x=0; x<MapSize.cx; x++) {
			char chip[3];
			chip[0] = str[x * 2 + 0];
			chip[1] = str[x * 2 + 1];
			chip[2] = 0;

			int idx = FindMapStatus(chip);
			if (idx < 0) {
				Error(reader, "文法错误");
				return false;
			}
			map_data[y][x].type = (unsigned char)idx;
			if (idx != RESTRICTED_AREA && idx != NONE) {
				map<int, CChip>::iterator p = chip_list.find(idx);
				if (p == chip_list.end()) {		// 尚未登录
					Error(reader, "人物角色尚未登录");
					return false;
				}
				else {
					if ((idx & PLAYER) != 0) {
						player_list.push_back(CCharacter(p->second.image, x, y, CCharacter::DEPTH_CHAR));
						CCharacter &s = player_list.back();

						// 根据人物姓名读入状态
						if (!Parent->GetParam().GetStatus(p->second.name.c_str(), &s.status)) {	// 无的状态
							Error(reader, "找不到玩家人物%s的状态信息");
							return false;
						}

						sprite.insert(&s);
						s.SetDirection(CCharacter::RIGHT);
						map_data[y][x].sprite = &s;

						StatusCharacter[idx >> 8] = &s;	// 显示用人物相关信息
					}
					else if ((idx & ENEMY) != 0) {
						enemy_list.push_back(CCharacter(p->second.image, x, y, CCharacter::DEPTH_CHAR));
						CCharacter &s = enemy_list.back();

						// 根据人物姓名读入状态
						if (!Parent->GetParam().GetStatus(p->second.name.c_str(), &s.status)) {	// 无的状态
							Error(reader, "找不到敌方人物%s的状态信息");
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
// 读入地图定义档
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
		Parent->MessageBox("无法读取地图文件。");
		return false;
	}

	const char *str;
	while ((str = reader.GetString()) != 0) {
		Lexer	lexer(str);

		if (lexer.NumToken()) {
			const char *cmd = lexer.GetString(0);
			if (stricmp(cmd, "end") == 0)	// 结束
				break;

			cmdmap::iterator p = cmd_table.find(cmd);
			if (p != cmd_table.end()) {
				if (!(this->*p->second)(reader, lexer))
					return false;
			}
			else {
				Error(reader, "文法错误");
				return false;
			}
		}
	}
	return true;
}

//
// 读入战斗地图的资料
//
bool CBattleAction::Load(const char *name)
{
	// 若仍残留之前的状态，则为error
	// 应该不会有这个状态，故设为ASSERT
	ASSERT(map_data == 0);
	ASSERT(_map_data == 0);

	// 读入地图资料
	if (!LoadMap(name))
		return false;

	// 读入CG资料
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
	// 产生图像
	CClientDC	dc(Parent);
	if (!PopupImage.Create(dc, PopupImageBase.Width(), PopupImageBase.Height()))
		return false;

	// 设定sprite
	CursorSprite.Set(&Cursor, CPoint(0, 0), CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_CURSOR);

	{
		for (int i=0; i<MAX_COMMAND; i++) {
			CommandSprite[i].Set(&Command, CPoint(0, 0), CPoint(0, i * 22), CSize(70, 22), 0);
			CommandSprite[i].Show(false);
		}
	}
	TurnEndSprite.Set(&TurnEnd, CPoint(0, 0), CPoint(0, 0), CSize(70, 22), 0);
	PopupSprite.Set(&PopupImage, CPoint(0, 0), CPoint(0, 0), PopupImage.Size(), 0);

	// 人物的状态
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

	// 清除事件
	Parent->ClearMessage();
	// 清除重叠
	Parent->ClearOverlap();

	// 制作背景图像
	DrawMap(BackLayer);

	// Wipe in
	Parent->WipeIn();

	return true;
}

//
// 产生地图画面
//
void CBattleAction::DrawMap(CImage *image, const CRect &rect)
{
	// 若地图有背景，则显示之
	if (MapImage.IsOK()) {
		image->Copy(&MapImage, rect);
	}
	// 显示sprite
	for (mapset::iterator p = sprite.begin(); p != sprite.end(); p++) {
		(*p)->Draw(image, rect);
	}
	// 显示角色的状态
	for (int i=0; i<MAX_CHARACTER; i++) {
		StatusSprite[i].Draw(image, rect);
	}
	// 显示光标位置的状态
	if (StatusChar != 0) {
		PopupSprite.Draw(image, rect);
	}
	// 照编号显示贴图零件(sprite)
	for (int j=0; j<NumberView; j++)
		Number[j].Draw(image, rect);

	// 显示菜单
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
// 显示地图（整个地图）
//
void CBattleAction::DrawMap(CImage *image)
{
	DrawMap(image, CRect(0, 0, WindowWidth, WindowHeight));
}

//
// 建立人物状态的图像
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
		dc.ExtTextOut(10, 22, 0, 0, str, sprintf(str, "等级　　　%7d", status.level), NULL);
		dc.ExtTextOut(10, 40, 0, 0, str, sprintf(str, "经验值　　%7d", status.experience), NULL);
		dc.ExtTextOut(10, 58, 0, 0, str, sprintf(str, "HP值　　%3d/%3d", status.hit_point, status.max_hit_point), NULL);
		dc.ExtTextOut(10, 76, 0, 0, str, sprintf(str, "MP值　　%3d/%3d", status.magic_point, status.max_magic_point), NULL);

		dc.SelectObject(oldFont);
	}
}

//
// 显示角色的状态
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
// 显示突现式状态说明
//
void CBattleAction::ChangeStatus(CPoint point)
{
	if (MapRect.PtInRect(point) && map_data[point.y][point.x].type & (PLAYER | ENEMY)) {
		if (StatusChar != (CCharacter *)map_data[point.y][point.x].sprite) {
			StatusChar = 0;

			RedrawSprite(&PopupSprite);		// 删除之前的显示
			StatusChar = (CCharacter *)map_data[point.y][point.x].sprite;
			CStatus &status = StatusChar->status;

			// 组合状态显示的零件
			PopupImage.Copy(&PopupImageBase);	// 复制原始图像
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

			// 若不在范围内，则放在窗口内
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
			RedrawSprite(&PopupSprite);		// 显示
		}
	}
	else if (StatusChar != 0) {				// 有显示状态，但不在人物上
		StatusChar = 0;
		RedrawSprite(&PopupSprite);			// 删除之前的显示
	}
}

//
// 移动地图光标
//
void CBattleAction::SetCursor(CPoint point)
{
	if (CursorPos != point) {		// 若地图位置有更改
		ClearCursor();				// 则删除目前显示的光标
		CursorPos = point;			// 重新设定新的位置
		DrawCursor();				// 显示新的光标
		ChangeStatus(point);		// 变更角色的状态
	}
}

//
// 删除地图光标
//
void CBattleAction::ClearCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		RemoveSprite(&CursorSprite);
	}
	CursorPos.x = CursorPos.y = -1;
}

//
// 显示地图光标
//
void CBattleAction::DrawCursor()
{
	if (MapRect.PtInRect(CursorPos)) {
		CursorSprite.SetMapPoint(CursorPos);
		AddSprite(&CursorSprite);
	}
}

// 重新绘制sprite

void CBattleAction::RedrawSprite(CSprite *s)
{
	CRect	rect;
	s->GetRect(&rect);
	UpdateRect(rect);
}

// 新增sprite

void CBattleAction::AddSprite(CSprite *s)
{
	sprite.insert(s);
	RedrawSprite(s);
}

// 删除sprite

void CBattleAction::RemoveSprite(CSprite *s)
{
	if (sprite.erase(s)) {
		RedrawSprite(s);
	}
}

// 寻找移动范围

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

// 搜寻攻击范围

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

// 显示范围

void CBattleAction::AddDistCursor(int x, int y, int dist, int type)
{
	if ((map_data[y][x].type & (MOVEDIST | ATTACKDIST)) != 0)	// 已登录的部分
		return;

	cursor_list.push_back(CMapSprite(&Cursor, x, y, CSize(MAPGRID_WIDTH, MAPGRID_HEIGHT), CMapSprite::DEPTH_AREA));
	CMapSprite &s = cursor_list.back();
	s.SetSrcPos(0, 32 + type * 32);
	AddSprite(&s);
}

// 删除显示范围

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

// 人物移动

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

// 登录人物的移动动画

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

// 执行移动动画

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

// 变成轮到敌方

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

// 显示命令菜单

CPoint CBattleAction::CommandIndex(int idx)
{
	// 移动
	switch (idx) {
	  case COMMAND_MOVE:		// 移动
		return CPoint(SelChar->move_done? 140: 0, 0);

	  case COMMAND_ATTACK:		// 直接攻击
		return CPoint(SelChar->attack_done? 140: 0, 22);

	  default:					// 魔法
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

// 选择命令菜单--让它反白

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

// 删除命令菜单

void CBattleAction::HideCommand()
{
	for (int i=0; i<MAX_COMMAND; i++) {
		if (CommandSprite[i].IsShow()) {
			CommandSprite[i].Show(false);
			RedrawSprite(CommandSprite + i);
		}
	}
}

// 选择命令菜单

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

// 从命令菜单的位置开始搜寻

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

// 显示 TURN END 菜单

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

// 隐藏 TURN END 菜单

void CBattleAction::HideTurnEnd()
{
	RedrawSprite(&TurnEndSprite);
}

// 选取 TURN END 菜单

void CBattleAction::SelectTurnEnd(bool sel)
{
	if (sel_turnend != sel) {
		sel_turnend = sel;
		TurnEndSprite.SetSrcPos(sel? 70: 0, 0);
		RedrawSprite(&TurnEndSprite);
	}
}

// 攻击

void CBattleAction::Attack(CCharacter *from, CCharacter *to, bool player)
{
	// 删除光标
	SetCursor(CPoint(-1, -1));

	// 删除范围显示
	ClearDistCursor();
	CPoint pos = from->GetMapPoint();
	map_data[pos.y][pos.x].type &= ~ATTACKDIST;
	map_data[pos.y][pos.x].attack_dist = 0;

	CSize diff = from->GetMapPoint() - to->GetMapPoint();

	// 调整人物方向
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

	// 攻击的动画开始
	AttackFrom = from;
	AttackTo = to;
	AttackPlayerToEnemy = player;
	AttackAnimeCount = 0;

	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_ATTACK_ID, TIMER_ATTACK_TIME);

	// 播放音效
	Parent->StartWave(ATTACK_WAVE);
}

// 攻击阶段2

void CBattleAction::AttackPhase2()
{
	// 判断攻击
	// debug用，追踪变量
	TRACE("攻方参数:[%s]\n", AttackFrom->status.name);
	TRACE(" 攻击力 : %4d\n", AttackFrom->status.attack_power);
	TRACE(" 防御力 : %4d\n", AttackFrom->status.defence_power);
	TRACE(" 剩余HP : %4d\n", AttackFrom->status.hit_point);
	TRACE("守方参数:[%s]\n", AttackTo->status.name);
	TRACE(" 攻击力 : %4d\n", AttackTo->status.attack_power);
	TRACE(" 防御力 : %4d\n", AttackTo->status.defence_power);
	TRACE(" 剩余HP : %4d\n", AttackTo->status.hit_point);

	// 这个方法只用来计算而已，并没有使用乱数。
	// 破坏力 = 攻方攻击力 * 攻方攻击力 / 守方攻击力
	int ap = AttackFrom->status.attack_power;
	int dp = AttackTo->status.defence_power == 0? 1: AttackTo->status.defence_power;
	int attack = ap * ap / dp;
	if (attack <= 0)	// 最低1点就有破坏力
		attack = 1;

	// 显示破坏力
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

	TRACE(" 破坏力 : %4d\n", attack);

	AttackTo->status.hit_point -= attack;

	TRACE(" 剩余HP : %4d\n", AttackTo->status.hit_point);

	// 计算经验值
	// 每次攻击就有基本 10 point，若被打败倒地则+20 point
	// 如果有等级差距时
	// 攻方等级 ＞ 守方等级  减低所获得的经验值
	// 攻方等级 ＜ 守方等级  提高所获得的经验值
	int experience = 10;
	if (AttackTo->status.hit_point == 0)
		experience += 20;
	int level_diff = AttackFrom->status.level - AttackTo->status.level;
	if (level_diff > 0) {	// 攻击较弱的对手
		experience >>= level_diff;
	}
	else if (level_diff) {
		experience <<= -level_diff;
	}

	TRACE(" 经验值 = %d + %d = %d\n",
		AttackFrom->status.experience, experience,
		AttackFrom->status.experience + experience);

	AttackFrom->status.experience += experience;

	// 破坏力的显示开始
	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_NUMBER_ID, TIMER_NUMBER_TIME);
}

// 攻击阶段3

void CBattleAction::AttackPhase3()
{
	// 最后结束的处理
	status = STATUS_NONE;

	// 游戏者状态显示的变换
	ChangePlayerStatus(AttackFrom);
	ChangePlayerStatus(AttackTo);

	// 死亡的判定
	if (AttackTo->status.hit_point == 0) {	// HP值没有了就等于死亡
		CPoint po = AttackTo->GetMapPoint();
		map_data[po.y][po.x].sprite = 0;
		map_data[po.y][po.x].type &= ~(PLAYER | ENEMY | OBJECT);
		RemoveSprite(AttackTo);		// 从sprite清单中删掉

		//
		// 判断战斗结束
		//
		// 这里最后的状况是全军覆没，
		// 可由游戏角色死亡来判断
		//
		if (turn == PLAYER_TURN) {
			list<CCharacter>::iterator p = find(enemy_list.begin(), enemy_list.end(), *AttackTo);
			if (&*p != AttackTo) {
				TRACE("error\n");
			}
			ASSERT(p != enemy_list.end());
			enemy_list.erase(p);
			if (enemy_list.empty()) {	// 全军覆没
				TRACE("打赢\n");
				BattleEnd(true);
			}
		}
		else {
			// 人物阵亡前要把状态储存回去
			Parent->GetParam().SetStatus(AttackTo->status.name, &AttackTo->status);

			list<CCharacter>::iterator p = find(player_list.begin(), player_list.end(), *AttackTo);
			if (&*p != AttackTo) {
				TRACE("error\n");
			}
			ASSERT(p != player_list.end());
			player_list.erase(p);
			if (player_list.empty()) {	// 全军覆没
				TRACE("打输\n");
				BattleEnd(false);
			}
		}
	}
}

// 提升等级

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

	// 播放音效
	Parent->StartWave(LEVELUP_WAVE);
}

// 战斗结束

void CBattleAction::BattleEnd(bool iswin)
{
	// 结束所有动作
	status = STATUS_BATTLE_END;

	// 将胜负结果储存到变量
	Parent->SetValue("battle", iswin? 1: 0);

	// 先把人物的状态储存回去
	for (list<CCharacter>::iterator p = player_list.begin(); p != player_list.end(); ++p) {
		Parent->GetParam().SetStatus(p->status.name, &p->status);
	}

	// 把显示中的内容储存到“背景”，以便移往事件
	BackLayer->Copy(ViewImage);
	Parent->Invalidate();

	// CBattleAction作废
	Parent->ScriptDone();
}

// 魔法

void CBattleAction::Magic(CCharacter *from, CCharacter *to, bool player)
{
	// 删除光标
	SetCursor(CPoint(-1, -1));

	// 清除显示范围
	ClearDistCursor();
	CPoint pos = from->GetMapPoint();
	map_data[pos.y][pos.x].type &= ~ATTACKDIST;
	map_data[pos.y][pos.x].attack_dist = 0;

	CSize diff = from->GetMapPoint() - to->GetMapPoint();

	// 调整人物角色的方向
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

	// 攻击动画开始
	AttackFrom = from;
	AttackTo = to;
	AttackPlayerToEnemy = player;
	AttackAnimeCount = 0;
	AttackAnimeState = 0;

	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_MAGIC_ID, TIMER_MAGIC_TIME);

	// 播放效果音
	switch (magic_param.id) {
	  case MAGIC_HEAL:			// 恢复
		Parent->StartWave(HEAL_WAVE);
		break;

	  case MAGIC_FIREBALL:		// Fire ball
		Parent->StartWave(FIREBALL_WAVE);
		break;
	}
}

// 魔法动作(Heal)

bool CBattleAction::MagicHeal()
{
	switch (AttackAnimeState) {
	  case 0:		// 第1阶段  魔法发动
		if (AttackFrom->MagicAnime(AttackAnimeCount++)) {
			AttackAnimeCount = 0;
			AttackAnimeState++;

			MagicSprite[0].Set(&HealImage, CPoint(0, 0), CPoint(0, 0),
				CSize(HealImage.Width() / 7, HealImage.Height() / 2), 10);	// 前
			MagicSprite[1].Set(&HealImage, CPoint(0, 0), CPoint(0, HealImage.Height() / 2),
				CSize(HealImage.Width() / 7, HealImage.Height() / 2), -10);	// 后傠
			MagicSprite[0].SetMapPoint(AttackTo->GetMapPoint());
			MagicSprite[1].SetMapPoint(AttackTo->GetMapPoint());
			AddSprite(MagicSprite);
			AddSprite(MagicSprite + 1);
		}
		RedrawSprite(AttackFrom);
		break;

	  case 1:		// 第2阶段  效果发挥
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

// 第2阶段的魔法恢复/补血

void CBattleAction::HealPhase2()
{
	// 判断魔法
	// debug用，追踪变量
	TRACE("攻方参数:[%s]\n", AttackFrom->status.name);
	TRACE(" 攻击力 : %4d\n", AttackFrom->status.attack_power);
	TRACE(" 防御力 : %4d\n", AttackFrom->status.defence_power);
	TRACE(" 剩余HP : %4d\n", AttackFrom->status.hit_point);
	TRACE("守方参数:[%s]\n", AttackTo->status.name);
	TRACE(" 攻击力 : %4d\n", AttackTo->status.attack_power);
	TRACE(" 防御力 : %4d\n", AttackTo->status.defence_power);
	TRACE(" 剩余HP : %4d\n", AttackTo->status.hit_point);

	TRACE("使用魔法 id    : %d\n", magic_param.id);
	TRACE("         type  : %d\n", magic_param.type);
	TRACE("         dist  : %d\n", magic_param.dist);
	TRACE("         power : %d\n", magic_param.power);
	TRACE("         use_mp: %d\n", magic_param.use_mp);

	// MP的消耗
	AttackFrom->status.magic_point -= magic_param.use_mp;

	// 损伤的显示
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

	TRACE(" 剩余HP : %4d\n", AttackTo->status.hit_point);

	// 经验值的计算
	AttackFrom->status.experience += 10;

	// 开始显示恢复量
	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_NUMBER_ID, TIMER_NUMBER_TIME);
}

// 魔法动作(FireBall)

bool CBattleAction::MagicFireBall()
{
	switch (AttackAnimeState) {
	  case 0:		//第1阶段  魔法发动
		if (AttackFrom->MagicAnime(AttackAnimeCount++)) {
			AttackAnimeCount = 0;
			AttackAnimeState++;

			MagicSprite[0].SetFireBall(&FireBallImage, AttackFrom);
			AddSprite(MagicSprite);
		}
		RedrawSprite(AttackFrom);
		break;

	  case 1:		// 第2阶段  火焰的移动
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

	  case 2:		// 第3阶段  爆炸
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

// 第2阶段的攻击魔法

void CBattleAction::FireBallPhase2()
{
	// 判断魔法
	// debug用，追踪变量
	TRACE("攻方参数:[%s]\n", AttackFrom->status.name);
	TRACE(" 攻击力 : %4d\n", AttackFrom->status.attack_power);
	TRACE(" 防御力 : %4d\n", AttackFrom->status.defence_power);
	TRACE(" 剩余HP : %4d\n", AttackFrom->status.hit_point);
	TRACE("守方参数:[%s]\n", AttackTo->status.name);
	TRACE(" 攻击力 : %4d\n", AttackTo->status.attack_power);
	TRACE(" 防御力 : %4d\n", AttackTo->status.defence_power);
	TRACE(" 剩余HP : %4d\n", AttackTo->status.hit_point);

	TRACE("使用魔法 id    : %d\n", magic_param.id);
	TRACE("         type  : %d\n", magic_param.type);
	TRACE("         dist  : %d\n", magic_param.dist);
	TRACE("         power : %d\n", magic_param.power);
	TRACE("         use_mp: %d\n", magic_param.use_mp);

	// MP的消耗
	AttackFrom->status.magic_point -= magic_param.use_mp;
	int ap = magic_param.power;
	int dp = AttackTo->status.resistance == 0? 1: AttackTo->status.resistance;
	int attack = ap * ap / dp;
	if (attack <= 0)	// 破坏力至少1点
		attack = 1;

	// 损伤的显示
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

	TRACE(" 损伤 : %4d\n", attack);

	AttackTo->status.hit_point -= attack;

	TRACE(" 剩余HP : %4d\n", AttackTo->status.hit_point);

	// 计算经验值
	// 每次攻击就有基本 10 point，若被打败倒地则+20 point
	// 如果有等级差距时
	// 攻方等级 ＞ 守方等级  减低所获得的经验值
	// 攻方等级 ＜ 守方等级  提高所获得的经验值
	int experience = 10;
	if (AttackTo->status.hit_point == 0)
		experience += 20;
	int level_diff = AttackFrom->status.level - AttackTo->status.level;
	if (level_diff > 0) {	// 攻击较弱的对手
		experience >>= level_diff;
	}
	else if (level_diff) {
		experience <<= -level_diff;
	}

	TRACE(" 经验值 = %d + %d = %d\n",
		AttackFrom->status.experience, experience,
		AttackFrom->status.experience + experience);

	AttackFrom->status.experience += experience;

	// 开始显示损伤
	AttackAnimeCount = 0;
	status = STATUS_ANIME;
	Parent->SetTimer(TIMER_NUMBER_ID, TIMER_NUMBER_TIME);
}

// 搜寻攻击范围(敌方移动使用)

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

// 敌军人物的移动

void CBattleAction::MoveEnemy()
{
	TRACE("[%s]的移动\n", enemy_ptr->status.name);
	enemy_ptr->move_done = true;			// 已移动
	attack_target = 0;						// 清除攻击对象

	// 当作直接攻击设定参数
	magic_attack = false;
	int attack_dist = enemy_ptr->status.attack_dist;

	// 判断如果还有攻击魔法，则可使用
	int magic_id = enemy_ptr->status.FindMagic(MAGIC_FIREBALL);
	if (magic_id >= 0 && enemy_ptr->IsMagicOk(magic_id)) {
		magic_attack = true;
		magic_param = enemy_ptr->GetMagicParam(magic_id);
		attack_dist = magic_param.dist;
	}

	CPoint mp = enemy_ptr->GetMapPoint();
	FindDist(mp.x, mp.y, enemy_ptr->status.move_dist);
	Parent->UpdateWindow();

	// 为了让后面的处理好做，让目前位置也包含到移动范围之内
	map_data[mp.y][mp.x].type |= MOVEDIST;
	map_data[mp.y][mp.x].move_dist = enemy_ptr->status.move_dist;

	int eval_point = INT_MIN;			// 评分点数
	CPoint move = mp;					// 移动位置

	for (list<CCharacter>::iterator p = player_list.begin(); p != player_list.end(); p++) {
		CPoint pos = p->GetMapPoint();
		FindAttackCalc(pos.x, pos.y, attack_dist);
		map_data[pos.y][pos.x].type &= ~ATTACKDIST;

		CRect rect(pos.x, pos.y, pos.x + 1, pos.y + 1);
		rect.InflateRect(attack_dist, attack_dist);
		rect &= MapRect;

		for (int y=rect.top; y<rect.bottom; y++) {
			for (int x=rect.left; x<rect.right; x++) {
				// 判断是否有可移动、可攻击的位置
				if (map_data[y][x].ChkType(ATTACKDIST | MOVEDIST)) {
					// 评分标准为：
					// 1. 移动距离越少越好
					// 2. 攻击距离越远越好
					// 3. 对方的HP越小越好
					// 4. 对方的攻击力越高越好
					// 依此原则计分
					int point = (attack_dist - map_data[y][x].attack_dist) * 20
						+ map_data[y][x].move_dist * 50
						- p->status.hit_point * 5
						+ p->status.attack_power;

					// 若得分较高，则更新
					if (eval_point < point) {
						eval_point = point;		// 新的得分
						attack_target = &*p;	// 设定攻击目标
						move.x = x;				// 新的移动
						move.y = y;
					}
				}
				// 清除作业区域
				map_data[y][x].attack_dist = 0;
				map_data[y][x].type &= ~ATTACKDIST;
			}
		}
	}

	if (attack_target == 0) {		// 无法移动到可攻击的位置
		CRect rect(mp.x, mp.y, mp.x + 1, mp.y + 1);
		rect.InflateRect(enemy_ptr->status.move_dist, enemy_ptr->status.move_dist);
		rect &= MapRect;

		eval_point = INT_MAX;		// 清除得分

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

	// 清除目前位置中的移动范围旗标
	map_data[mp.y][mp.x].type &= ~MOVEDIST;
	map_data[mp.y][mp.x].move_dist = 0;

	::Sleep(70);				// 等待直到出现移动范围的显示

	if (mp == move) {			// 不移动
		ClearDistCursor();
	}
	else {
		MovePlayer(&*enemy_ptr, move);
	}
}

// 敌军人物的攻击

void CBattleAction::AttackEnemy()
{
	TRACE("[%s]的攻击\n", enemy_ptr->status.name);
	enemy_ptr->attack_done = true;	// 已攻击

	if (attack_target) {
		CPoint mp = enemy_ptr->GetMapPoint();

		if (magic_attack) {	// 魔法攻击
			FindAttack(mp.x, mp.y, magic_param.dist);
			Parent->UpdateWindow();
			Sleep(100);
			Magic(&*enemy_ptr, attack_target, false);
		}
		else {				// 直接攻击
			FindAttack(mp.x, mp.y, enemy_ptr->status.attack_dist);
			Parent->UpdateWindow();
			Sleep(100);
			Attack(&*enemy_ptr, attack_target, false);
		}
	}
}
