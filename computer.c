
/************************************************************************
 * コンピューターの思考ルーチン                                         *
 * 機能 - othello.c とのインターフェイスは get_pos_comp を通じて行う.   *
 * 上記部分以外を除いてothello.cとの繋がりはない.                       *
 * 思考ルーチン自体はothelloプログラムから独立し, 疎結合を実現している. *
 ************************************************************************/

#include "othello.h"

/* 評価関数の値 */
int cmp_eval_board[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 120, -40, 20, 5, 5, 20, -40, 120, 0,
  0, -40, -60, -5, -5, -5, -5, -60, -40, 0,
  0, 20, -5, 15, 3, 3, 15, -5, 20, 0,
  0, 5, -5, 3, 3, 3, 3, -5, 5, 0,
  0, 5, -5, 3, 3, 3, 3, -5, 5, 0,
  0, 20, -5, 15, 3, 3, 15, -5, 20, 0,
  0, -40, -60, -5, -5, -5, -5, -60, -40, 0,
  0, 120, -40, 20, 5, 5, 20, -40, 120, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* 思考回数 */
static int counter = 0;

/* 盤面のモード */
enum eval_mode {
  BOARD,
  STONE
};
typedef enum eval_mode eval_mode_t;
/* 盤面全体の評価モード */
eval_mode_t mode = BOARD;

/* 
 * 盤面等の状況を表す構造体.
 * この構造体を通してコンピューターの思考ルーチンを作る
 */
typedef struct {
  state_t state;                /* 現在の局面状況 */
  int avilable;                 /* 置ける駒の数 */
  int is_pos[30];               /* 置ける石の位置 */
  int pos;                      /* コンピューターが置く石の位置 */
  int eval;                     /* 評価値 */
} cmp_t;

/* state 以外を初期化 */
void board_clear(cmp_t *cmp) {
  int i;
  cmp->avilable = 0;
  cmp->pos = 0;
  for (i = 0; i < 30; i++) {
    cmp->is_pos[i] = 0;
  }
}

/* 初期化 */
cmp_t cmp_init(state_t state) {
  cmp_t cmp;
  cmp.state = state;
  board_clear(&cmp);
  counter = 0;
  return cmp;
}


/* 石の数で比較する場合 */
void eval_update_stone(cmp_t* cmp, int pos) {
  int i, j, eval;
  for (i = 1; i <= 8; i++) {
    for (j = 1; j <= 8; j++) {
      int pos = 10 * i + j;
      if (USER == cmp->state.board[pos]) eval++;
    }
  }
  cmp->eval = eval;
}

/* 評価関数を更新 */
void eval_board_update(int points, int pos1, int pos2, int pos3) {
  cmp_eval_board[pos1] = points;
  cmp_eval_board[pos2] = points;
  cmp_eval_board[pos3] = points;
}

/* 四隅の評価値を更新する */
void eval_update_square(pos) {  
  switch(pos) {
  case 11:
    eval_board_update(40, 12, 22, 21);
    break;
  case 18:
    eval_board_update(40, 17, 27, 28);
    break;
  case 81:
    eval_board_update(40, 71, 72, 82);
    break;
  case 88:
    eval_board_update(40, 87, 77, 78);
    break;
  default:
    break;
  }
}

/* 評価値で比較する場合 */
void eval_update_board(cmp_t* cmp, int pos) {
  switch (cmp->state.board[pos]) {
  case USER:
    cmp->eval += cmp_eval_board[pos];
    eval_update_square(pos);
    break;
  case YOU:
    cmp->eval -= cmp_eval_board[pos];
    break;
  default:
    // NONE
    break;
  }
  
}

/* 盤面の評価値を更新 */
void eval_update(cmp_t *cmp, int pos) {
  switch(mode) {
  case BOARD:                   /* 評価値を利用するモード */
    eval_update_board(cmp, pos);
    break;
  case STONE:                   /* 単純に石の数を比較するモード */
    eval_update_stone(cmp, pos);
    break;
  } 
}

/* 盤面の評価値を初期化 */
void eval_init(cmp_t *cmp) {
  int i, j, pos;
  cmp->eval = 0;
  for (i = 1; i <= 8; i++) {
    for (j = 1; j <= 8; j++) {
      pos = i * 10 + j;
      eval_update(cmp, pos);
    }
  }
}

void dump_pos(int pos) {  
    printf("pos = (%d, %d)\n", pos / 10, pos % 10);
    printf("pos = %d\n", pos);
}

/* pos に石を置いた場合の局面 */
cmp_t* cmp_set_pos(cmp_t* cmp, int pos) {
  cmp->state.pos = pos;
  if (check_pos(&cmp->state)) {
    process_reverse(&cmp->state);
    eval_update(cmp, pos);
  }
  else {
    /* ありえないことが起こった */
    show_board(&cmp->state);
    dump_pos(pos);
    assert(false);
  }
  return cmp;
}

/* 駒をposに置き, 次の曲面を返す */
cmp_t* next_turn(cmp_t* cmp, int pos) {
  cmp_t* next = cmp_set_pos(cmp, pos);
  board_clear(next);
  return next;
}

/* 昇順にソート */
int compare_int(const void *a, const void *b) {
  return *(int *)b - *(int *)a;
}

/* arr_eval配列を出力debug用 */
void arr_show(int *arr_eval, int avilable) {
  int i;
  for (i = 0; i < avilable; i++) {
    printf("arr_eval[%d] = %d\n", i, arr_eval[i]);
  }
}

/* 可能な着手をよさそうな順にソート */
void sort(cmp_t cmp, int *is_pos) {
  int arr_eval[cmp.avilable];
  int i;
  ENTRY item, *found_item;
  char name_to_find[cmp.avilable];
  hcreate(cmp.avilable);
  /* 一手先読みを行い, arr_evalに先読みした場合の評価値を入れる */
  for (i = 0; i < cmp.avilable; i++) {
    char line[MAXLINE];
    cmp_t* next = next_turn(&cmp, cmp.is_pos[i]);
    arr_eval[i] = next->eval;
    while (1) {
      sprintf(line, "%d", arr_eval[i]);
      item.key = strdup(line);
      if (hsearch(item, FIND) != NULL) {
        arr_eval[i] += 1;
        item.key = strdup(line);
      }
      else break;
    }
    sprintf(line, "%d", is_pos[i]);
    item.data = strdup(line);
    //printf("key = %s, data = %s\n", (char *)item.key, (char *)item.data);
    hsearch(item, ENTER);
  }

  /* arr_evalを昇順に並び替え */
  qsort(arr_eval, cmp.avilable, sizeof(int), compare_int);

  item.key = name_to_find;
  for (i = 0; i < cmp.avilable; i++) {
    sprintf(item.key, "%d", arr_eval[i]);
    if ((found_item = hsearch(item, FIND)) != NULL) {
      is_pos[i] = atoi(found_item->data);
    }
    else {
      assert(false);
    }
  }
  /* 
   * arr_show(arr_eval, cmp.avilable);
   * arr_show(is_pos, cmp.avilable);
   * assert(false);
   */
  hdestroy();
}

/* 指せる手筋を全てピックアップする */
void set_avilable_pos(cmp_t *cmp) {
  int i, j;
  for (i = 1; i <= 8; i++) {
    for (j = 1; j <= 8; j++) {
      state_t state;
      state = cmp->state;
      state.pos = i * 10 + j;
      if (check_pos(&state)) {
        cmp->is_pos[cmp->avilable] = state.pos;
        (cmp->avilable)++;
        if (cmp->avilable > 30) {
          assert(false);
        }
      }
    }
  }
}

/* min, max 法でdepth手先読みを行う */
int min_max_pos(cmp_t *cmp, int depth, int alpha, int beta) {
  int max, i;
  if (0 == depth) {
    int eval;
    eval = cmp->eval;
#if 0
    show_board(&cmp->state);
    printf("eval = %d\n\n", eval);
#endif
    return eval;
  }
  max = alpha;
  
  set_avilable_pos(cmp);
  
  /* cmp->is_posをよさそうな順にソート */
  sort(*cmp, cmp->is_pos);
  if (cmp->avilable == 0) {
    return -(beta + 1);
  }
  for (i = 0; i < cmp->avilable; i++) {
    int x;
    int pos = cmp->is_pos[i];
    cmp_t current = *cmp;
    cmp_t* next = next_turn(cmp, pos); /* pos に石を置いた次の局面 */
    x = -min_max_pos(next, depth - 1, -beta, -max);
    *next = current;
    if (x >= beta) return x;
    if (x > max) {
      max = x;
      cmp->pos = pos;
      printf("\rcounter = %d", ++counter);
    }
  }
  //printf("cmp->pos = %d\n", cmp->pos);
  return max;
}

/* 読み切り用の配列 */
int end_pos[5];

/* 読み切りモード */
int solove(cmp_t *cmp, int depth) {
  int i;
  set_avilable_pos(cmp);
  /* 最終局面まで駒が埋まった場合の評価値を計算する */
  if (depth == 0) {
    return cmp->eval;
  }
  int max = -9999;
  for (i = 0; i < cmp->avilable; i++) {
    int pos = cmp->is_pos[i];
    cmp_t current = *cmp;
    cmp_t *next = next_turn(cmp, pos);
    int eval = -search_end(next, depth - 1);
    *next = current;
    if (eval > max) {
      max = eval;
      cmp->pos = pos;
      printf("\rcounter = %d", ++counter);
    }
  }
  return max;
}

/* ポジションをチェックする */
void error_check(cmp_t* cmp) {
  int* pos = &cmp->pos;
  if (*pos == 0) {
    if (cmp->avilable > 0) {
      srand(time(NULL));
      *pos = cmp->is_pos[(rand() % cmp->avilable)];
      printf("\npos = %d\n", *pos);
    }
    else {
      printf("置けるとこない...  (´Д｀)ﾊｧ… \nパス\n");
      *pos = -1;
    }
  }
  printf("\n\n");
}

/* 石を碁盤のどこに置くか決定する */
int turn_cmp(state_t *state, int level) {
  cmp_t cmp;
  int pos;
  int beta = 9999;
  int alpha = -9999;

  /* 現在の局面を cmp_t 構造体にコピーする */
  cmp = cmp_init(*state);
  eval_init(&cmp);

  /* 手筋を決定 */
  if (level == 0) {
    /* ランダムに石を置く */
    set_avilable_pos(&cmp);    
    pos = cmp.is_pos[rand() % cmp.avilable];
    return pos;
  }

  /* 14手以下の場合は読み切りモード */  
  int depth = (64 - curr_result.number);
  printf("curr_result.number = %d\n", curr_result.number);
  if (depth <= 10) {
    /* 読み切りモードに変更 */
    mode = STONE;
    solove(&cmp, depth);
    printf("\npos: %d\n", cmp.pos);
  }
  else {
    /* level 分先読みしてから石を置く */
    min_max_pos(&cmp, level, alpha, beta);
  }  
  
  error_check(&cmp);
  return cmp.pos;
}
