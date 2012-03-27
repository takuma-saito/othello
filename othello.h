#ifndef __OTHELLO_H
#define __OTHELLO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <search.h>
#include <sys/time.h>

#define alloc(type) (type *)malloc(sizeof(type))
#define MAXLINE 1024
#define STACK_SIZE 5
#define BATTLE_USER 0
#define BATTLE_COMP 1

/* ユーザータイプ */
typedef enum stone_type_tag {
  YOU, USER, NONE, WALL,
} stone_type_t;

/* 状態遷移 */
typedef enum state_type_tag {
  st_start, st_end, st_wait, st_reverse,
} state_type;

/*****************************************************************
 * boardの位置はposで表す                                        *
 * 0  1  2  3  4  5  6  7  8  9                                  *
 * 10 11 12 13 14 15 16 17 18 19                                 *
 * 20 21 22 23 24 25 26 27 28 29                                 *
 * 30 31 32 33 34 35 36 37 38 39                                 *
 * 40 41 42 43 44 45 46 47 48 49                                 *
 * 50 51 52 53 54 55 56 57 58 59                                 *
 * 60 61 62 63 64 65 66 67 68 69                                 *
 * 70 71 72 73 74 75 76 77 78 79                                 *
 * 80 81 82 83 84 85 86 87 88 89                                 *
 * 90 91 92 93 94 95 96 97 98 99                                 *
 *                                                               *
 * 各辺を壁とみなすので、実際に指すのは 縦：11 ~ 88 の間である   *
 *****************************************************************/

/* 盤面の状態 */
typedef struct state_tag {
  stone_type_t board[10 * 10];  /* ボードの状況 */
  stone_type_t turn;      /* 現在の指して（どちらのターンかを表す） */
  int pos;                                    /* 置いた手筋, -1の時はパス */
  int prev_pos;                               /* 前回置かれた手筋 */
  state_type st_type;                         /* 現在の状況 */
  int rv_size;                                /* リバースできる石の数 */
  int rv_pos[30];                  /* リバースできる石の位置情報 */
} state_t;

/* 現在の成績 */
typedef struct {
  int you;                      /* you の現在のコマ数 */
  int user;                     /* user の現在のコマ数 */
  int number;                   /* 現在の局面数 */
} curr_result_t;
curr_result_t curr_result;

/* 統計情報 */
typedef struct statis_tag {
  int battle;                   /* 対戦相手 */
  int time;                     /* 開始時間 */
  int level;                    /* コンピューターのレベル */
  int init;                     /* 初期配置 */
} statis_t;
statis_t statis;

/* 深さ優先探索 */
extern bool
depth_search(int pos, state_t *state);

/* pos の位置に置けるか手筋をチェックする */
extern bool
check_pos(state_t *state);

/* コンピューターの手筋 */
extern int
turn_cmp(state_t *state, int level);

/* リバーシを実行する */
extern void
process_reverse(state_t *state);

/* 局面を表示する */
extern void
show_board(state_t *state) ;

#endif // __OTHELLO_H
