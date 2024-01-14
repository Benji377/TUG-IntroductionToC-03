#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WRONG_ARGUMENT_COUNT 1
#define CANNOT_OPEN_FILE 2
#define INVALID_FILE 3
#define MEMORY_ALLOCATION_ERROR 4

const int MAX_CARDS_HAND = 10;
const int MAX_CARD_ROWS = 20;

struct _Card_
{
  char color_;
  int value_;
  struct _Card_ *next_;
};
typedef struct _Card_ Card;

struct _Player_
{
  int player_id_;
  Card *hand_cards_;
  Card *table_cards_;
  Card **card_rows_;
};
typedef struct _Player_ Player;


int checkMainArgumentsCount(int argc);
int checkConfigFile(char *config_file);
int getPlayersCount(char *config_file);
void printWelcomeMessage(int players_count);
Player *createPlayer();
void assignCardsToPlayers(Player *player_one, Player *player_two, char *config_file);
void printCardChoosingPhase();
void printPlayer(Player *player);
void freePlayer(Player *player);


int main(int argc, char *argv[])
{
  // Check if correct number of arguments
  if (checkMainArgumentsCount(argc) != 0)
  {
    return WRONG_ARGUMENT_COUNT;
  }
  // Check if config file is valid
  int config_file_error = checkConfigFile(argv[1]);
  if (config_file_error != 0)
  {
    return config_file_error;
  }
  int players_count = getPlayersCount(argv[1]);
  printWelcomeMessage(players_count);
  // Create the players
  Player *player_one = createPlayer();
  player_one->player_id_ = 1;
  if (player_one == NULL)
  {
    return MEMORY_ALLOCATION_ERROR;
  }
  Player *player_two = createPlayer();
  player_two->player_id_ = 2;
  if (player_two == NULL)
  {
    freePlayer(player_one);
    return MEMORY_ALLOCATION_ERROR;
  }
  // Assign cards to players
  assignCardsToPlayers(player_one, player_two, argv[1]);
  // Start the game
  int game_over = 0;
  do {
    printCardChoosingPhase();
    printPlayer(player_one);
    // TODO: Ask for player input
    printPlayer(player_two);
    game_over = 1;
  } while (!game_over);
  return 0;
}

int checkMainArgumentsCount(int argc)
{
  if (argc != 2)
  {
    printf("Usage: ./a3 <config file>\n");
    return 1;
  }
  return 0;
}

FILE *openFile(char *config_file)
{
  FILE *file = fopen(config_file, "r");
  if (file == NULL)
  {
    printf("Error: Cannot open file: %s\n", config_file);
    return NULL;
  }
  return file;
}

// Read a single line from the config file at a specific line number and return a string
char *readLine(FILE *file, int line_number)
{
  // Go to the beginning of the file
  char *line = NULL;
  size_t len = 0;
  fseek(file, 0, SEEK_SET);
  // Read the file line by line until the line number is reached
  for (int i = 0; i < line_number; i++)
  {
    getline(&line, &len, file);
  }
  return line;

}

int checkConfigFile(char *config_file)
{
  // Check if program can open file
  FILE *file = openFile(config_file);
  if (file == NULL)
  {
    printf("Error: Cannot open file: %s\n", config_file);
    return CANNOT_OPEN_FILE;
  }

  // Check if file contains the correct magic number
  // The first line should be ESP\n in ASCII text
  char *magic_number = "ESP\n";
  char *line = NULL;
  size_t len = 0;
  size_t read;
  read = getline(&line, &len, file);
  if (read == -1)
  {
    printf("Error: Invalid file: %s\n", config_file);
    return INVALID_FILE;
  }
  if (strcmp(line, magic_number) != 0)
  {
    printf("Error: Invalid file: %s\n", config_file);
    return INVALID_FILE;
  }

  return 0;
}

int getPlayersCount(char *config_file)
{
  // The number of players is defined in the second line of the file
  FILE *file = openFile(config_file);
  char *line = readLine(file, 2);
  int players_count = atoi(line);
  return players_count;
}

void printWelcomeMessage(int players_count)
{
  printf("Welcome to SyntaxSakura (%i players are playing)!\n", players_count);
  printf("\n");
}

Player *createPlayer()
{
  Player *player = malloc(sizeof(Player));
  if (player == NULL)
  {
    printf("Error: Memory allocation error\n");
    return NULL;
  }
  player->hand_cards_ = malloc(sizeof(Card) * MAX_CARDS_HAND);
  if (player->hand_cards_ == NULL)
  {
    printf("Error: Out of memory\n");
    freePlayer(player);
    return NULL;
  }
  player->table_cards_ = malloc(sizeof(Card) * MAX_CARDS_HAND);
  if (player->table_cards_ == NULL)
  {
    printf("Error: Out of memory\n");
    freePlayer(player);
    return NULL;
  }
  player->card_rows_ = malloc(sizeof(Card *) * MAX_CARDS_HAND);
  if (player->card_rows_ == NULL)
  {
    printf("Error: Out of memory\n");
    freePlayer(player);
    return NULL;
  }
  player->hand_cards_ = NULL;
  player->table_cards_ = NULL;
  player->card_rows_ = NULL;
  return player;
}

// Create a card from a line in the config file
Card *createCard(char *config_file_line)
{
  Card *card = malloc(sizeof(Card));
  if (card == NULL)
  {
    printf("Error: Memory allocation error\n");
    return NULL;
  }
  // The line in the config has the following format <value>_<color>\n
  // We need to split the line into the value and the color
  char *color = strtok(config_file_line, "_");
  char *value = strtok(NULL, "_");

  card->color_ = *color;
  card->value_ = atoi(value);
  card->next_ = NULL;
  return card;
}

void assignCardsToPlayers(Player *player_one, Player *player_two, char *config_file)
{
  // We start reading the config file from line 3 and assign each card in an alternating fashion to each player
  // The first card is assigned to player one, the second card to player two, the third card to player one, etc.
  // Each player gets exactly 10 cards
  FILE *file = openFile(config_file);
  for (int i = 3; i < 23; i++)
  {
    char *line = readLine(file, i);
    Card *card = createCard(line);
    if (i % 2 == 0)
    {
      // Assign card to player one
      player_one->hand_cards_[i / 2 - 1] = *card;
      // Add the card to the previous card next pointer
      if (i > 3)
      {
        player_one->hand_cards_[i / 2 - 2].next_ = card;
      }
    }
    else
    {
      // Assign card to player two
      player_two->hand_cards_[i / 2] = *card;
      // Add the card to the previous card next pointer
      if (i > 3)
      {
        player_two->hand_cards_[i / 2 - 1].next_ = card;
      }
    }
  }
}

void printCardChoosingPhase()
{
  printf(" -------------------\n");
  printf("CARD CHOOSING PHASE\n");
  printf(" -------------------\n");
  printf("\n");
}

void printPlayer(Player *player)
{
  printf("Player %i:\n", player->player_id_);
  printf("  hand cards:\n");
  Card *head = &player->hand_cards_[0];
  while (head != NULL)
  {
    if (head->next_ != NULL)
    {
      printf("%i_%c ", head->value_, head->color_);
    }
    else
    {
      printf("%i_%c\n", head->value_, head->color_);
    }
    head = head->next_;
  }
  printf("  chosen cards:\n");
  *head = player->table_cards_[0];
  while (head != NULL)
  {
    if (head->next_ != NULL)
    {
      printf("%i_%c ", head->value_, head->color_);
    }
    else
    {
      printf("%i_%c\n", head->value_, head->color_);
    }
    head = head->next_;
  }
  printf("Player card rows:\n");
  if (player->card_rows_[0] == NULL)
  {
    printf("Empty\n");
  }
  else
  {
    for (int i = 0; i < MAX_CARD_ROWS; i++)
    {
      if (player->card_rows_[i] != NULL)
      {
        printf("  row_%i: ", i);
        *head = player->card_rows_[i][0];
        while (head != NULL)
        {
          if (head->next_ != NULL)
          {
            printf("%i_%c ", head->value_, head->color_);
          }
          else
          {
            printf("%i_%c\n", head->value_, head->color_);
          }
          head = head->next_;
        }
      }
    }
    printf("\n");
  }
}

void freePlayer(Player *player)
{
  for (int i = 0; i < MAX_CARDS_HAND; i++)
  {
    free(&player->hand_cards_[i]);
    free(&player->table_cards_[i]);
    free(&player->card_rows_[i]);
  }
  free(player->hand_cards_);
  free(player->table_cards_);
  free(player->card_rows_);
  free(player);
}