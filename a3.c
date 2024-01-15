#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WRONG_ARGUMENT_COUNT 1
#define CANNOT_OPEN_FILE 2
#define INVALID_FILE 3
#define MEMORY_ALLOCATION_ERROR 4

const int CONFIG_CARDS_LINE_START = 3;
const int CONFIG_CARDS_LINE_END = 23;
const int MAX_CARD_ROWS = 3;


enum _Color_ {
  RED = 'r',
  GREEN = 'g',
  BLUE = 'b',
  WHITE = 'w',
};
typedef enum _Color_ Color;

struct _Card_ {
  Color color_;
  int value_;
  struct _Card_ *next_;
};
typedef struct _Card_ Card;


int checkMainArgumentsCount(int argc);
int getPlayersCount(char *config_file);
void printWelcomeMessage(int players_count);
void printCardChoosingPhase();
void printActionPhase();

// File functions
FILE *openFile(char *config_file);
int checkConfigFile(char *config_file);

// Card functions
Card *createCard(char *config_file_line);
int assignCardsToPlayers(Card **player_one_handcards, Card **player_two_handcards, char *config_file);
Card *getCardFromHand(Card *player_handcards, int card_number);
Card *getCardFromChosen(Card *player_chosencards, int card_number);
int exchangePlayerCards(Card **player_one_handcards, Card **player_two_handcards);
int sortCards(Card **player_cards);

// Player functions
void printPlayer(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows);
int addCardToHand(Card **player_handcards, Card *card);
int addCardToChosen(Card **player_chosencards, Card *card);
int removeCardFromHand(Card **player_handcards, Card *card);
int removeCardFromChosen(Card **player_chosencards, Card *card);
int addCardToRow(Card **player_cardrows, Card *card, int row_number);


// Ask user input
int chooseCardToKeep(int player_id, Card **player_handcards, Card **player_chosencards);
int cardChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                      Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows);
int actionChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                        Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows);
int isActionInputCorrect(char *row_number, char *card_number);
int actionChoosingLoop(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows);

void freePlayer(Card *player_handcards, Card *player_chosencards, Card **player_cardrows);


int main(int argc, char *argv[]) {
  // Check if correct number of arguments
  if (checkMainArgumentsCount(argc) != 0) {
    return WRONG_ARGUMENT_COUNT;
  }
  // Check if config file is valid
  int config_file_error = checkConfigFile(argv[1]);
  if (config_file_error != 0) {
    return config_file_error;
  }
  int players_count = getPlayersCount(argv[1]);
  printWelcomeMessage(players_count);
  // Create the players
  Card *player_one_handcards = NULL;
  Card *player_one_chosencards = NULL;
  Card **player_one_cardrows = malloc(sizeof(Card) * MAX_CARD_ROWS);
  if (player_one_cardrows == NULL) {
    return MEMORY_ALLOCATION_ERROR;
  }
  Card *player_two_handcards = NULL;
  Card *player_two_chosencards = NULL;
  Card **player_two_cardrows = malloc(sizeof(Card) * MAX_CARD_ROWS);
  if (player_two_cardrows == NULL) {
    return MEMORY_ALLOCATION_ERROR;
  }
  // Create an array of cards with the size of 3 for each player
  for (int i = 0; i < MAX_CARD_ROWS; i++) {
    player_one_cardrows[i] = NULL;
    player_two_cardrows[i] = NULL;
  }

  // Assign cards to players
  if (assignCardsToPlayers(&player_one_handcards, &player_two_handcards, argv[1]) != 0) {
    return 1;
  }
  // Sort the cards in the hand cards of each player
  sortCards(&player_one_handcards);
  sortCards(&player_two_handcards);
  // Start the game
  do {
    printCardChoosingPhase();
    if (cardChoosingPhase(&player_one_handcards, &player_one_chosencards, &player_one_cardrows,
                          &player_two_handcards, &player_two_chosencards, &player_two_cardrows) == 1) {
      break;
    }
    exchangePlayerCards(&player_one_handcards, &player_two_handcards);
    printActionPhase();
    if (actionChoosingPhase(&player_one_handcards, &player_one_chosencards, &player_one_cardrows,
                            &player_two_handcards, &player_two_chosencards, &player_two_cardrows) == 1) {
      break;
    }
    // If the player one has no handcards and no chosencards, or the player two has no handcards and no chosencards,
    // the game is over

  } while ((player_one_handcards != NULL || player_one_chosencards != NULL) &&
           (player_two_handcards != NULL || player_two_chosencards != NULL));
  freePlayer(player_one_handcards, player_one_chosencards, player_one_cardrows);
  freePlayer(player_two_handcards, player_two_chosencards, player_two_cardrows);
  return 0;
}

int checkMainArgumentsCount(int argc) {
  if (argc != 2) {
    printf("Usage: ./a3 <config file>\n");
    return 1;
  }
  return 0;
}

FILE *openFile(char *config_file) {
  FILE *file = fopen(config_file, "r");
  if (file == NULL) {
    printf("Error: Cannot open file: %s\n", config_file);
    return NULL;
  }
  return file;
}

// Read a single line from the config file at a specific line number and return a string
char *readLine(FILE *file, int line_number) {
  // Go to the beginning of the file
  char *line = NULL;
  size_t len = 0;
  fseek(file, 0, SEEK_SET);
  // Read the file line by line until the line number is reached
  for (int i = 0; i < line_number; i++) {
    getline(&line, &len, file);
  }
  return line;

}

int checkConfigFile(char *config_file) {
  // Check if program can open file
  FILE *file = openFile(config_file);
  if (file == NULL) {
    return CANNOT_OPEN_FILE;
  }
  char *magic_number = "ESP\n";
  char *line = NULL;
  size_t len = 0;
  size_t read;
  read = getline(&line, &len, file);
  if (read == -1) { // TODO: warning: comparison of integers of different signs: 'size_t' (aka 'unsigned long') and 'int' [-Wsign-compare]
    printf("Error: Invalid file: %s\n", config_file);
    return INVALID_FILE;
  }
  if (strcmp(line, magic_number) != 0) {
    printf("Error: Invalid file: %s\n", config_file);
    return INVALID_FILE;
  }

  return 0;
}

int getPlayersCount(char *config_file) {
  // The number of players is defined in the second line of the file
  FILE *file = openFile(config_file);
  char *line = readLine(file, 2);
  int players_count = atoi(line);
  return players_count;
}

void printWelcomeMessage(int players_count) {
  printf("Welcome to SyntaxSakura (%i players are playing)!\n", players_count);
  printf("\n");
}


// Create a card from a line in the config file
Card *createCard(char *config_file_line) {
  Card *card = malloc(sizeof(Card));
  if (card == NULL) {
    printf("Error: Memory allocation error\n");
    return NULL;
  }
  // We get a line that looks like this: 1_g\n and the char is the color and the int is the value
  // So we need to split the string we get
  int value = atoi(strtok(config_file_line, "_"));
  char *color = strtok(NULL, "_");
  color[strcspn(color, "\n")] = 0;

  if (strcmp(color, "r") == 0) {
    card->color_ = RED;
  } else if (strcmp(color, "g") == 0) {
    card->color_ = GREEN;
  } else if (strcmp(color, "b") == 0) {
    card->color_ = BLUE;
  } else if (strcmp(color, "w") == 0) {
    card->color_ = WHITE;
  } else {
    printf("Error: Invalid color: %s\n", color);
    return NULL;
  }
  card->value_ = value;
  card->next_ = NULL;
  return card;
}

int assignCardsToPlayers(Card **player_one_handcards, Card **player_two_handcards, char *config_file) {
  // We start reading the config file from line 3 and assign each card in an alternating fashion to each player
  // The first card is assigned to player one, the second card to player two, the third card to player one, etc.
  // Each player gets exactly 10 cards
  FILE *file = openFile(config_file);
  for (int i = CONFIG_CARDS_LINE_START; i < CONFIG_CARDS_LINE_END; i++) {
    char *line = readLine(file, i);
    Card *card = createCard(line);
    if (card == NULL) {
      return 1;
    }
    if (i % 2 != 0) {
      // Assign card to player one
      addCardToHand(player_one_handcards, card);
    } else {
      addCardToHand(player_two_handcards, card);
    }
  }
  return 0;
}

int exchangePlayerCards(Card **player_one_handcards, Card **player_two_handcards) {
  Card *temp_hand_cards = *player_one_handcards;
  *player_one_handcards = *player_two_handcards;
  *player_two_handcards = temp_hand_cards;
  return 0;
}

void printCardChoosingPhase() {
  printf("-------------------\n");
  printf("CARD CHOOSING PHASE\n");
  printf("-------------------\n");
  printf("\n");
}

void printActionPhase() {
  printf("------------\n");
  printf("ACTION PHASE\n");
  printf("------------\n");
  printf("\n");
}

int cardChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                      Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows) {
  printPlayer(1, player_one_handcards, player_one_chosencards, player_one_cardrows);

  printf("Please choose a first card to keep:\n");
  if (chooseCardToKeep(1, player_one_handcards, player_one_chosencards) == 1) {
    return 1;
  }

  printf("Please choose a second card to keep:\n");
  if (chooseCardToKeep(1, player_one_handcards, player_one_chosencards) == 1) {
    return 1;
  }
  printf("\n");

  printPlayer(2, player_two_handcards, player_two_chosencards, player_two_cardrows);
  printf("Please choose a first card to keep:\n");
  if (chooseCardToKeep(2, player_two_handcards, player_two_chosencards) == 1) {
    return 1;
  }
  printf("Please choose a second card to keep:\n");
  if (chooseCardToKeep(2, player_two_handcards, player_two_chosencards) == 1) {
    return 1;
  }

  printf("\n");
  printf("Card choosing phase is over - passing remaining hand cards to the next player!\n");
  printf("\n");
  return 0;
}


void printPlayer(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows) {
  printf("Player %i:\n", player_id);
  printf("  hand cards:");
  Card *head = *player_handcards;
  if (head != NULL && head->color_ != '\0') {
    printf(" ");
  }
  while (head != NULL && head->color_ != '\0') {
    if (head->next_ != NULL) {
      printf("%i_%c ", head->value_, head->color_);
    } else {
      printf("%i_%c\n", head->value_, head->color_);
    }
    head = head->next_;
  }

  printf("  chosen cards:");
  head = *player_chosencards;
  if (head != NULL && head->color_ != '\0') {
    printf(" ");
  } else {
    printf("\n");
  }
  while (head != NULL && head->color_ != '\0') {
    if (head->next_ != NULL) {
      printf("%i_%c ", head->value_, head->color_);
    } else {
      printf("%i_%c\n", head->value_, head->color_);
    }
    head = head->next_;
  }

  if (player_cardrows != NULL && *player_cardrows != NULL) {
    for (int i = 0; i < MAX_CARD_ROWS; i++) {
      if ((*player_cardrows)[i] != NULL && (*player_cardrows)[i]->color_ != '\0') {
        printf("  row_%i: ", i+1);
        head = (*player_cardrows)[i];
        while (head != NULL) {
          if (head->next_ != NULL) {
            printf("%i_%c ", head->value_, head->color_);
          } else {
            printf("%i_%c\n", head->value_, head->color_);
          }
          head = head->next_;
        }
      }
    }
  }
  printf("\n");
}


Card *getCardFromHand(Card *player_handcards, int card_number) {
  Card *head = player_handcards;
  while (head != NULL) {
    if (head->value_ == card_number) {
      return head;
    }
    head = head->next_;
  }
  return NULL;
}

Card *getCardFromChosen(Card *player_chosencards, int card_number) {
  Card *head = player_chosencards;
  while (head != NULL) {
    if (head->value_ == card_number) {
      return head;
    }
    head = head->next_;
  }
  return NULL;
}

int chooseCardToKeep(int player_id, Card **player_handcards, Card **player_chosencards) {
  Card *chosen_card;
  do {
    chosen_card = NULL;
    int card_number;
    printf("P%i > ", player_id);
    // Read user input. Can be either a string saying quit or a number
    // Input must be stored on the heap
    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);
    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "quit") == 0) {
      return 1;
    } else if (strncmp(input, "quit", 4) == 0 && strlen(input) > 4) {
      printf("Please enter the correct number of parameters!\n");
      continue;
    } else if (atoi(input) == 0) {
      printf("Please enter the number of a card in your hand cards!\n");
      continue;
    }

    card_number = atoi(input);
    chosen_card = getCardFromHand(*player_handcards, card_number);
    if (chosen_card == NULL) {
      printf("Please enter the number of a card in your hand cards!\n");
    } else {
      break;
    }
    // Free the input from the heap
    free(input);
  } while (true);
  removeCardFromHand(player_handcards, chosen_card);
  addCardToChosen(player_chosencards, chosen_card);
  return 0;
}

int addCardToChosen(Card **player_chosencard, Card *card) {
  if (*player_chosencard == NULL || (*player_chosencard)->color_ == '\0') {
    *player_chosencard = card;
    (*player_chosencard)->next_ = NULL;
    return 0;
  }

  Card *head = *player_chosencard;
  Card *prev = NULL;

  while (head != NULL && card->value_ > head->value_) {
    prev = head;
    head = head->next_;
  }

  if (prev == NULL) {
    // Insert at the beginning of the list
    card->next_ = *player_chosencard;
    *player_chosencard = card;
  } else {
    // Insert in the middle or at the end of the list
    prev->next_ = card;
    card->next_ = head;
  }

  return 0;
}


int removeCardFromChosen(Card **player_chosencards, Card *card) {
  if (*player_chosencards == NULL) {
    return 1;
  }
  if ((*player_chosencards)->value_ == card->value_) {
    *player_chosencards = card->next_;
    card->next_ = NULL;
    return 0;
  }
  Card *head = *player_chosencards;
  while (head->next_ != NULL && head->next_ != card) {
    head = head->next_;
  }
  if (head->next_ != NULL) {
    head->next_ = card->next_;
    card->next_ = NULL;
    return 0;
  }
  return 1;
}

int addCardToHand(Card **player_handcards, Card *card) {
  Card *head = *player_handcards;
  if (head == NULL || head->color_ == '\0') {
    *player_handcards = card;
    (*player_handcards)->next_ = NULL;
    return 0;
  }
  while (head != NULL) {
    if (head->next_ == NULL) {
      head->next_ = card;
      card->next_ = NULL;
      return 0;
    }
    head = head->next_;
  }
  return 1;
}

int removeCardFromHand(Card **player_handcards, Card *card) {
  if (*player_handcards == NULL) {
    return 1;
  } else if ((*player_handcards)->value_ == card->value_) {
    *player_handcards = card->next_;
    card->next_ = NULL;
    return 0;
  }
  Card *head = *player_handcards;
  while (head->next_ != NULL && head->next_ != card) {
    head = head->next_;
  }
  if (head->next_ != NULL) {
    head->next_ = card->next_;
    card->next_ = NULL;
    return 0;
  }
  return 1;
}

int addCardToRow(Card **player_cardrows, Card *card, int row_number) {
  card->next_ = NULL;

  Card *current = player_cardrows[row_number];
  Card *prev = NULL;

  // If the list is empty, add the card as the first element
  if (current == NULL) {
    player_cardrows[row_number] = card;
    return 0;
  }

  // If the card should be inserted at the beginning of the list
  if (card->value_ < current->value_) {
    card->next_ = current;
    player_cardrows[row_number] = card;
    return 0;
  }

  // Traverse the list to find the appropriate position for the card
  while (current != NULL && card->value_ > current->value_) {
    prev = current;
    current = current->next_;
  }

  // If the card should be inserted at the end of the list
  if (current == NULL) {
    prev->next_ = card;
    return 0;
  }

  // If the card should be inserted between two cards, return 1 without adding it
  return 1;
}




void swap(Card **a, Card **b) {
  Color temp_color = (*a)->color_;
  int temp_value = (*a)->value_;

  (*a)->color_ = (*b)->color_;
  (*a)->value_ = (*b)->value_;

  (*b)->color_ = temp_color;
  (*b)->value_ = temp_value;
}

int sortCards(Card **player_cards) {
  if (*player_cards == NULL || (*player_cards)->next_ == NULL) {
    // No need to sort an empty list or a list with a single element
    return 1;
  }

  int swapped;
  Card *ptr1;
  Card *lptr = NULL;

  do {
    swapped = 0;
    ptr1 = *player_cards;

    while (ptr1->next_ != lptr) {
      if (ptr1->value_ > ptr1->next_->value_) {
        swap(&ptr1, &(ptr1->next_));
        swapped = 1;
      }
      ptr1 = ptr1->next_;
    }
    lptr = ptr1;
  } while (swapped);
  return 0;
}

int actionChoosingPhase(Card **player_one_handcards, Card **player_one_chosencards, Card ***player_one_cardrows,
                        Card **player_two_handcards, Card **player_two_chosencards, Card ***player_two_cardrows) {
  printPlayer(1, player_one_handcards, player_one_chosencards, player_one_cardrows);

  if (actionChoosingLoop(1, player_one_handcards,
                         player_one_chosencards, player_one_cardrows) == 1)
  {
    return 1;
  }
  printf("\n");

  printPlayer(2, player_two_handcards, player_two_chosencards, player_two_cardrows);
  if (actionChoosingLoop(2, player_two_handcards,
                         player_two_chosencards, player_two_cardrows) == 1)
  {
    return 1;
  }

  printf("\n");
  printf("Action phase is over - starting next game round!\n");
  printf("\n");

  return 0;
}

int isActionInputCorrect(char *row_number, char *card_number) {
  if ((row_number == NULL || card_number == NULL) || strtok(NULL, " ") != NULL) {
    printf("Please enter the correct number of parameters!\n");
    return false;
  }
  else if (atoi(row_number) > 3 || atoi(row_number) < 1) {
    printf("Please enter a valid row number!\n");
    return false;
  } else {
    return true;
  }
}

int actionChoosingLoop(int player_id, Card **player_handcards, Card **player_chosencards, Card ***player_cardrows) {
  bool skip_prompt = false;
  do {
    if (!skip_prompt) {
      printf("What do you want to do?\n");
    }
    printf("P%i > ", player_id);
    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);
    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "quit") == 0) {
      free(input);
      return 1;
    } else if (strncmp(input, "place", 5) == 0 && strlen(input) > 5) {
      char *row_number = strtok(input + 5, " ");
      char *card_number = strtok(NULL, " ");
      if (isActionInputCorrect(row_number, card_number)) {
        Card *choosen_card = getCardFromChosen(*player_chosencards, atoi(card_number));
        if (choosen_card == NULL) {
          printf("Please enter the number of a card in your chosen cards!\n");
          skip_prompt = true;
          free(input);
          continue;
        } else {
          removeCardFromChosen(player_chosencards, choosen_card);
          int resu = addCardToRow(*player_cardrows, choosen_card, atoi(row_number) - 1);
          if (resu == 1) {
            printf("This card cannot extend the chosen row!\n");
            skip_prompt = true;
            addCardToChosen(player_chosencards, choosen_card);
            free(input);
            continue;
          }
          printf("\n");
          printPlayer(player_id, player_handcards, player_chosencards, player_cardrows);
        }
      } else {
        skip_prompt = true;
        free(input);
        continue;
      }
    }
    else if (strncmp(input, "discard", 7) == 0 && strlen(input) > 7) {
      char *card_number = strtok(input + 7, " ");
      if (card_number == NULL || strtok(NULL, " ") != NULL) {
        printf("Please enter the correct number of parameters!\n");
        skip_prompt = true;
        free(input);
        continue;
      }
      Card *choosen_card = getCardFromChosen(*player_chosencards, atoi(card_number));
      if (choosen_card == NULL) {
        printf("Please enter the number of a card in your chosen cards!\n");
        skip_prompt = true;
        free(input);
        continue;
      } else {
        removeCardFromChosen(player_chosencards, choosen_card);
        printf("\n");
        printPlayer(player_id, player_handcards, player_chosencards, player_cardrows);
      }
  } else {
      printf("Please enter the correct number of parameters!\n");
      free(input);
      continue;
    }
    skip_prompt = false;
    free(input);
  } while (*player_chosencards != NULL);
  return 0;
}

void freePlayer(Card *player_handcards, Card *player_chosencards, Card **player_cardrows) {
  Card *head = player_handcards;
  while (head != NULL) {
    Card *temp = head;
    head = head->next_;
    free(temp);
  }
  head = player_chosencards;
  while (head != NULL) {
    Card *temp = head;
    head = head->next_;
    free(temp);
  }
  for (int i = 0; i < MAX_CARD_ROWS; i++) {
    head = player_cardrows[i];
    while (head != NULL) {
      Card *temp = head;
      head = head->next_;
      free(temp);
    }
  }
  free(player_cardrows);
}