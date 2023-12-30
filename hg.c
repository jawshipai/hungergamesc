#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define DEBUG 0

#define MAX_PLAYERS 200 // Maximum allowed players
#define MAX_SCENARIOS 200 // Maximum allowed scenarios
#define playersTextFile "exampleplayers.txt" // name of player file
#define scenariosTextFile "examplescenarios.txt" // name of scenario file
#define SCENARIO_DELAY 1 // the amount (in seconds) of time between each print


int playerCount = 0;
int scenarioCount = 0;

// struct for player
struct Player {
  char* name;
  int adv;
  int elims;
  int health;
};
// generates a random number, [min,max] both inclusive
int randomNum(int min, int max) {
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
// creates a player with name and advantage number (used in combat)
struct Player* createPlayer(char* name, int adv){
    struct Player* newPlayer = malloc(sizeof(struct Player));
    newPlayer->name = malloc(sizeof(char) * strlen(name)+1);
    strcpy(newPlayer->name, name);
    newPlayer->adv = adv;
    newPlayer->elims = 0;
    newPlayer->health = 1;
    return newPlayer;
}
// used to changePlayers name
void changePlayerName(struct Player* player, char* name){
    free(player->name);
    player->name = malloc(sizeof(char)*strlen(name)+1);
    strcpy(player->name, name);
}
// used to change players advantage
void changePlayerAdvantage(struct Player* player, int adv){
    player->adv = adv;
}
// push player above LPI using Index (switching player with LPI+1)
void surfacePlayerIndex(struct Player* players[MAX_PLAYERS], int DPI, int* pLPI) {
    //printf("surfacing player %s\n",players[DPI]->name);
    if(DPI < 0) { return; }
    if(DPI != *(pLPI)+1){
        //printf("switching %s with %s\n",players[DPI]->name,players[*(pLPI)+1]->name);
        struct Player* tmpPlayer = &*(players[DPI]);
        players[DPI] = players[*(pLPI)+1];
        players[*(pLPI)+1] = tmpPlayer;
    }
    *(pLPI) += 1;
}
// push player below LPI using Index (switching player with LPI)
void dropPlayerIndex(struct Player* players[MAX_PLAYERS], int DPI, int* pLPI) {
    //printf("dropping player %s\n",players[DPI]->name);
    if(DPI < 0) { return; }
    if(DPI != *(pLPI)) {
        //printf("switching %s with %s\n",players[DPI]->name,players[*(pLPI)]->name);
        struct Player* tmpPlayer = &*(players[DPI]);
        players[DPI] = players[*(pLPI)];
        players[*(pLPI)] = tmpPlayer;
    }
    *(pLPI) -= 1;
}
// push player above LPI using Player 
void surfacePlayer(struct Player* players[MAX_PLAYERS], struct Player* player, int* pLPI) {
    int DPI = -1; // find DPI (Dead Player Index)
    for(int i = 0, d = 0; i < MAX_PLAYERS && d == 0; i++){
        //printf("comparing &p[i]: %p with &p: %p\n",&*(players[i]),&*(player));
        if(&*(players[i]) == &*(player)) {
            DPI = i;
            d = 1;
        }
    }
    if(DPI != *(pLPI)+1){
        //printf("switching %s with %s\n",players[DPI]->name,players[*(pLPI)+1]->name);
        struct Player* tmpPlayer = &*(players[DPI]);
        players[DPI] = players[*(pLPI)+1];
        players[*(pLPI)+1] = tmpPlayer;
    }
    *(pLPI) += 1;
    //printf("LPI:%d\n",*(pLPI));
}
// push player below LPI using Player
void dropPlayer(struct Player* players[MAX_PLAYERS], struct Player* player, int* pLPI) {
    int DPI = -1; // find DPI (Dead Player Index)
    for(int i = 0, d = 0; i < MAX_PLAYERS && d == 0; i++){
        //printf("comparing &p[i]: %p with &p: %p\n",&*(players[i]),&*(player));
        if(&*(players[i]) == &*(player)) {
            DPI = i;
            d = 1;
        }
    }
    if(DPI == -1) { return; }
    if(DPI != *(pLPI)) {
        struct Player* tmpPlayer = &*(players[DPI]);
        players[DPI] = players[*(pLPI)];
        players[*(pLPI)] = tmpPlayer;
    }
    *(pLPI) -= 1;
}
// fights players in fightersIndex and returns the index of the fighterindex winner
int playerFight(struct Player* players[MAX_PLAYERS], int fightersIndex[]) {
    int lastValidWinner = 0;
    int table[MAX_PLAYERS] = {0};
    int i = 0;
    int prev = 0;
    while(fightersIndex[i] != -2) {
        if(fightersIndex[i] == -1) {
            i++;
            continue;
        }
        table[i] = prev + players[fightersIndex[i]]->adv;
        prev = table[i];
        lastValidWinner = i;
        if(DEBUG) { printf("succesfully added %s to the fight at index %d",players[fightersIndex[i]]->name,i); }
        if(DEBUG) { printf("/table[%d]:%d\n",i,table[i]); }
        i++;
    }
    i-=1;
    int r = randomNum(1,prev);
    if(DEBUG) { printf("chose r:%d\n",r); }
    for(i; i > -1; i--) {
        if(table[i] != 0 && r >= table[i]) {
            return lastValidWinner;
            
        }
        if(table[i] != 0) {
            lastValidWinner = i;
        }
    }
    return lastValidWinner;
}
// prints a players stats
void printPlayer(struct Player* player){
    printf("name:%s adv:%d elims:%d health:%d\n",
    player->name,
    player->adv,
    player->elims,
    player->health);
}
// prints all players
void printPlayers(struct Player* players[MAX_PLAYERS]){
    printf("-----<Players>-----\n");
    for(int i = 0; i < playerCount; i++) {
        if(players[i] == NULL) { continue; }
        printPlayer(players[i]);
    }
}
// reads players from a file (Player1\nPlayer2\n ... Player3\n)
int readPlayers(struct Player* players[MAX_PLAYERS], char* filename) {
    FILE* pfile = fopen(filename,"r");
    char buffer[100] = {0};
    // p = amount of players, b = buffer
    int p = 0, b = 0, done = 0;
    char ch;
    while((ch = fgetc(pfile)) != EOF && done == 0) {
        if(ch == '\n') {
            buffer[b] = 0;
            players[p] = createPlayer(buffer,100);
            b = 0;
            p++;
            if(p == MAX_PLAYERS) { done = 1; }
            continue;
        }
        buffer[b] = ch;
        b++;
    }
    fclose(pfile);
    playerCount = p;
    return p;
}
// reads scenarios from a file (Scenario1\nScenario2\n ... Scenario3\n)
// example: "2#@w and @w defeated @l!" or "1#@p*a+50* found a sword!"
int readScenarios(char* scenarios[MAX_SCENARIOS], char* filename) {
    FILE* sfile = fopen(filename,"r");
    char buffer[200] = {0};
    // s = number of scenarios, b = buffer
    int s = 0, b = 0, done = 0;
    char ch;
    while((ch = fgetc(sfile)) != EOF && done == 0) {
        if(ch == '\n') {
            scenarios[s] = malloc(sizeof(char) * (strlen(buffer)+1));
            buffer[b] = 0;
            strcpy(scenarios[s], buffer);
            b = 0;
            s++;
            if(s == MAX_SCENARIOS) { done = 1; }
            continue;
        }
        buffer[b] = ch;
        b++;
    }
    fclose(sfile);
    scenarioCount = s;
    return s;
}
// gets the "2" from a string such as "2#@w beat @l!"
int findPlayerCount(char* string) {
    int c = 0;
    char* tmp = strdup(string);
    char* orig = tmp;
    char* found = strsep(&tmp,"#");
    c = atoi(found);
    free(orig);
    return c;
}
// compare function for playerCount
int scenarioCompareCount(const void* a, const void* b) {
    return findPlayerCount(*(char**)a) - findPlayerCount(*(char**)b);
}
// sorts scenarios by playerCount 
void sortScenarios(char* scenarios[MAX_SCENARIOS]) {
    qsort(scenarios, scenarioCount,sizeof(scenarios[0]),scenarioCompareCount);
}
// remove impossible scenarios
void removeImpScenarios(char* scenarios[MAX_SCENARIOS], int* pLSI, int LPI) {
    int numOfPlayers = LPI+1;
    if(numOfPlayers == 0) {
        //printf("we have 0 players\n");
        return;
    }
    int s = 0;
    for(int i = 0; i < *(pLSI)+1; i++) {
        //printf("i:%d\n",i);
        // grab # of players in scenario (s)
        s = findPlayerCount(scenarios[i]);
        // compare with numOfPlayers
        //printf("s:%d numOfPlayer: %d\n",s,numOfPlayers);
        if(numOfPlayers == 1) {
            return;
        }
        if(s > numOfPlayers) {
            // make sure LSI (Last Scenario Index) abides by s <= numOfPlayers
            while(findPlayerCount(scenarios[*(pLSI)]) > numOfPlayers) {
                //printf("decrementing pLSI\n");
                *(pLSI) -= 1;
            }
        }
    }
}
// check for @x's where x is from the scenario syntax
int match(char* scenario, int i, int scenarioLength, char* x) {
    if(scenario[i] == x[0] && i+1 < scenarioLength && scenario[i+1] == x[1]) {
        return 1;
    }
    return 0;
}
// generates a scenario a returns a log statement
char* generateScenario(char* scenarios[MAX_SCENARIOS], int* pLSI, struct Player* players[MAX_PLAYERS], int* pLPI) {
    if(DEBUG) { printf("began generateScenario()/"); }
    char buffer[300] = {0};
    char* bptr = &buffer[0];
    int s = randomNum(0,*(pLSI));
    int scenarioLength = strlen(scenarios[s]);
    if(DEBUG) { printf("chose %s\n",scenarios[s]); }
    int i = 0; // scenario interator
    int b = 0; // buffer iterator
    while(scenarios[s][i] != '#') { // exclude #
        i++;
    }
    i++;
    // all variables for calculating
    int randomPlayerCount = 0;
    int poolPlayerCount = 0;
    
    int usedPlayers[10] = {0};
    int u = 0;
    int poolPlayers[10] = {0};
    int p = 0;
    
    while(i < scenarioLength) {
        if(match(scenarios[s],i,scenarioLength,"@r")) {
            int r = randomNum(0,*(pLPI));
            strcat(buffer, players[r]->name);
            b+= strlen(players[r]->name);
            dropPlayerIndex(players, r, pLPI);
            usedPlayers[u++] = *(pLPI)+1;
            randomPlayerCount++;
            i++;
        } else if(match(scenarios[s],i,scenarioLength,"@p")) { // add not modified case
            int r = randomNum(0,*(pLPI));
            players[r]->health = 0; // kill the player
            //printf("chose @p player:%s\n",players[r]->name);
            strcat(buffer, players[r]->name);
            b+= strlen(players[r]->name);
            dropPlayerIndex(players, r, pLPI);
            usedPlayers[u++] = *(pLPI)+1;
            poolPlayers[p++] = *(pLPI)+1;
            poolPlayerCount++;
            i++;
        } else if(match(scenarios[s],i,scenarioLength,"@w")) {
            if(poolPlayerCount > 0) {
                poolPlayers[p] = -2;
                int poolWinnerIndex = playerFight(players, poolPlayers);
                int winnerIndex = poolPlayers[poolWinnerIndex];
                poolPlayers[poolWinnerIndex] = -1;
                players[winnerIndex]->health = 1; // revive the player
                players[winnerIndex]->elims += 1; // FIX jank elim rewarding (subtract @ws later)
                strcat(buffer, players[winnerIndex]->name);
                b+= strlen(players[winnerIndex]->name);
            } else {
                
            }
            i++;
        } else if(match(scenarios[s],i,scenarioLength,"@l")) {
            if(poolPlayerCount > 0) {
                
            } else {
                int r = randomNum(0,*(pLPI));
                players[r]->health = 0; // kill the player
                strcat(buffer, players[r]->name);
                b+= strlen(players[r]->name);
                dropPlayerIndex(players, r, pLPI);
            }
            i++;
        } else if(match(scenarios[s],i,scenarioLength,"@g")) {
            i++;
        } else {
            buffer[b++] = scenarios[s][i];
        }
        i++;
    }
    //printf("attempting to surface all living players\n");
    // surface all living players from scenario
    usedPlayers[u] = -2;
    int offset = 0;
    for(int z = 0; usedPlayers[z] != -2; z++) {
        if(players[usedPlayers[z]]->health > 0) {
            surfacePlayerIndex(players, usedPlayers[z], pLPI);
            for(int y = z, done = 0; usedPlayers[y] != -2 && done == 0; y++) {
                if(usedPlayers[y] == *(pLPI)) {
                    usedPlayers[y] = usedPlayers[z];
                    done = 1;
                }
            }
        }
    }
    //printf("surfaced players, attempting to remove impossible scenarios\n");
    // remove scenarios that cannot be used anymore / return buffer
    removeImpScenarios(scenarios, pLSI, *(pLPI));
    //printf("removing impossible scenarios (LSI is now %d)\n",*(pLSI));
    //printf("output:");
    return bptr;
}
// prints all available scenarios
void printScenarios(char* scenarios[MAX_SCENARIOS]) {
    printf("-----<Scenarios>-----\n");
    for(int i = 0; i < scenarioCount; i++) {
        if(scenarios[i] == NULL) { continue; }
        printf("%s\n",scenarios[i]);
    }
}
// frees a specific player
void freePlayer(struct Player* player){
    free(player->name);
    free(player);
}
// resets players (should be empty) to values at ogplayers
void resetPlayers(struct Player* players[MAX_PLAYERS], struct Player* ogplayers[MAX_PLAYERS], int* pLPI){
    int p = 0;
    for(int i = 0; i < playerCount; i++) {
        if(ogplayers[i] != NULL) {
            players[i] = createPlayer(ogplayers[i]->name, ogplayers[i]->adv);
            p++;
        }
    }
    *(pLPI) = p - 1;
}
// resets scenarios (should be empty) to values at ogscenarios
void resetScenarios(char* scenarios[MAX_SCENARIOS], char* ogscenarios[MAX_SCENARIOS], int* pLSI){
    int s = 0;
    for(int i = 0; i < scenarioCount; i++) {
        if(ogscenarios[i] != NULL) {
            scenarios[i] = strdup(ogscenarios[i]);
            s++;
        }
    }
    *(pLSI) = s - 1;
}
// frees all players
void freePlayers(struct Player* players[MAX_PLAYERS]) {
    for(int i = 0; i < playerCount; i++) {
        if(players[i] == NULL) { continue; }
        freePlayer(players[i]);
    }
}
// frees all scenarios
void freeScenarios(char* scenarios[MAX_SCENARIOS]) {
    for(int i = 0; i < scenarioCount; i++) {
        if(scenarios[i] == 0) { continue; }
        //printf("freeing \"%s\"\n",scenarios[i]);
        free(scenarios[i]);
    }
}

void startSimluation()
{
    printf("-----<Simulation Start>-----\n");
    // READ IN PLAYERS
    struct Player* players[MAX_PLAYERS] = {0};
    int p = readPlayers(players, playersTextFile);
    int LPI = p-1; // Last Player Index 
    int* pLPI = &LPI;
    printPlayers(players);
    // READ IN SCENARIOS
    char* scenarios[MAX_SCENARIOS] = {0};
    int s = readScenarios(scenarios, scenariosTextFile);
    sortScenarios(scenarios);
    int LSI = s-1; // Last Scenario Index
    int* pLSI = &LSI;
    removeImpScenarios(scenarios, pLSI, LPI);
    printScenarios(scenarios);
    // Start the games!
    printf("-----<Game Start>-----\n");
    while(LPI > 0) {
        sleep(SCENARIO_DELAY);
        printf("%s\n",generateScenario(scenarios, pLSI, players, pLPI));
    }
    if(players[0]->health > 0) {
        printf("%s WON THE GAME WITH %d ELIMS!\n",players[0]->name,players[0]->elims);
    } else {
        printf("NOBODY WON!\n");
    }
    printf("-----<Game End>-----\n");
    // free everything
    freePlayers(players);
    freeScenarios(scenarios);
    printf("-----<Simulation End>-----\n");
}

void startOptSimluation(int iterations)
{
    //printf("-----<Simulation Start>-----\n");
    // READ IN PLAYERS
    struct Player* players[MAX_PLAYERS] = {0};
    struct Player* ogplayers[MAX_PLAYERS] = {0};
    int p = readPlayers(ogplayers, playersTextFile);
    int LPI = p-1; // Last Player Index 
    int* pLPI = &LPI;
    //printPlayers(players);
    // READ IN SCENARIOS
    char* scenarios[MAX_SCENARIOS] = {0};
    char* ogscenarios[MAX_SCENARIOS] = {0};
    int s = readScenarios(ogscenarios, scenariosTextFile);
    sortScenarios(ogscenarios);
    int LSI = s-1; // Last Scenario Index
    int* pLSI = &LSI;
    removeImpScenarios(ogscenarios, pLSI, LPI);
    // reset players and scenarios
    //void resetPlayers(struct Player* players[MAX_PLAYERS], struct Player* ogplayers[MAX_PLAYERS], int* pLPI){
    //void resetScenarios(char* scenarios[MAX_SCENARIOS], char* ogscenarios[MAX_SCENARIOS], int* pLSI){
    resetPlayers(players, ogplayers, pLPI);
    resetScenarios(scenarios, ogscenarios, pLSI);
    int i = 0;
    while(i < iterations) {
        while(LPI > 0) {
            generateScenario(scenarios, pLSI, players, pLPI);
        }
        if(players[0]->health > 0) {
        } else {
        }
        freePlayers(players);
        freeScenarios(scenarios);
        resetPlayers(players, ogplayers, pLPI);
        resetScenarios(scenarios, ogscenarios, pLSI);
        i++;
    }
}

int main() {
    srand(time(NULL)); // initialize random
    startSimluation();
    return 0;
}
