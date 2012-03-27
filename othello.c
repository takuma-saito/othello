
/********************************
 * othello - オセロのプログラム *
 ********************************/

#include "othello.h"

/* 自石を起点(0) とした場合の各マス目の相対位置 */
int square[] = {
  -11,                          /* 左上 */
  -10,                          /* 真上 */
  -9,                           /* 右上 */
  -1,                           /* 右 */
  1,                            /* 左 */
  9,                            /* 左下 */
  10,                           /* 真下 */
  11,                           /* 右下 */
};
int square_length = sizeof(square) / sizeof(square[0]);

double get_time(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

stone_type_t get_oppose_user(stone_type_t turn) {
  stone_type_t oppose;
  switch(turn) {
  case YOU:
    oppose = USER;
    break;
  case USER:
    oppose = YOU;
    break;
  default:
    assert(false);
    break;
  }
  return oppose;
}

/* 現在の盤面を更新 */
void update_state(state_t *state) {
  state->turn = get_oppose_user(state->turn);
  state->pos = 0;
  state->rv_size = 0;
  state->st_type = st_wait;
}

/* 自分の手筋を入力する */
void set_pos_you(state_t *state) {
  int i, j;
  char line[MAXLINE];
  const char *prompt = "you > ";
  while (1) {
    printf("%s", prompt);
    fgets(line, MAXLINE, stdin);
    line[strlen(line) - 1] = '\0'; /* 改行を削除 */
    if (strcmp("quit", line) == 0 ||
        strcmp("q", line) == 0 ||
        strcmp("exit", line) == 0 ||
        strcmp("e", line) == 0) exit(0);
    if (strcmp("pass", line) == 0) {
      state->pos = -1;
      break;
    }
    if (strcmp("", line) == 0) {
      printf("配置を入力して下さい.\n");
      continue;
    }
    
    sscanf(line, "%d %d", &i, &j);
    if ((i >= 1 && i <= 8) || (j >= 1 || j <= 8)) {
      state->pos = i * 10 + j;
      break;
    }
    else {
      printf("%d, %d には置けません。", i, j);
    }
  }
}

/* 手筋を得る (仮想ユーザー) */
int get_pos_user(state_t *state) {
  char line[MAXLINE];
  int i, j;
  const char *prompt = "user > ";
  while (1) {
    printf("%s", prompt);
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d %d", &i, &j);
    if ((i >= 1 && i <= 8) || (j >= 1 || j <= 8)) {
      break;
    }
    else {
      printf("%d, %d には置けません。", i, j);
    }
  }
  return i * 10 + j;
}

/* コンピューターが手筋を入力する */
void set_pos_user(state_t *state) {
  int pos;
  switch(statis.battle) {
  case BATTLE_USER:
    pos = get_pos_user(state);
    break;
  case BATTLE_COMP:
    pos = turn_cmp(state, statis.level);
    break;
  default:
    assert(false);
    break;
  }
  state->pos = pos;
}

/* 手を得る */
void set_pos(state_t *state) {  
  switch(state->turn) {
  case YOU:
    set_pos_you(state);
    break;
  case USER:
    set_pos_user(state);
    break;
  default:
    assert(false);
    break;
  }
}

/* 現在の盤面を初期化 */
state_t init_state(void) {
  state_t state;
  int i, j;
  state.st_type = st_start;
  state.pos = 0;
  state.prev_pos = 0;
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      state.board[10 * i + j] = NONE;
    }
  }
  /* 壁を作る */
  for (i = 0; i <= 9; i++) {
    state.board[i] = WALL;
    state.board[10 * i] = WALL;
    state.board[10 * i + 9] = WALL;
    state.board[10 * 9 + i] = WALL;
  }
  return state;
}

/* 初期配置を設定する */
void set_init_board(stone_type_t *board) {  
  /* 初期配置 */
  if (statis.init == 0) {
    board[4 * 10 + 4] = YOU;
    board[5 * 10 + 5] = YOU;
    board[4 * 10 + 5] = USER;
    board[5 * 10 + 4] = USER;
  }
  else if (statis.init == 1) {
    board[4 * 10 + 4] = USER;
    board[5 * 10 + 5] = USER;
    board[4 * 10 + 5] = YOU;
    board[5 * 10 + 4] = YOU;
  }
  else {
    assert(false);
  }
}

/* 対戦相手を決める */
int get_user(void) {
  char line[MAXLINE];
  int battle;
  while (1) {
    printf("対戦相手を決定して下さい [user or comp]: ");
    fgets(line, MAXLINE, stdin);
    line[strlen(line) - 1] = '\0';
    if (strcmp("user", line) == 0) {
      battle = BATTLE_USER;
      break;
    }
    else if (strcmp("comp", line) == 0) {
      battle = BATTLE_COMP;
      break;
    }
    else {
      printf("user, comp のどちかを入力して下さい。\n");
    }
  }
  return battle;
}

/* 先攻, 後攻を決める */
stone_type_t get_turn(void) {
  char line[MAXLINE];
  int number;
  stone_type_t turn;
  while (1) {
    printf("先攻, 後攻を入力して下さい [先攻 ... 0, 後攻　... 1]： ");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &number);
    if (number == 0) {
      turn = YOU;
      break;
    }
    else if (number == 1) {
      turn = USER;
      break;
    }
    else {
      printf("0 か 1 で入力して下さい。\n");
    }
  }
  return turn;
}

/* コンピューターのレベル */
int get_level(void) {
  char line[MAXLINE];
  int level;
  while (1) {
    printf("レベルを入力して下さい [1 ~ 8]： ");
    fgets(line, MAXLINE, stdin);    
    sscanf(line, "%d", &level);
    if (level > 0 && level <= 8) {
      level -= 1;
      level *= 2;
      break;
    }
    else {
      printf("1 ~ 8 で入力して下さい。\n");
    }
  }
  return level;
}

/* 初期盤面を決める */
int get_init(void) {
  char line[MAXLINE];
  int init;
  while (1) {
    printf("盤面の初期値 [左上 |  ●... 0,  ○... 1]： ");
    fgets(line, MAXLINE, stdin);    
    sscanf(line, "%d", &init);
    if (init >= 0 && init <= 1) {
      break;
    }
    else {
      printf("1 ~ 10 で入力して下さい。\n");
    }
  }
  return init;
}

/* ゲームスタート */
void process_start(state_t *state) {
  /**************************
   * デバックモード時の条件 *
   * 先行：コンピューター   *
   * レベル：6              *
   * 石の色：○             *
   **************************/
#if DEBUG
  state->turn = USER;
  statis.battle = BATTLE_COMP;
  statis.time = get_time();
  statis.init = 0;
  set_init_board(state->board);
  statis.level = 2 * 2;         /* 4手先読み */
  state->st_type = st_wait;
  return;
#endif
  
  /* 先攻, 後攻を決定する */
  state->turn = get_turn();

  /* 対戦相手を決定する */
  statis.battle = get_user();

  /* 開始時間を測定 */
  statis.time = get_time();

  /* 初期値 */
  statis.init = get_init();
  set_init_board(state->board);

  /* COMP 選択時, レベルを決定する */
  if (statis.battle == BATTLE_COMP) {
    statis.level = get_level();
  }
  
  state->st_type = st_wait;
}

/* 現在の成績の初期化 */
void result_init(void) {
  curr_result.you = 0;
  curr_result.user = 0;
  curr_result.number = 0;
}

/* 現在の成績を表示 */
void result_show(void) {
  printf("  you... ● %d    user... ○ %d     残り局面: %d\n\n",
         curr_result.you, curr_result.user, (64 - curr_result.number));
}

/* 現在の盤面を出力する, resultの更新 */
void show_board(state_t *state) {
  int i, j, pos;
  int prev_pos = state->prev_pos;
  result_init();
  printf("        1  2  3  4  5  6  7  8\n");
  printf("      --------------------------\n");
  for (i = 1; i <= 8; i++) {
    printf("  %d  | ", i);
    for (j = 1; j <= 8; j++) {
      pos = 10 * i + j;
      printf(" ");
      if (state->board[pos] == YOU) {
        curr_result.you++;
        curr_result.number++;
        if (prev_pos == pos) {
          printf("★ ");
          continue;
        }
        printf("● ");
      }
      else if (state->board[pos] == USER) {
        curr_result.user++;
        curr_result.number++;
        if (prev_pos == pos) {
          printf("☆ ");
          continue;
        }
        printf("○ ");
      }
      else if (state->board[pos] == NONE) {
        printf(". ");
      }
    }
    printf(" |\n");
  }
  printf("      --------------------------\n");
  result_show();
}

/* リバース変数を初期化 */
void init_rv(state_t *state) {
  int i;
  state->rv_size = 0;
  for (i = 0; i < 30; i++) {
    state->rv_pos[i] = 0;
  }
}

/* 深さ優先探索 */
bool depth_search(int pos, state_t *state) {
  int curr_pos = state->pos;
  stone_type_t oppose_user = get_oppose_user(state->turn);
  bool find_flg = false;
  while (1) {
    curr_pos += pos;
    /* 自分の石があった場合 */
    if (state->board[curr_pos] == state->turn) {
      find_flg = true;
      break;
    }
    /* 相手の石があった場合 */
    else if (state->board[curr_pos] == oppose_user) {
      state->rv_pos[state->rv_size] = curr_pos;
      ++(state->rv_size);
      if (state->rv_size > 30) {
        assert(false);
      }
    }
    /* 壁にぶつかったか, 何も石がなかった場合 */
    else {
      /* stackに積んだ石を下ろす */
      while ((curr_pos -= pos) != state->pos) {
        --state->rv_size;
        state->rv_pos[state->rv_size] = 0;
      }
      find_flg = false;
      break;
    }
  }
  return find_flg;
}

/* pos の位置に置けるかどうか手筋をチェックする */
bool check_pos(state_t *state) {
  int loop;
  bool find_flg = false, flg;
  stone_type_t oppose_user = get_oppose_user(state->turn);

  /* パスの場合 */
  if (state->pos == -1) {
    return true;
  }

  /* 置く場所がNONEでない場合はfalse */
  if (state->board[state->pos] != NONE) {
    return false;
  }

  /* リバース変数を初期化 */
  init_rv(state);
  for (loop = 0; loop < square_length; loop++) {
    /* 自石の周りに自分と反対の石があった */
    if (state->board[state->pos + square[loop]] == oppose_user) {
      flg = depth_search(square[loop], state);
      if (flg) find_flg = true;
    }
  }
  return find_flg;
}

/* 手を考える */
void process_wait(state_t *state) {
  int check;
  while (1) {
    show_board(state);
    set_pos(state);
    check = check_pos(state);
    if (check == true) {
      /* posの位置に置きstateを更新する */
      state->st_type = st_reverse;
      break;
    }
    else {
      /* もう一度やり直し */
      printf("(%d, %d) には置けません。もう一度入力して下さい。\n",
             state->pos / 10,
             state->pos % 10);
    }
  }
}

/* 実際にリバーシを行う, この時点でstate構造体に相手の指した目の情報が全て含まれている */
void process_reverse(state_t *state) {
  int i;
  
  /* パスの場合 */
  if (state->pos == 0) {
    state->prev_pos = 0;
    update_state(state);
    return;
  }
  
  /* リーバイスを実行, 盤面を更新 */
  state->prev_pos = state->pos;
  state->board[state->pos] = state->turn;
  for (i = 0; i < state->rv_size; i++) {
    state->board[state->rv_pos[i]] = state->turn;
  }
  /* stateを初期化 */
  update_state(state);
}

/* 結果を表示 */
void show_fin_result(state_t *state) {
  printf("勝負終わり\n");
}

/* 勝負終わり */
void process_end(state_t *state) {
  show_fin_result(state);
}

/* mainルーチン */
int main(void) {
  state_t state;

  /* 盤面情報を初期化 */
  state = init_state();
  
  while (1) {
    switch(state.st_type) {
    case st_start:
      process_start(&state);
      break;
    case st_end:
      process_end(&state);
      break;
    case st_wait:
      process_wait(&state);
      break;
    case st_reverse:
      process_reverse(&state);
      break;
    default:
      break;
    }
  }
}
