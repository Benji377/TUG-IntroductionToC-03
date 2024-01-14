#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WRONG_ARGUMENT_COUNT 1
#define CANNOT_OPEN_FILE 2
#define INVALID_FILE 3
#define MEMORY_ALLOCATION_ERROR 4
#define MAX_INPUT_LENGTH 100

const int MAX_CARDS_HAND = 10;
const int MAX_CARD_ROWS = 3;

struct _Card_ {
  char color_;
  int value_;
  struct _Card_ *next_;
};
typedef struct _Card_ Card;

struct _Player_ {
  int player_id_;
  Card hand_cards_; // Only contains the first card of the linked card list
  Card table_cards_;
  Card *card_rows_; // An array of linked lists
};
typedef struct _Player_ Player;


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
void assignCardsToPlayers(Player *player_one, Player *player_two, char *config_file);
Card *getCardFromHand(Player *player, int card_number);
Card *getCardFromTable(Player *player, int card_number);
int exchangePlayerCards(Player *player_one, Player *player_two);
int sortCards(Card *cards);

// Player functions
Player *createPlayer();
void printPlayer(Player *player);
int addCardToHand(Player *player, Card *card);
int addCardToTable(Player *player, Card *card);
int removeCardFromHand(Player *player, Card *card);
int removeCardFromTable(Player *player, Card *card);
int addCardToRow(Player *player, Card *card, int row_number);


// Ask user input
int chooseCardToKeep(Player *player);
int cardChoosingPhase(Player *player_one, Player *player_two);
int actionChoosingPhase(Player *player_one, Player *player_two);

// Free memory allocation
void freePlayer(Player *player);


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
  Player *player_one = createPlayer();
  player_one->player_id_ = 1;
  if (player_one == NULL) {
    return MEMORY_ALLOCATION_ERROR;
  }
  Player *player_two = createPlayer();
  player_two->player_id_ = 2;
  if (player_two == NULL) {
    //freePlayer(player_one);
    return MEMORY_ALLOCATION_ERROR;
  }
  // Assign cards to players
  assignCardsToPlayers(player_one, player_two, argv[1]);
  // Sort the cards in the hand cards of each player
  sortCards(&player_one->hand_cards_);
  sortCards(&player_two->hand_cards_);
  // Start the game
  do {
    printCardChoosingPhase();
    if (cardChoosingPhase(player_one, player_two) == 1) {
      break;
    }
    exchangePlayerCards(player_one, player_two);
    printActionPhase();
    if (actionChoosingPhase(player_one, player_two) == 1) {
      break;
    }
    // TODO: Continue


  } while (1);
  //freePlayer(player_one);
  //freePlayer(player_two);
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

  // Check if file contains the correct magic number
  // The first line should be ESP\n in ASCII text
  char *magic_number = "ESP\n";
  char *line = NULL;
  size_t len = 0;
  size_t read;
  read = getline(&line, &len, file);
  if (read == -1) {
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

Player *createPlayer() {
  Player *player = malloc(sizeof(Player));
  if (player == NULL) {
    printf("Error: Memory allocation error\n");
    return NULL;
  }
  player->card_rows_ = malloc(sizeof(Card) * MAX_CARDS_HAND);
  if (player->card_rows_ == NULL) {
    printf("Error: Out of memory\n");
    //freePlayer(player);
    return NULL;
  }
  return player;
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

  card->color_ = *color;
  card->value_ = value;
  card->next_ = NULL;
  return card;
}

void assignCardsToPlayers(Player *player_one, Player *player_two, char *config_file) {
  // We start reading the config file from line 3 and assign each card in an alternating fashion to each player
  // The first card is assigned to player one, the second card to player two, the third card to player one, etc.
  // Each player gets exactly 10 cards
  FILE *file = openFile(config_file);
  for (int i = 3; i < 23; i++) {
    char *line = readLine(file, i);
    Card *card = createCard(line);
    if (i % 2 != 0) {
      // Assign card to player one
      addCardToHand(player_one, card);
    } else {
      addCardToHand(player_two, card);
    }
  }
}

int exchangePlayerCards(Player *player_one, Player *player_two) {
  Card temp_hand_cards = player_one->hand_cards_;
  player_one->hand_cards_ = player_two->hand_cards_;
  player_two->hand_cards_ = temp_hand_cards;
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

int cardChoosingPhase(Player *player_one, Player *player_two) {
  printPlayer(player_one);
  printf("Please choose a first card to keep:\n");
  if (chooseCardToKeep(player_one) == 1) {
    return 1;
  }
  printf("Please choose a second card to keep:\n");
  if (chooseCardToKeep(player_one) == 1) {
    return 1;
  }
  printf("\n");
  printPlayer(player_two);
  printf("Please choose a first card to keep:\n");
  if (chooseCardToKeep(player_two) == 1) {
    return 1;
  }
  printf("Please choose a second card to keep:\n");
  if (chooseCardToKeep(player_two) == 1) {
    return 1;
  }
  sortCards(&player_one->table_cards_);
  sortCards(&player_two->table_cards_);
  sortCards(&player_one->table_cards_);
  sortCards(&player_two->table_cards_);
  printf("\n");
  printf("Card choosing phase is over - passing remaining hand cards to the next player!\n");
  printf("\n");
  return 0;
}

int actionChoosingPhase(Player *player_one, Player *player_two) {
  printPlayer(player_one);
  printf("What do you want to do?\n");
  printf("P%i> ", player_one->player_id_);
  char input[MAX_INPUT_LENGTH];
  fgets(input, sizeof(input), stdin);

  // Remove trailing newline character
  if (input[strlen(input) - 1] == '\n') {
    input[strlen(input) - 1] = '\0';
  }
  printf("You entered: %s\n", input);
  // Tokenize the input
  char* token = strtok(input, " ");

  if (token == NULL) {
    // Empty command
    printf("Error: Empty command\n");
    return 1;
  }

  if (strcmp(token, "help") == 0) {
    // Process help command
    printf("Help command executed\n");
  } else if (strcmp(token, "place") == 0) {
    // Process place command
    // Extract ROW and CARD_NUMBER
    int row, cardNumber;
    token = strtok(NULL, " ");
    if (token == NULL || sscanf(token, "%d", &row) != 1) {
      printf("Error: Invalid ROW parameter\n");
      return 1;
    }
    token = strtok(NULL, " ");
    if (token == NULL || sscanf(token, "%d", &cardNumber) != 1) {
      printf("Error: Invalid CARD_NUMBER parameter\n");
      return 1;
    }

    printf("Place command executed with ROW=%d and CARD_NUMBER=%d\n", row, cardNumber);
  } else if (strcmp(token, "discard") == 0) {
    // Process discard command
    // Extract CARD_NUMBER
    int cardNumber;
    token = strtok(NULL, " ");
    if (token == NULL || sscanf(token, "%d", &cardNumber) != 1) {
      printf("Error: Invalid CARD_NUMBER parameter\n");
      return 1;
    }

    printf("Discard command executed with CARD_NUMBER=%d\n", cardNumber);
  } else if (strcmp(token, "quit") == 0) {
    // Process quit command
    printf("Quit command executed\n");
    // You can add cleanup code or exit the program here
  } else {
    // Unrecognized command
    printf("Error: Unrecognized command\n");
    return 1;
  }
  return 0;
}

void printPlayer(Player *player) {
  printf("Player %i:\n", player->player_id_);
  printf("  hand cards:");
  Card *head = &player->hand_cards_;
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
  head = &player->table_cards_;
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
  if (player->card_rows_ != NULL) {
    for (int i = 0; i < MAX_CARD_ROWS; i++) {
      if (player->card_rows_[i].color_ != '\0') {
        printf("  row_%i: ", i);
        head = &player->card_rows_[i];
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
    printf("\n");
  }
  printf("\n");
}

Card *getCardFromHand(Player *player, int card_number) {
  Card *head = &player->hand_cards_;
  while (head != NULL) {
    if (head->value_ == card_number) {
      return head;
    }
    head = head->next_;
  }
  return NULL;
}

Card *getCardFromTable(Player *player, int card_number) {
  Card *head = &player->table_cards_;
  while (head != NULL) {
    if (head->value_ == card_number) {
      return head;
    }
    head = head->next_;
  }
  return NULL;
}

int chooseCardToKeep(Player *player) {
  Card *chosen_card;
  int error_occurred = 1;
  do {
    int card_number;
    printf("P%i> ", player->player_id_);
    // Read user input. Can be either a string saying quit or a number
    char input[10];
    scanf("%s", input);
    if (strcmp(input, "quit") == 0) {
      return 1;
    }
    card_number = atoi(input);
    chosen_card = getCardFromHand(player, card_number);
    if (chosen_card == NULL) {
      printf("Please enter the number of a card in your hand cards!\n");
    } else {
      error_occurred = 0;
    }
  } while (error_occurred);
  removeCardFromHand(player, chosen_card);
  addCardToTable(player, chosen_card);
  return 0;
}

int addCardToTable(Player *player, Card *card) {
  Card *head = &player->table_cards_;
  if (head == NULL || head->color_ == '\0') {
    card->next_ = NULL;
    player->table_cards_ = *card;
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

int removeCardFromTable(Player *player, Card *card) {
  Card *head = &player->table_cards_;
  if (head == NULL) {
    return 1;
  }
  while (head != NULL) {
    if (head->next_ == card) {
      head->next_ = card->next_;
      return 0;
    }
    head = head->next_;
  }
  return 1;
}

int addCardToHand(Player *player, Card *card) {
  Card *head = &player->hand_cards_;
  if (head == NULL || head->color_ == '\0') {
    player->hand_cards_ = *card;
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

int removeCardFromHand(Player *player, Card *card) {
  Card *head = &player->hand_cards_;
  if (head == NULL) {
    return 1;
  }
  while (head != NULL) {
    if (head->next_ == card) {
      head->next_ = card->next_;
      card->next_ = NULL;
      return 0;
    }
    head = head->next_;
  }
  return 1;
}

int addCardToRow(Player *player, Card *card, int row_number) {
  Card *head = &player->card_rows_[row_number];
  if (head == NULL) {
    player->card_rows_[row_number] = *card;
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

int sortCards(Card *cards) {
  // Sort the linked list of cards in ascending order, depending on the value of the cards
  Card *head;
  Card *temp = NULL;
  int swapped;
  do {
    swapped = 0;
    head = cards;
    while (head->next_ != temp) {
      if (head->value_ > head->next_->value_) {
        // Swap the cards
        Card *temp_card = head->next_;
        head->next_ = head->next_->next_;
        temp_card->next_ = head;
        swapped = 1;
      }
      head = head->next_;
    }
    temp = head;
  } while (swapped);
  return 0;
}