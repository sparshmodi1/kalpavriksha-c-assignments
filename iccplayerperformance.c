#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

#define TEAM_LIMIT 10
#define MAX_PLAYERS_PER_TEAM 50
#define MAX_TOTAL_PLAYERS (TEAM_LIMIT * MAX_PLAYERS_PER_TEAM)
#define NAME_LENGTH 51
#define ROLE_COUNT 3

typedef enum Role{
    BATSMAN = 1,
    BOWLER,
    ALL_ROUNDER
}Role;

typedef struct PlayerNode {
    int playerId;
    char name[NAME_LENGTH];
    char teamName[NAME_LENGTH];
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
}PlayerNode;

typedef struct Team{
    int teamId;
    char name[NAME_LENGTH];
    int totalPlayers;
    float averageBattingStrikeRate;
    float strikeRateSum;
    int battingPlayerCount;
    PlayerNode *head;
    PlayerNode *rolePlayers[ROLE_COUNT][MAX_PLAYERS_PER_TEAM];
    int roleCount[ROLE_COUNT];
}Team;

Team teamList[TEAM_LIMIT];
int numberOfTeams = 0;

void readLine(const char *message, char *value, int size) {
    printf("%s", message);
    if (fgets(value, size, stdin) == NULL) {
        value[0] = '\0';
        return;
    }
    value[strcspn(value, "\n")] = '\0';
}

int readInt(const char *message) {
    char line[100];
    char *end;
    long value;

    while (1) {
        readLine(message, line, sizeof(line));
        value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end)) end++;
        if (line[0] != '\0' && *end == '\0') return (int)value;
        printf("Invalid input. Try again.\n");
    }
}

float readFloat(const char *message) {
    char line[100];
    char *end;
    float value;

    while (1) {
        readLine(message, line, sizeof(line));
        value = strtof(line, &end);
        while (isspace((unsigned char)*end)) end++;
        if (line[0] != '\0' && *end == '\0') return value;
        printf("Invalid input. Try again.\n");
    }
}

const char *roleName(Role role) {
    if (role == BATSMAN) return "Batsman";
    if (role == BOWLER) return "Bowler";
    return "All-rounder";
}

const char *rolePlural(Role role) {
    if (role == BATSMAN) return "Batsmen";
    if (role == BOWLER) return "Bowlers";
    return "All-rounders";         
}

float getPerformanceIndex(PlayerNode *player) {
    if (player->role == BATSMAN)
        return player->battingAverage * player->strikeRate / 100.0f;
    if (player->role == BOWLER)
        return player->wickets * 2.0f + 100.0f - player->economyRate;
    return player->battingAverage * player->strikeRate / 100.0f
           + player->wickets * 2.0f;
}

Team *findTeam(int teamId) {
    int low = 0, high = numberOfTeams - 1;

    while (low <= high) {
        int middle = (low + high) / 2;
        if (teamList[middle].teamId == teamId) return &teamList[middle];
        if (teamList[middle].teamId < teamId) low = middle + 1;
        else high = middle - 1;
    }
    return NULL;
}

int findTeamByName(const char *name) {
    int i;
    for (i = 0; i < numberOfTeams; i++)
        if (strcmp(teamList[i].name, name) == 0) return i;
    return -1;
}

int playerIdExists(int playerId) {
    int i;
    for (i = 0; i < numberOfTeams; i++) {
        PlayerNode *current = teamList[i].head;
        while (current != NULL) {
            if (current->playerId == playerId) return 1;
            current = current->next;
        }
    }
    return 0;
}

void insertSorted(PlayerNode **list, int *count, PlayerNode *player) {
    int position = *count;

    while (position > 0 &&
           list[position - 1]->performanceIndex < player->performanceIndex) {
        list[position] = list[position - 1];
        position--;
    }
    list[position] = player;
    (*count)++;
}

int addPlayer(Team *team, PlayerNode *player) {
    PlayerNode *current;
    int roleIndex = (int)player->role - 1;

    if (team->totalPlayers >= MAX_PLAYERS_PER_TEAM) return 0;

    player->next = NULL;
    if (team->head == NULL) {
        team->head = player;
    } else {
        current = team->head;
        while (current->next != NULL) current = current->next;
        current->next = player;
    }

    insertSorted(team->rolePlayers[roleIndex], &team->roleCount[roleIndex], player);

    team->totalPlayers++;
    if (player->role == BATSMAN || player->role == ALL_ROUNDER) {
        team->strikeRateSum += player->strikeRate;
        team->battingPlayerCount++;
        team->averageBattingStrikeRate =
            team->strikeRateSum / team->battingPlayerCount;
    }
    return 1;
}

PlayerNode *makePlayer(const Player *source) {
    PlayerNode *player = malloc(sizeof(PlayerNode));
    if (player == NULL) return NULL;

    player->playerId = source->id;
    snprintf(player->name, sizeof(player->name), "%s", source->name);
    snprintf(player->teamName, sizeof(player->teamName), "%s", source->team);
    if (strcmp(source->role, "Batsman") == 0) player->role = BATSMAN;
    else if (strcmp(source->role, "Bowler") == 0) player->role = BOWLER;
    else player->role = ALL_ROUNDER;
    player->totalRuns = source->totalRuns;
    player->battingAverage = source->battingAverage;
    player->strikeRate = source->strikeRate;
    player->wickets = source->wickets;
    player->economyRate = source->economyRate;
    player->performanceIndex = getPerformanceIndex(player);
    player->next = NULL;
    return player;
}

void initializeSystem(void) {
    int i;
    numberOfTeams = teamCount > TEAM_LIMIT ? TEAM_LIMIT : teamCount;

    for (i = 0; i < numberOfTeams; i++) {
        memset(&teamList[i], 0, sizeof(Team));
        teamList[i].teamId = i + 1;
        snprintf(teamList[i].name, sizeof(teamList[i].name), "%s", teams[i]);
    }

    for (i = 0; i < playerCount; i++) {
        int teamIndex = findTeamByName(players[i].team);
        PlayerNode *player;
        if (teamIndex == -1) continue;
        player = makePlayer(&players[i]);
        if (player == NULL || !addPlayer(&teamList[teamIndex], player)) {
            free(player);
            printf("Unable to load player data.\n");
            exit(1);
        }
    }
}

void printTableHead(int showTeam) {
    printf("======================================================================================\n");
    if (showTeam)
        printf("%-6s %-25s %-16s %-12s %8s %7s %7s %6s %6s %10s\n",
               "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    else
        printf("%-6s %-25s %-12s %8s %7s %7s %6s %6s %10s\n",
               "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("======================================================================================\n");
}

void printPlayer(PlayerNode *player, int showTeam) {
    if (showTeam)
        printf("%-6d %-25.25s %-16.16s %-12s %8d %7.1f %7.1f %6d %6.1f %10.2f\n",
               player->playerId, player->name, player->teamName, roleName(player->role),
               player->totalRuns, player->battingAverage, player->strikeRate,
               player->wickets, player->economyRate, player->performanceIndex);
    else
        printf("%-6d %-25.25s %-12s %8d %7.1f %7.1f %6d %6.1f %10.2f\n",
               player->playerId, player->name, roleName(player->role), player->totalRuns,
               player->battingAverage, player->strikeRate, player->wickets,
               player->economyRate, player->performanceIndex);
}

void addPlayerMenu(void) {
    int teamId, roleChoice;
    Team *team;
    PlayerNode *player;

    teamId = readInt("Enter Team ID to add player: ");
    team = findTeam(teamId);
    if (team == NULL) {
        printf("Team ID not found.\n");
        return;
    }
    if (team->totalPlayers >= MAX_PLAYERS_PER_TEAM) {
        printf("This team already has %d players.\n", MAX_PLAYERS_PER_TEAM);
        return;
    }

    printf("Enter Player Details:\n");
    player = calloc(1, sizeof(PlayerNode));
    if (player == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    player->playerId = readInt("Player ID: ");
    if (player->playerId < 1 || player->playerId > 1000 || playerIdExists(player->playerId)) {
        printf("Invalid or duplicate Player ID.\n");
        free(player);
        return;
    }
    readLine("Name: ", player->name, sizeof(player->name));
    if (player->name[0] == '\0') {
        printf("Name cannot be empty.\n");
        free(player);
        return;
    }

    roleChoice = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (roleChoice < 1 || roleChoice > 3) {
        printf("Invalid role.\n");
        free(player);
        return;
    }
    player->role = (Role)roleChoice;
    player->totalRuns = readInt("Total Runs: ");
    player->battingAverage = readFloat("Batting Average: ");
    player->strikeRate = readFloat("Strike Rate: ");
    player->wickets = readInt("Wickets: ");
    player->economyRate = readFloat("Economy Rate: ");
    snprintf(player->teamName, sizeof(player->teamName), "%s", team->name);
    player->performanceIndex = getPerformanceIndex(player);

    if (addPlayer(team, player))
        printf("Player added successfully to Team %s!\n", team->name);
    else {
        printf("Could not add player.\n");
        free(player);
    }
}

void displayTeamPlayers(void) {
    int teamId = readInt("Enter Team ID: ");
    Team *team = findTeam(teamId);
    PlayerNode *current;

    if (team == NULL) {
        printf("Team ID not found.\n");
        return;
    }
    printf("Players of Team %s:\n", team->name);
    printTableHead(0);
    current = team->head;
    while (current != NULL) {
        printPlayer(current, 0);
        current = current->next;
    }
    printf("======================================================================================\n");
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", team->averageBattingStrikeRate);
}

void displayTeamsByStrikeRate(void) {
    Team *sorted[TEAM_LIMIT];
    int i, j;

    for (i = 0; i < numberOfTeams; i++) {
        sorted[i] = &teamList[i];
        j = i;
        while (j > 0 && sorted[j - 1]->averageBattingStrikeRate < sorted[j]->averageBattingStrikeRate) {
            Team *temp = sorted[j - 1];
            sorted[j - 1] = sorted[j];
            sorted[j] = temp;
            j--;
        }
    }

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-5s %-20s %-12s %-14s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");
    for (i = 0; i < numberOfTeams; i++)
        printf("%-5d %-20s %-12.1f %-14d\n", sorted[i]->teamId, sorted[i]->name,
               sorted[i]->averageBattingStrikeRate, sorted[i]->totalPlayers);
    printf("=========================================================\n");
}

int getRoleChoice(void) {
    int role = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    while (role < 1 || role > 3) {
        printf("Invalid role. Choose 1, 2 or 3.\n");
        role = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    }
    return role;
}

void displayTopKPlayers(void) {
    int teamId = readInt("Enter Team ID: ");
    Team *team = findTeam(teamId);
    int roleChoice, k, i, available;

    if (team == NULL) {
        printf("Team ID not found.\n");
        return;
    }
    roleChoice = getRoleChoice();
    k = readInt("Enter number of players: ");
    if (k <= 0) {
        printf("K must be greater than zero.\n");
        return;
    }

    available = team->roleCount[roleChoice - 1];
    printf("Top %d %s of Team %s:\n", k, rolePlural((Role)roleChoice), team->name);
    if (available == 0) {
        printf("No players found for this role.\n");
        return;
    }
    printTableHead(0);
    for (i = 0; i < k && i < available; i++)
        printPlayer(team->rolePlayers[roleChoice - 1][i], 0);
    if (k > available)
        printf("Only %d players are available for this role.\n", available);
}

void mergeRoleTeams(int roleIndex, int left, int right, int offsets[],
                    PlayerNode *result[], PlayerNode *temp[]) {
    int middle, i, j, k, start, split, end;

    if (left == right) {
        for (i = 0; i < teamList[left].roleCount[roleIndex]; i++)
            result[offsets[left] + i] = teamList[left].rolePlayers[roleIndex][i];
        return;
    }

    middle = (left + right) / 2;
    mergeRoleTeams(roleIndex, left, middle, offsets, result, temp);
    mergeRoleTeams(roleIndex, middle + 1, right, offsets, result, temp);

    start = offsets[left];
    split = offsets[middle + 1];
    end = offsets[right + 1];
    i = start;
    j = split;
    k = start;

    while (i < split && j < end) {
        if (result[i]->performanceIndex >= result[j]->performanceIndex)
            temp[k++] = result[i++];
        else
            temp[k++] = result[j++];
    }
    while (i < split) temp[k++] = result[i++];
    while (j < end) temp[k++] = result[j++];
    for (i = start; i < end; i++) result[i] = temp[i];
}

void displayAllPlayersOfRole(void) {
    int roleChoice = getRoleChoice();
    int roleIndex = roleChoice - 1;
    int offsets[TEAM_LIMIT + 1] = {0};
    PlayerNode *result[MAX_TOTAL_PLAYERS];
    PlayerNode *temp[MAX_TOTAL_PLAYERS];
    int i, total;

    for (i = 0; i < numberOfTeams; i++)
        offsets[i + 1] = offsets[i] + teamList[i].roleCount[roleIndex];
    total = offsets[numberOfTeams];

    printf("%s of all teams:\n", rolePlural((Role)roleChoice));
    if (total == 0) {
        printf("No players found for this role.\n");
        return;
    }

    mergeRoleTeams(roleIndex, 0, numberOfTeams - 1, offsets, result, temp);
    printTableHead(1);
    for (i = 0; i < total; i++) printPlayer(result[i], 1);
    printf("======================================================================================\n");
}

void freeSystem(void) {
    int i;
    for (i = 0; i < numberOfTeams; i++) {
        PlayerNode *current = teamList[i].head;
        while (current != NULL) {
            PlayerNode *next = current->next;
            free(current);
            current = next;
        }
    }
}

void displayMenu(void) {
    printf("==============================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    printf("==============================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==============================================================================\n");
}

int main(void) {
    int choice;
    initializeSystem();

    do {
        displayMenu();
        choice = readInt("Enter your choice: ");
        switch (choice) {
            case 1:
                addPlayerMenu();
                break;
            case 2:
                displayTeamPlayers();
                break;
            case 3:
                displayTeamsByStrikeRate();
                break;
            case 4:
                displayTopKPlayers();
                break;
            case 5:
                displayAllPlayersOfRole();
                break;
            case 6:
                printf("Exiting ICC ODI Player Performance Analyzer.\n");
                break;
            default:
                printf("Invalid choice. Please choose 1-6.\n");
        }
    } while (choice != 6);
    freeSystem();
    return 0;
}