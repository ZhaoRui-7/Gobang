#include<stdio.h>
#define SIZE 15
#define BLACKtem 11
#define WHITEtem 21
#define BLACK 10
#define WHITE 20

int Board[SIZE][SIZE];
int b, d;//a,b为本次落子位置，c,d,为上次落子位置
char a, c;
int mytem, my, other;
int y1, y2, x1, x2;//computer落子位置
int forbid1;//禁手
int supposemy[10][10][10], supposeother[10][10][10];//存储往前看时得分，[我或对方][往前看第几步][第几好的落子位置]
int supposey[10][10][10], supposex[10][10][10];//存储往前看时落子位置
int count;//记录步数
int error = 0;

struct direction {//四个棋盘方向
	int dy;
	int dx;
};
struct direction d1 = { 0,1 };//横
struct direction d2 = { 1,0 };//竖
struct direction d3 = { 1,-1 };//撇
struct direction d4 = { 1,1 };//捺
struct direction dir;

void luozi(int mode);//落子函数
void DisplayBoard(int mode);
void ClearBoardArray();
void caipan();//裁判函数，判断输赢
int score(int y, int x, int who, int scoretype);//评分函数，y，x为评分点，who为自己或对方落在该点，scoretype为1表示对已下棋局面评分，找最优点，为2表示对向前看的预测局面进行评分
void computer();//控制电脑下棋位置
void renji();//负责选择人机模式，并对my，other，mytem赋值
void supother(int y, int z);//往前看时判断对方如何下
void supmy(int y, int z);//往前看时判断自己如何下
void supother2(int y, int z);//往前看时局面评分
void supmy2(int y, int z);//往前看时局面评分

int main()
{

	int mode;
	a = 7;
	b = 7;

	printf("请选择对战模式，人机大战输入1，人人对战输入2：");
	scanf("%d", &mode);
	//初始化棋盘数组
	ClearBoardArray();

	//设定指定位置的棋子为黑子
	Board[7][7] = BLACK;


	if (mode == 1) {//人机
		renji();
		for (count = 0; count < 225; count++) {
			system("cls");
			printf("%d\n", count);
			DisplayBoard(mode);
			caipan();
			luozi(mode);
		}
	}
	else if (mode == 2) {

		for (count = 0; count < 225; count++) {
			system("cls");
			DisplayBoard(mode);
			caipan();
			luozi(mode);
		}
	}
	return  0;
}

int score(int y, int x, int player, int scoretype)
{
	int kind[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };//记录棋型的数组
	int nextx, nexty, negx, negy, posx, posy;//评分位置正向、负向相邻的位置
	int line[10];//连子正负向的情况，line[1]-line[4]为负向，line[6]-line[9]为正向
	int len1, len2, len;//连子长度
	int i, j, s;
	int opp;

	if (player == 10)
		opp = 20;
	else if (player == 20)
		opp = 10;

	for (i = 1; i <= 4; i++) {
		if (i == 1)
			dir = d1;
		else if (i == 2)
			dir = d2;
		else if (i == 3)
			dir = d3;
		else if (i == 4)
			dir = d4;

		nextx = x - dir.dx;
		nexty = y - dir.dy;

		negx = x;
		negy = y;

		for (len1 = 1; nextx >= 0 && nextx <= 14 && nexty >= 0 && nexty <= 14 && (Board[nexty][nextx] == player || Board[nexty][nextx] == player + 1 || Board[nexty][nextx] == player + 2); len1++) {
			negx = nextx;
			negy = nexty;
			nextx = nextx - dir.dx;
			nexty = nexty - dir.dy;
		}

		nextx = x + dir.dx;
		nexty = y + dir.dy;

		posx = x;
		posy = y;

		for (len2 = 1; nextx >= 0 && nextx <= 14 && nexty >= 0 && nexty <= 14 && (Board[nexty][nextx] == player || Board[nexty][nextx] == player + 1 || Board[nexty][nextx] == player + 2); len2++) {
			posx = nextx;
			posy = nexty;
			nextx = nextx + dir.dx;
			nexty = nexty + dir.dy;
		}

		len = len1 + len2 - 1;

		for (j = 1; j <= 4; j++) {
			nextx = negx - dir.dx * j;
			nexty = negy - dir.dy * j;
			if (nextx >= 0 && nextx <= 14 && nexty >= 0 && nexty <= 14) {
				line[j] = Board[nexty][nextx];
				if (line[j] <= 9 && line[j] >= 1)
					line[j] = 0;
			}
			else {
				line[j] = opp;
			}
			nextx = posx + dir.dx * j;
			nexty = posy + dir.dy * j;
			if (nextx >= 0 && nextx <= 14 && nexty >= 0 && nexty <= 14) {
				line[j + 5] = Board[nexty][nextx];
				if (line[j + 5] <= 9 && line[j + 5] >= 1)
					line[j + 5] = 0;
			}
			else {
				line[j + 5] = opp;
			}
		}

		if (len > 5) {
			kind[0]++;//长连
		}
		else if (len == 5) {
			kind[1]++;//连5
		}
		else if (len == 4) {
			if (line[1] == 0 && line[6] == 0) 
				kind[2]++;//活4
			else if (line[1] == 0 || line[6] == 0) 
				kind[3]++;//半死4
			else 
				kind[11]++;//死4（无意义）
		}
		else if (len == 3) {
			if ((line[1] == 0 && (line[2] == player || line[2] == player + 1 || line[2] == player + 2)) || (line[6] == 0 && (line[7] == player || line[7] == player + 1 || line[7] == player + 2))) 
				kind[4]++;//冲4
			else if (line[1] == 0 && line[6] == 0 && (line[2] == 0 || line[7] == 0)) 
				kind[5]++;//活3
			else if ((line[1] == 0 && line[2] == 0) || (line[6] == 0 && line[7] == 0)) 
				kind[7]++;//半死3
			else if (line[1] == 0 && line[6] == 0) 
				kind[7]++;//半死3
			else 
				kind[11]++;//死三（无意义）
		}
		else if (len == 2) {
			if ((line[1] == 0 && (line[2] == player || line[2] == player + 1 || line[2] == player + 2) && (line[3] == player || line[3] == player + 1 || line[3] == player + 2)) ||
				(line[6] == 0 && (line[7] == player || line[7] == player + 1 || line[7] == player + 2) && (line[8] == player || line[8] == player + 1 || line[8] == player + 2))) 
				kind[4]++;//冲4
			else if (line[1] == 0 && line[6] == 0 &&
				(((line[2] == player || line[2] == player + 1 || line[2] == player + 2) && line[3] == 0) ||
				((line[7] == player || line[7] == player + 1 || line[7] == player + 2) && line[8] == 0))) 
				kind[6]++;//跳活3
			else if ((line[1] == 0 && line[3] == 0 && (line[2] == player || line[2] == player + 1 || line[2] == player + 2)) ||
				(line[6] == 0 && line[8] == 0 && (line[7] == player || line[7] == player + 1 || line[7] == player + 2))) 
				kind[7]++;//一端有对方棋子的眠三（__黑__黑黑白）
			else if ((line[1] == 0 && line[6] == 0) &&
				((line[2] == player || line[2] == player + 1 || line[2] == player + 2) || (line[7] == player || line[7] == player + 1 || line[7] == player + 2))) 
				kind[7]++;//一端有对方棋子的眠三（白黑__黑黑__）
			else if ((line[1] == 0 && line[2] == 0 && (line[3] == player || line[3] == player + 1 || line[3] == player + 2)) ||
				(line[6] == 0 && line[7] == 0 && (line[8] == player || line[8] == player + 1 || line[8] == player + 2))) 
				kind[7]++;//眠3（黑__ __黑黑）
			else if ((line[1] == 0 && line[6] == 0 && line[7] == 0 && line[8] == 0) ||
				(line[6] == 0 && line[1] == 0 && line[2] == 0 && line[3] == 0) ||
				(line[1] == 0 && line[2] == 0 && line[6] == 0 && line[7] == 0)) 
				kind[8]++;//活2
			else if ((line[1] == 0 && line[2] == 0 && line[3] == 0) || (line[6] == 0 && line[7] == 0 && line[8] == 0)) 
				kind[10]++;//半死2
			else
				kind[11]++;
		}
		else if (len == 1) {
			if ((line[1] == 0 && (line[2] == player || line[2] == player + 1 || line[2] == player + 2) &&
				(line[3] == player || line[3] == player + 1 || line[3] == player + 2) && (line[4] == player || line[4] == player + 1 || line[4] == player + 2)) ||
				(line[6] == 0 && (line[7] == player || line[7] == player + 1 || line[7] == player + 2) &&
				(line[8] == player || line[8] == player + 1 || line[8] == player + 2) && (line[9] == player || line[9] == player + 1 || line[9] == player + 2)))
				kind[4]++;//冲4
			else if ((line[1] == 0 && line[6] == 0) &&
				(((line[2] == player || line[2] == player + 1 || line[2] == player + 2) && (line[3] == player || line[3] == player + 1) && line[4] == 0) ||
				((line[7] == player || line[7] == player + 1) && (line[8] == player || line[8] == player + 1) && line[9] == 0)))
				kind[6]++;//跳活3
			else if ((line[1] == 0 && line[6] == 0) &&
				(((line[2] == player || line[2] == player + 1) && (line[3] == player || line[3] == player + 1 || line[3] == player + 2)) ||
				((line[7] == player || line[7] == player + 1 || line[7] == player + 2) && (line[8] == player || line[8] == player + 1 || line[8] == player + 2))))
				kind[7]++;//眠3
			else if ((line[1] == 0 && (line[2] == player || line[2] == player + 1) && (line[3] == player || line[3] == player + 1) && line[4] == 0) ||
				(line[6] == 0 && (line[7] == player || line[7] == player + 1) && (line[8] == player || line[8] == player + 1) && line[9] == 0))
				kind[7]++;//眠3
			else if ((line[6] == 0 && line[7] == 0 && (line[8] == player || line[8] == player + 1) && (line[9] == player || line[9] == player + 1)) ||
				(line[1] == 0 && line[2] == 0 && (line[3] == player || line[3] == player + 1) && (line[4] == player || line[4] == player + 1)))
				kind[7]++;//眠3
			else if ((line[1] == 0 && (line[2] == player || line[2] == player + 1) && line[3] == 0 && (line[4] == player || line[4] == player + 1)) ||
				(line[6] == 0 && (line[7] == player || line[7] == player + 1) && line[8] == 0 && (line[9] == player || line[9] == player + 1)))
				kind[7]++;//眠3
			else if ((line[1] == 0 && line[6] == 0 && line[8] == 0 && (line[7] == player || line[7] == player + 1)) && (line[2] == 0 || line[9] == 0))
				kind[9]++;//跳活2
			else if ((line[1] == 0 && line[6] == 0 && line[3] == 0 && (line[2] == player || line[2] == player + 1)) && (line[4] == 0 || line[7] == 0))
				kind[9]++;//跳活2
			else if ((line[1] == 0 && line[6] == 0 && (line[8] == player || line[8] == player + 1) && line[7] == 0 && line[9] == 0) ||
				(line[1] == 0 && line[6] == 0 && line[2] == 0 && (line[3] == player || line[3] == player + 1) && line[4] == 0))
				kind[9]++;//跳活2
			else if ((line[6] == 0 && line[8] == 0 && (line[7] == player || line[7] == player + 1) && line[9] == 0) ||
				(line[1] == 0 && line[3] == 0 && (line[2] == player || line[2] == player + 1) && line[4] == 0))
				kind[10]++;//半死2
			else if ((line[1] == 0 && line[6] == 0 && (line[2] == player || line[2] == player + 1) && line[7] == 0) ||
				(line[1] == 0 && line[6] == 0 && line[2] == 0 && (line[7] == player || line[7] == player + 1)))
				kind[10]++;//半死2
			else if ((line[1] == 0 && line[2] == 0 && (line[3] == player || line[3] == player + 1) && line[4] == 0) ||
				(line[6] == 0 && line[7] == 0 && (line[8] == player || line[8] == player + 1) && line[9] == 0))
				kind[10]++;//半死2
			else if ((line[1] == 0 && line[6] == 0 && (line[3] == player || line[3] == player + 1) && line[2] == 0) ||
				(line[1] == 0 && line[6] == 0 && (line[8] == player || line[8] == player + 1) && line[7] == 0))
				kind[10]++;//半死2
			else if ((line[1] == 0 && line[2] == 0 && (line[4] == player || line[4] == player + 1) && line[3] == 0) ||
				(line[6] == 0 && line[7] == 0 && (line[9] == player || line[9] == player + 1) && line[8] == 0))
				kind[10]++;//半死2
			else
				kind[11]++;
		}
	}
	s = 0;
	if (scoretype == 1) {
		s = 7000 * (kind[1] + kind[0]) + 1601 * kind[2] + 800 * (kind[4] + kind[6]) + 900 * (kind[5] + kind[3]) + kind[8] * 11
			+ kind[9] * 10 + kind[7] * 9 + kind[10] * 8;
		if (player == 10) {//禁手
			if (kind[5] + kind[6] >= 2 || kind[3] + kind[4] + kind[2] >= 2 || kind[0] >= 1)
				s = -100;
		}
	}
	else if (scoretype == 2) {
		s = 1602 * (kind[1] + kind[0]) + 1601 * kind[2] + 800 * (kind[4] + kind[6]) + 900 * (kind[5] + kind[3]) + kind[8] * 11
			+ kind[9] * 10 + kind[7] * 9 + kind[10] * 8;
		if (player == 10) {//禁手
			if (kind[5] + kind[6] >= 2 || kind[3] + kind[4] + kind[2] >= 2 || kind[0] >= 1)
				s = -100;
		}
	}
	return s;
}


void computer()
{
	int a1 = -101, a2 = 0, b1 = -101, b2 = 0, a12 = 0, a22 = 0, b12 = 0, b22 = 0, a3 = 0, b3 = 0, a32 = 0, b32 = 0;
	int m1 = 0, m2 = 0, n1 = 0, n2 = 0;
	int y12 = 0, y22 = 0, x12 = 0, x22 = 0, x3, y3, x32, y32;
	int i, j, r = 0, r1 = 0, r2 = 0, r3 = 0, r4 = 0, r5 = 0, r6 = 0, r7;

	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m1 = score(i, j, my, 1);
				n1 = score(i, j, other, 1);

				if (m1 + n1 > a1 + b1) {//进攻
					a12 = a1;//次佳进攻
					b12 = b1;
					y12 = y1;
					x12 = x1;
					y1 = i;//最佳进攻
					x1 = j;
					a1 = m1;
					b1 = n1;
				}

				else if (m1 + n1 <= a1 + b1 && m1 + n1 > a12 + b12) {
					a12 = m1;//次佳进攻
					b12 = n1;
					y12 = i;
					x12 = j;
				}
			}
		}
	}
	//	printf("%c %d %d %d次佳进攻\n", x12 + 97, 15 - y12, a12, b12);
	//	printf("%c %d %d %d最佳进攻\n", x1 + 97, 15 - y1, a1, b1);
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m2 = score(i, j, my, 1);
				n2 = score(i, j, other, 1);
				if (n2 > b2) {//防守
					a22 = a2;//次佳防守
					b22 = b2;
					y22 = y2;
					x22 = x2;
					y2 = i;//最佳防守
					x2 = j;
					b2 = n2;
					a2 = m2;
				}
				else if (n2 <= b2 && n2 > b22) {
					b22 = n2;
					y22 = i;
					x22 = j;
					a22 = m2;
				}
			}
		}
	}
	//最佳进攻点
	r1 = Board[y1][x1];
	Board[y1][x1] = my;
	supother(2, 1);//假如我下在最佳进攻点，对方怎么下

	r2 = Board[supposey[2][2][1]][supposex[2][2][1]];
	//	printf("1对方:%c %d\n", 97 + supposex[2][2][1], 15 - supposey[2][2][1]);
	Board[supposey[2][2][1]][supposex[2][2][1]] = other;//假设对方进攻
	supmy(3, 1);//我如何下

	r3 = Board[supposey[1][3][1]][supposex[1][3][1]];//假设我选择最佳进攻点
//	printf("2我:%c %d\n", 97 + supposex[1][3][1], 15 - supposey[1][3][1]);
	Board[supposey[1][3][1]][supposex[1][3][1]] = my;
	supother(4, 1);//对方如何下

	r4 = Board[supposey[2][4][1]][supposex[2][4][1]];
	//	printf("3对方:%c %d\n", 97 + supposex[2][4][1], 15 - supposey[2][4][1]);
	Board[supposey[2][4][1]][supposex[2][4][1]] = other;
	supmy2(5, 1);//对局面评分
	supother2(5, 1);

	//	printf("最佳进攻点往前推后局面%d %d\n", supposemy[1][5][1], supposeother[2][5][1]);
		//将假设落子位置恢复
	Board[supposey[1][3][1]][supposex[1][3][1]] = r3;
	Board[y1][x1] = r1;
	Board[supposey[2][2][1]][supposex[2][2][1]] = r2;
	Board[supposey[2][4][1]][supposex[2][4][1]] = r4;

	//次佳进攻点
	r1 = Board[y12][x12];
	Board[y12][x12] = my;
	supother(2, 2);//假如我下在最佳进攻点，对方怎么下

	r2 = Board[supposey[2][2][2]][supposex[2][2][2]];
	Board[supposey[2][2][2]][supposex[2][2][2]] = other;//假设对方进攻
	supmy(3, 2);//我如何下

	r3 = Board[supposey[1][3][2]][supposex[1][3][2]];//假设我选择最佳进攻点
	Board[supposey[1][3][2]][supposex[1][3][2]] = my;
	supother(4, 2);//对方如何下

	r4 = Board[supposey[2][4][2]][supposex[2][4][2]];
	Board[supposey[2][4][2]][supposex[2][4][2]] = other;
	supmy2(5, 2);//对局面评分
	supother2(5, 2);

	//	printf("次佳进攻点往前推后局面%d %d\n", supposemy[1][5][2], supposeother[2][5][2]);

	Board[supposey[1][3][2]][supposex[1][3][2]] = r3;
	Board[y12][x12] = r1;
	Board[supposey[2][2][2]][supposex[2][2][2]] = r2;
	Board[supposey[2][4][2]][supposex[2][4][2]] = r4;

	//最佳防守点
	r1 = Board[y2][x2];
	Board[y2][x2] = my;
	//	printf("0我:%c %d\n", 97 + x2, 15 - y2);
	supother(2, 3);//假如我下在最佳进攻点，对方怎么下

	r2 = Board[supposey[2][2][3]][supposex[2][2][3]];
	//	printf("1对方:%c %d\n", 97 + supposex[2][2][3], 15 - supposey[2][2][3]);
	Board[supposey[2][2][3]][supposex[2][2][3]] = other;//假设对方进攻
	supmy(3, 3);//我如何下

	r3 = Board[supposey[1][3][3]][supposex[1][3][3]];//假设我选择最佳进攻点
//	printf("2我:%c %d\n", 97 + supposex[1][3][3], 15 - supposey[1][3][3]);
	Board[supposey[1][3][3]][supposex[1][3][3]] = my;
	supother(4, 3);//对方如何下

	r4 = Board[supposey[2][4][3]][supposex[2][4][3]];
	//	printf("3对方:%c %d\n", 97 + supposex[2][4][3], 15 - supposey[2][4][3]);
	Board[supposey[2][4][3]][supposex[2][4][3]] = other;
	supmy2(5, 3);//对局面评分
	supother2(5, 3);

	//	printf("最佳防守点往前推后局面%d %d\n", supposemy[1][5][3], supposeother[2][5][3]);

	Board[supposey[1][3][3]][supposex[1][3][3]] = r3;
	Board[y2][x2] = r1;
	Board[supposey[2][2][3]][supposex[2][2][3]] = r2;
	Board[supposey[2][4][3]][supposex[2][4][3]] = r4;
	//次佳防守点
	r1 = Board[y22][x22];
	Board[y22][x22] = my;
	//	printf("0我:%c %d\n", 97 + x22, 15 - y22);
	supother(2, 4);//假如我下在最佳进攻点，对方怎么下

	r2 = Board[supposey[2][2][4]][supposex[2][2][4]];
	//	printf("1对方:%c %d\n", 97 + supposex[2][2][4], 15 - supposey[2][2][4]);
	Board[supposey[2][2][4]][supposex[2][2][4]] = other;//假设对方进攻
	supmy(3, 4);//我如何下

	r3 = Board[supposey[1][3][4]][supposex[1][3][4]];//假设我选择最佳进攻点
//	printf("2我:%c %d\n", 97 + supposex[1][3][4], 15 - supposey[1][3][4]);
	Board[supposey[1][3][4]][supposex[1][3][4]] = my;
	supother(4, 4);//对方如何下

	r4 = Board[supposey[2][4][4]][supposex[2][4][4]];
	//	printf("3对方:%c %d\n", 97 + supposex[2][4][4], 15 - supposey[2][4][4]);
	Board[supposey[2][4][4]][supposex[2][4][4]] = other;
	supmy2(5, 4);//对局面评分
	supother2(5, 4);

	//	printf("次佳防守点往前推后局面%d %d\n", supposemy[1][5][4], supposeother[2][5][4]);

	Board[supposey[1][3][4]][supposex[1][3][4]] = r3;
	Board[y22][x22] = r1;
	Board[supposey[2][2][4]][supposex[2][2][4]] = r2;
	Board[supposey[2][4][4]][supposex[2][4][4]] = r4;

	if (my == 10 && count == 1 && Board[6][8] == 20) {//棋谱
		y1 = 8;
		x1 = 8;
		Board[8][8] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[6][6] == 20) {
		y1 = 6;
		x1 = 8;
		Board[6][8] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[8][6] == 20) {
		y1 = 6;
		x1 = 6;
		Board[6][6] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[8][8] == 20) {
		y1 = 8;
		x1 = 6;
		Board[8][6] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[7][8] == 20) {
		y1 = 8;
		x1 = 8;
		Board[8][8] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[6][7] == 20) {
		y1 = 6;
		x1 = 8;
		Board[6][8] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[7][6] == 20) {
		y1 = 6;
		x1 = 6;
		Board[6][6] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 1 && Board[8][7] == 20) {
		y1 = 8;
		x1 = 6;
		Board[8][6] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 3 && Board[6][8] == 20 && Board[8][8] == 20) {
		y1 = 9;
		x1 = 7;
		Board[9][7] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 3 && Board[6][8] == 20 && Board[6][6] == 20) {
		y1 = 7;
		x1 = 9;
		Board[7][9] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 3 && Board[6][6] == 20 && Board[8][6] == 20) {
		y1 = 5;
		x1 = 7;
		Board[5][7] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 10 && count == 3 && Board[8][6] == 20 && Board[8][8] == 20) {
		y1 = 7;
		x1 = 5;
		Board[7][5] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 20 && count == 6 && Board[7][5] == 10 && Board[7][6] == 20 && Board[8][6] == 10 && Board[9][5] == 20 && Board[9][7] == 10) {
		y1 = 6;
		x1 = 4;
		Board[6][4] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (my == 20 && count == 6 && Board[5][7] == 10 && Board[6][7] == 20 && Board[6][8] == 10 && Board[5][9] == 20 && Board[7][9] == 10) {
		y1 = 4;
		x1 = 6;
		Board[4][6] = mytem;
		//		printf("棋谱\n");
		printf("%c %d", x1 + 97, 15 - y1);
	}
	else if (a1 >= 7000 || b2 < 1600) {//进攻
		if (a1 >= 1600) {
			Board[y1][x1] = mytem;
			//			printf("进攻\n");
			printf("%c %d", x1 + 97, 15 - y1);
		}
		else {
			if (supposeother[2][5][2] >= 3200 || supposeother[2][5][1] >= 3200) {
				if (b1 >= 7000 || b2 >= 7000 || b2 - b22 >= 100) {//直接防守
					if (a2 < 0) {//如果最佳防守为禁手，选择次佳防守
						y1 = y22;
						x1 = x22;
						Board[y22][x22] = mytem;
						//						printf("防守22\n");
						printf("%c %d", x22 + 97, 15 - y22);
					}
					else {
						y1 = y2;
						x1 = x2;
						Board[y2][x2] = mytem;
						//						printf("防守2\n");
						printf("%c %d", x2 + 97, 15 - y2);
					}
				}
				else {//择优防守
					if ((supposemy[1][5][3] - supposeother[2][5][3]) < (supposemy[1][5][4] - supposeother[2][5][4])) {
						y1 = y22;
						x1 = x22;
						Board[y22][x22] = mytem;
						//						printf("次佳防守\n");
						printf("%c %d", x22 + 97, 15 - y22);
					}
					else {
						y1 = y2;
						x1 = x2;
						Board[y2][x2] = mytem;
						//						printf("最佳防守\n");
						printf("%c %d", x2 + 97, 15 - y2);
					}
				}
			}
			else if ((supposemy[1][5][1] - supposeother[1][5][1]) < (supposemy[1][5][2] - supposeother[1][5][2])) {
				y1 = y12;
				x1 = x12;
				Board[y12][x12] = mytem;
				//				printf("次佳进攻\n");
				printf("%c %d", x12 + 97, 15 - y12);
			}
			else {
				Board[y1][x1] = mytem;
				//				printf("进攻\n");
				printf("%c %d", x1 + 97, 15 - y1);
			}
		}
	}
	else {//防守
		if (b1 >= 7000 || b2 >= 7000 || b2 - b22 >= 100) {//直接防守
			if (a2 < 0) {//次佳防守
				y1 = y22;
				x1 = x22;
				Board[y22][x22] = mytem;
				//				printf("防守22\n");
				printf("%c %d", x22 + 97, 15 - y22);
			}
			else {
				y1 = y2;
				x1 = x2;
				Board[y2][x2] = mytem;
				//				printf("防守2\n");
				printf("%c %d", x2 + 97, 15 - y2);
			}
		}
		else {//择优防守
			if ((supposemy[1][5][3] - supposeother[2][5][3]) < (supposemy[1][5][4] - supposeother[2][5][4])) {
				y1 = y22;
				x1 = x22;
				Board[y22][x22] = mytem;
				//				printf("次佳防守\n");
				printf("%c %d", x22 + 97, 15 - y22);
			}
			else {
				y1 = y2;
				x1 = x2;
				Board[y2][x2] = mytem;
				//				printf("最佳防守\n");
				printf("%c %d", x2 + 97, 15 - y2);
			}
		}
	}
}



void luozi(int mode)
{
	c = a;
	d = b;
	if (mode == 2) {
		if (count % 2 == 1) {
			printf("请执黑方落子");
			Board[d][c] = WHITE;
			scanf(" %c ", &a);
			scanf("%d", &b);
			a = a - 97;
			b = 15 - b;
			if (a >= 0 && a <= 14 && b >= 0 && b <= 14)
				Board[b][a] = BLACKtem;
			else {
				printf("出界");
			}
		}
		else if (count % 2 == 0) {
			printf("请执白方落子");
			Board[d][c] = BLACK;
			scanf(" %c ", &a);
			scanf("%d", &b);
			a = a - 97;
			b = 15 - b;
			if (a >= 0 && a <= 14 && b >= 0 && b <= 14)
				Board[b][a] = WHITEtem;
			else
				printf("出界");
		}
	}
	else if (mode == 1) {//人机
		if (mytem == 11) {//机黑
			if (count % 2 == 1) {
				if (error != 1) {
					printf("请执黑方落子");
					Board[d][c] = WHITE;
					computer();
					a = x1;
					b = y1;
					system("pause");
				}
			}
			else if (count % 2 == 0) {
				printf("请执白方落子");
				Board[d][c] = BLACK;
				scanf(" %c ", &a);
				scanf("%d", &b);
				a = a - 97;
				b = 15 - b;
				if (Board[b][a] == 10 || Board[b][a] == 11 || Board[b][a] == 20 || Board[b][a] == 21) {
					printf("已落子");
					system("pause");
					error = 1;
				}
				else if (a >= 0 && a <= 14 && b >= 0 && b <= 14) {
					Board[b][a] = WHITEtem;
					error = 0;
				}
				else {
					printf("出界");
					system("pause");
					error = 1;
				}
			}
		}
		else if (mytem == 21) {//机白
			if (count % 2 == 0) {
				printf("请执白方落子");
				Board[d][c] = BLACK;
				computer();
				a = x1;
				b = y1;
				system("pause");
			}
			else if (count % 2 == 1) {
				printf("请执黑方落子");
				Board[d][c] = WHITE;
				scanf(" %c ", &a);
				scanf("%d", &b);
				a = a - 97;
				b = 15 - b;
				if (a >= 0 && a <= 14 && b >= 0 && b <= 14) {
					Board[b][a] = BLACKtem;
					forbid1 = score(b, a, other, 1);
					if (forbid1 < 0) {
						printf("禁手\n");
						printf("白方胜\n");
						system("pause");
					}
				}
				else
					printf("出界");
			}
		}
	}
}

void caipan()
{
	int i, j;
	if (count == 224)
		printf("平局");
	else if (count % 2 == 0) {//对黑子进行判断
		if (Board[b][a] == BLACKtem) {
			for (j = -4; j <= 1; j++) {
				for (i = j; Board[b + i][a] == BLACK; i++) {
					if (i == j + 3) {
						printf("黑方胜\n");
						system("pause");
						break;
					}
				}
				for (i = j; Board[b][a + i] == BLACK && a + i >= 0 && a + i <= 14; i++) {
					if (i == j + 3) {
						printf("黑方胜\n");
						system("pause");
						break;
					}
				}
				for (i = j; Board[b + i][a + i] == BLACK; i++) {
					if (i == j + 3) {
						printf("黑方胜\n");
						system("pause");
						break;
					}
				}
				for (i = j; Board[b - i][a + i] == BLACK; i++) {
					if (i == j + 3) {
						printf("黑方胜\n");
						system("pause");
						break;
					}
				}
			}
		}
	}
	else if (count % 2 == 1) {//对白子进行判断
		if (Board[b][a] == WHITEtem) {
			for (j = -4; j <= 1; j++) {
				for (i = j; Board[b + i][a] == WHITE; i++) {
					if (i == j + 3) {
						printf("白方胜\n");
						system("pause");
					}
				}
				for (i = j; Board[b][a + i] == WHITE && a + i <= 14 && a + i >= 0; i++) {
					if (i == j + 3) {
						printf("白方胜\n");
						system("pause");
					}
				}
				for (i = j; Board[b + i][a + i] == WHITE; i++) {
					if (i == j + 3) {
						printf("白方胜\n");
						system("pause");
					}
				}
				for (i = j; Board[b - i][a + i] == WHITE; i++) {
					if (i == j + 3) {
						printf("白方胜\n");
						system("pause");
					}
				}
			}
		}
	}
}

void DisplayBoard(int mode)
{
	int i, j;
	char line;
	char ary;
	if (mode == 1)
		printf("\t   人机对战\n");
	else
		printf("\t   人人对战\n");
	for (j = 0, line = 15; j <= SIZE - 1; j++)
	{
		printf("%2d", line);
		line -= 1;
		for (i = 0; i <= SIZE - 1; i++)
		{
			switch (Board[j][i])
			{
			case 1:
				printf("┏-");
				break;

			case 2:
				printf("┓");
				break;

			case 3:
				printf("┛");
				break;

			case 4:
				printf("┗-");
				break;

			case 5:
				printf("┠-");
				break;

			case 6:
				printf("┯-");
				break;

			case 7:
				printf("┨");
				break;

			case 8:
				printf("┷-");
				break;

			case 9:
				printf("┼-");
				break;

			case BLACKtem:      // 黑子上一步
				printf("▲");
				break;

			case WHITEtem:      //白子上一步
				printf("△");
				break;

			case BLACK:      //黑子通常
				printf("●");
				break;

			case WHITE:
				printf("○");  //白子通常
				break;
			}
			if (i == SIZE - 1)
			{
				printf("\n");
			}

		}
	}

	printf("   ");
	for (ary = 'A'; ary < 'A' + SIZE; ary++)
		printf("%c ", ary);

	printf("\n");
}




void ClearBoardArray()
{
	int j, i;

	Board[0][0] = 1;
	Board[0][SIZE - 1] = 2;
	Board[SIZE - 1][SIZE - 1] = 3;
	Board[SIZE - 1][0] = 4;

	for (j = 1; j <= SIZE - 2; j++)
	{
		Board[j][0] = 5;
	}

	for (i = 1; i <= SIZE - 2; i++)
	{
		Board[0][i] = 6;
	}

	for (j = 1; j <= SIZE - 2; j++)
	{
		Board[j][SIZE - 1] = 7;
	}

	for (i = 1; i <= SIZE - 2; i++)
	{
		Board[SIZE - 1][i] = 8;
	}

	for (j = 1; j <= SIZE - 2; j++)
	{
		for (i = 1; i <= SIZE - 2; i++)
		{
			Board[j][i] = 9;
		}
	}
}

void renji()
{
	int a;
	printf("请选择人先或机先，人先输入1，机先输入2");
	scanf("%d", &a);
	if (a == 1) {
		mytem = 21;//机白
		my = 20;
		other = 10;
	}
	else if (a == 2) {
		mytem = 11;//机黑
		my = 10;
		other = 20;
	}
}

void supmy(int y, int z)
{
	int aa1 = 0, aa2 = 0, bb1 = 0, bb2 = 0, aa12 = 0, aa22 = 0, bb12 = 0, bb22 = 0;
	int m1 = 0, m2 = 0, n1 = 0, n2 = 0;
	int yy12 = 0, yy22 = 0, xx12 = 0, xx22 = 0, yy1 = 0, xx1 = 0, yy2 = 0, xx2 = 0;
	int i = 0, j = 0, r = 0;

	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m1 = score(i, j, my, 1);
				n1 = score(i, j, other, 2);
				if (m1 + n1 > aa1 + bb1) {//进攻
					aa12 = aa1;
					bb12 = bb1;
					yy12 = yy1;
					xx12 = xx1;
					yy1 = i;
					xx1 = j;
					aa1 = m1;
					bb1 = n1;
				}
				else if (m1 + n1 == aa1 + bb1) {
					if (m1 > aa1) {
						aa12 = aa1;
						bb12 = bb1;
						yy12 = yy1;
						xx12 = xx1;
						yy1 = i;
						xx1 = j;
						aa1 = m1;
						bb1 = n1;
					}
					else if (m1 <= aa1) {
						aa12 = m1;
						bb12 = n1;
						yy12 = i;
						xx12 = j;
					}
				}
				else if (m1 + n1<aa1 + bb1 && m1 + n1>aa12 + bb12) {
					aa12 = m1;
					bb12 = n1;
					yy12 = i;
					xx12 = j;
				}
			}
		}
	}
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m2 = score(i, j, my, 1);
				n2 = score(i, j, other, 1);
				if (n2 > bb2) {//防守
					aa22 = aa2;
					bb22 = bb2;
					yy22 = yy2;
					xx22 = xx2;
					yy2 = i;
					xx2 = j;
					bb2 = n2;
					aa2 = m2;
				}
				else if (n2 <= bb2 && n2 > bb22) {
					bb22 = n2;
					yy22 = i;
					xx22 = j;
					aa22 = m2;
				}
			}
		}
	}
	if (aa1 >= 7000 || bb2 < 1600) {
		supposex[1][y][z] = xx1;
		supposey[1][y][z] = yy1;
	}
	else {
		supposex[1][y][z] = xx2;
		supposey[1][y][z] = yy2;
	}
	supposemy[1][y][z] = aa1 + aa12;
}

void supother(int y, int z)
{
	int aa1 = 0, aa2 = 0, bb1 = 0, bb2 = 0, aa12 = 0, aa22 = 0, bb12 = 0, bb22 = 0;
	int m1 = 0, m2 = 0, n1 = 0, n2 = 0;
	int yy12 = 0, yy22 = 0, xx12 = 0, xx22 = 0, yy1 = 0, xx1 = 0, yy2 = 0, xx2 = 0;
	int i = 0, j = 0, r = 0;
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m1 = score(i, j, my, 1);
				n1 = score(i, j, other, 1);
				if (n1 > bb1) {//进攻
					aa12 = aa1;
					bb12 = bb1;
					yy12 = yy1;
					xx12 = xx1;
					yy1 = i;
					xx1 = j;
					aa1 = m1;
					bb1 = n1;
				}
				else if (n1 = bb1) {
					aa12 = m1;
					bb12 = n1;
					yy12 = i;
					xx12 = j;
				}
			}
		}
	}
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m2 = score(i, j, my, 1);
				n2 = score(i, j, other, 1);
				if (m2 > aa2) {//防守
					aa22 = aa2;
					bb22 = bb2;
					yy22 = yy2;
					xx22 = xx2;
					yy2 = i;
					xx2 = j;
					bb2 = n2;
					aa2 = m2;
				}
				else if (m2 <= aa2 && m2 > aa22) {
					bb22 = n2;
					yy22 = i;
					xx22 = j;
					aa22 = m2;
				}
			}
		}
	}
	if (bb1 >= 1600 || aa2 < 1600) {
		supposex[2][y][z] = xx1;
		supposey[2][y][z] = yy1;
	}
	else {
		supposex[2][y][z] = xx2;
		supposey[2][y][z] = yy2;
	}
	supposeother[2][y][z] = bb1 + bb12;
}


void supmy2(int y, int z)
{
	int aa1 = 0, aa2 = 0, bb1 = 0, bb2 = 0, aa12 = 0, aa22 = 0, bb12 = 0, bb22 = 0;
	int m1 = 0, m2 = 0, n1 = 0, n2 = 0;
	int yy12 = 0, yy22 = 0, xx12 = 0, xx22 = 0, yy1 = 0, xx1 = 0, yy2 = 0, xx2 = 0;
	int i = 0, j = 0, r = 0;
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m1 = score(i, j, my, 2);
				n1 = score(i, j, other, 2);
				if (m1 + n1 > aa1 + bb1) {//进攻
					aa12 = aa1;
					bb12 = bb1;
					yy12 = yy1;
					xx12 = xx1;
					yy1 = i;
					xx1 = j;
					aa1 = m1;
					bb1 = n1;
				}
				else if (m1 + n1 == aa1 + bb1) {
					if (m1 > aa1) {
						aa12 = aa1;
						bb12 = bb1;
						yy12 = yy1;
						xx12 = xx1;
						yy1 = i;
						xx1 = j;
						aa1 = m1;
						bb1 = n1;
					}
					else if (m1 <= aa1) {
						aa12 = m1;
						bb12 = n1;
						yy12 = i;
						xx12 = j;
					}
				}
				else if (m1 + n1<aa1 + bb1 && m1 + n1>aa12 + bb12) {
					aa12 = m1;
					bb12 = n1;
					yy12 = i;
					xx12 = j;
				}
			}
		}
	}
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m2 = score(i, j, my, 2);
				n2 = score(i, j, other, 2);
				if (n2 > bb2) {//防守
					aa22 = aa2;
					bb22 = bb2;
					yy22 = yy2;
					xx22 = xx2;
					yy2 = i;
					xx2 = j;
					bb2 = n2;
					aa2 = m2;
				}
				else if (n2 <= bb2 && n2 > bb22) {
					bb22 = n2;
					yy22 = i;
					xx22 = j;
					aa22 = m2;
				}
			}
		}
	}
	if (aa1 >= 7000 || bb2 < 1600) {
		supposex[1][y][z] = xx1;
		supposey[1][y][z] = yy1;
	}
	else {
		supposex[1][y][z] = xx2;
		supposey[1][y][z] = yy2;
	}
	supposemy[1][y][z] = aa1 + aa12;
}

void supother2(int y, int z)
{
	int aa1 = 0, aa2 = 0, bb1 = 0, bb2 = 0, aa12 = 0, aa22 = 0, bb12 = 0, bb22 = 0;
	int m1 = 0, m2 = 0, n1 = 0, n2 = 0;
	int yy12 = 0, yy22 = 0, xx12 = 0, xx22 = 0, yy1 = 0, xx1 = 0, yy2 = 0, xx2 = 0;
	int i = 0, j = 0, r = 0;
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m1 = score(i, j, my, 2);
				n1 = score(i, j, other, 2);
				if (m1 + n1 > aa1 + bb1) {//进攻
					aa12 = aa1;
					bb12 = bb1;
					yy12 = yy1;
					xx12 = xx1;
					yy1 = i;
					xx1 = j;
					aa1 = m1;
					bb1 = n1;
				}
				else if (m1 + n1 == aa1 + bb1) {
					if (n1 > bb1) {
						aa12 = aa1;
						bb12 = bb1;
						yy12 = yy1;
						xx12 = xx1;
						yy1 = i;
						xx1 = j;
						aa1 = m1;
						bb1 = n1;
					}
					else if (n1 <= bb1) {
						aa12 = m1;
						bb12 = n1;
						yy12 = i;
						xx12 = j;
					}
				}
				else if (m1 + n1<aa1 + bb1 && m1 + n1>aa12 + bb12) {
					aa12 = m1;
					bb12 = n1;
					yy12 = i;
					xx12 = j;
				}
			}
		}
	}
	for (j = 0; j <= 14; j++) {
		for (i = 0; i <= 14; i++) {
			if (Board[i][j] != 10 && Board[i][j] != 11 && Board[i][j] != 20 && Board[i][j] != 21) {
				m2 = score(i, j, my, 2);
				n2 = score(i, j, other, 2);
				if (m2 > aa2) {//防守
					aa22 = aa2;
					bb22 = bb2;
					yy22 = yy2;
					xx22 = xx2;
					yy2 = i;
					xx2 = j;
					bb2 = n2;
					aa2 = m2;
				}
				else if (m2 <= aa2 && m2 > aa22) {
					bb22 = n2;
					yy22 = i;
					xx22 = j;
					aa22 = m2;
				}
			}
		}
	}
	if (bb1 >= 7000 || aa2 < 1600) {
		supposex[2][y][z] = xx1;
		supposey[2][y][z] = yy1;
	}
	else {
		supposex[2][y][z] = xx2;
		supposey[2][y][z] = yy2;
	}
	supposeother[2][y][z] = bb1 + bb12;
}