#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <random>
#include <algorithm>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <iomanip>
using namespace std;

const int MAP_W = 28;
const int MAP_H = 14;
const int MAX_HP = 100;
const int MAX_ENERGY = 100;

mt19937 rng(static_cast<unsigned int>(time(nullptr)));

int randomInt(int low, int high)
{
    uniform_int_distribution<int> dist(low, high);
    return dist(rng);
}
void clearScreen()
{
#ifdef _WIN32
   system("cls");
#else
    system("clear");
#endif
}

void pauseGame()
{
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void waitForEnter()
{
    string dummy;
    getline(cin, dummy);
}

struct Weapon
{
    string name;
    int damage;
    int cost;
    int ammo;
    int maxAmmo;
};

struct Enemy
{
    string name;
    int hp;
    int maxHp;
    int damage;
    int defense;
    int gold;
    int xp;
    bool boss;
};

struct Quest
{
    string name;
    string description;
    int target;
    int progress;
    int rewardGold;
    int rewardXp;
    bool completed;
};

struct Player
{
    string name;
    int x;
    int y;
    int hp;
    int maxHp;
    int energy;
    int level;
    int xp;
    int gold;
    int score;
    int potions;
    int grenades;
    int kills;
    int shots;
    int wins;
    int losses;
    int armor;
    int weaponIndex;
    vector<Weapon> weapons;
    vector<string> inventory;
};

vector<string> worldMap =
    {
        "############################",
        "#..........#...............#",
        "#.######...#...#######.....#",
        "#.#....#.......#.....#.....#",
        "#.#....#####...#.....#.....#",
        "#.#............#.....#.....#",
        "#.######..######.....####..#",
        "#..........................#",
        "#..####.....######.........#",
        "#..#..#.....#....#.........#",
        "#..#..#######....#####.....#",
        "#..................#.......#",
        "#..........B.......#.......#",
        "############################"};

vector<Quest> quests =
    {
        {"First Blood", "Defeat 3 enemies.", 3, 0, 100, 100, false},
        {"Hunter", "Defeat 8 enemies.", 8, 0, 250, 250, false},
        {"Survivor", "Reach level 5.", 5, 0, 500, 500, false},
        {"Collector", "Collect 500 gold.", 500, 0, 300, 300, false}};

void printTitle()
{
    cout << "============================================================\n";
    cout << "                  ZOMBIE FRONTIER\n";
    cout << "============================================================\n";
    cout << "A C++ Console Survival Adventure\n";
    cout << "============================================================\n";
}

void printMainMenu()
{
    cout << "\n";
    cout << "1. New Game\n";
    cout << "2. Load Game\n";
    cout << "3. Instructions\n";
    cout << "4. Credits\n";
    cout << "5. Exit\n";
    cout << "Choose: ";
}

void printInstructions()
{
    clearScreen();
    printTitle();
    cout << "\nHOW TO PLAY\n";
    cout << "------------------------------------------------------------\n";
    cout << "Explore the city, fight enemies, collect gold and become\n";
    cout << "strong enough to defeat the final boss.\n\n";
    cout << "MOVEMENT\n";
    cout << "W = Up\n";
    cout << "S = Down\n";
    cout << "A = Left\n";
    cout << "D = Right\n\n";
    cout << "COMBAT\n";
    cout << "Attack, use potions, throw grenades, defend or run.\n\n";
    cout << "MAP SYMBOLS\n";
    cout << "@ = Player\n";
    cout << "# = Wall\n";
    cout << ". = Road\n";
    cout << "B = Boss Area\n\n";
    cout << "GAME TIPS\n";
    cout << "- Upgrade your weapons in the shop.\n";
    cout << "- Save your game frequently.\n";
    cout << "- Stronger enemies appear as your level increases.\n";
    cout << "- The final boss appears after enough progress.\n";
    pauseGame();
}

void printCredits()
{
    clearScreen();
    printTitle();
    cout << "\nCREDITS\n";
    cout << "------------------------------------------------------------\n";
    cout << "Game Design      : Zombie Frontier Team\n";
    cout << "Programming      : C++17\n";
    cout << "Engine            : Custom Console Engine\n";
    cout << "Combat            : Turn Based\n";
    cout << "Map System        : Tile Based\n";
    cout << "Save System       : File Based\n";
    cout << "\nBuilt as a large educational C++ game project.\n";
    pauseGame();
}

void resetQuests()
{
    quests =
        {
            {"First Blood", "Defeat 3 enemies.", 3, 0, 100, 100, false},
            {"Hunter", "Defeat 8 enemies.", 8, 0, 250, 250, false},
            {"Survivor", "Reach level 5.", 5, 0, 500, 500, false},
            {"Collector", "Collect 500 gold.", 500, 0, 300, 300, false}};
}

Player createPlayer(const string &name)
{
    Player p;
    p.name = name;
    p.x = 1;
    p.y = 1;
    p.hp = 100;
    p.maxHp = 100;
    p.energy = 100;
    p.level = 1;
    p.xp = 0;
    p.gold = 150;
    p.score = 0;
    p.potions = 3;
    p.grenades = 2;
    p.kills = 0;
    p.shots = 0;
    p.wins = 0;
    p.losses = 0;
    p.armor = 0;
    p.weaponIndex = 0;

    p.weapons.push_back({"Rusty Pistol", 18, 0, 30, 30});
    p.weapons.push_back({"Shotgun", 32, 400, 12, 12});
    p.weapons.push_back({"Assault Rifle", 45, 800, 25, 25});
    p.weapons.push_back({"Plasma Gun", 70, 1500, 20, 20});

    p.inventory.push_back("Bandage");
    p.inventory.push_back("Old Radio");

    return p;
}

bool isInside(int x, int y)
{
    return x >= 0 && x < MAP_W && y >= 0 && y < MAP_H;
}

bool isWalkable(int x, int y)
{
    if (!isInside(x, y))
        return false;

    return worldMap[y][x] != '#';
}

void drawMap(const Player &p)
{
    clearScreen();
    printTitle();

    cout << "\n";
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if (x == p.x && y == p.y)
                cout << '@';
            else
                cout << worldMap[y][x];
        }
        cout << '\n';
    }

    cout << "\nPlayer: " << p.name;
    cout << " | HP: " << p.hp << "/" << p.maxHp;
    cout << " | Energy: " << p.energy;
    cout << " | Level: " << p.level;
    cout << " | Gold: " << p.gold << "\n";
    cout << "Weapon: " << p.weapons[p.weaponIndex].name;
    cout << " | Ammo: " << p.weapons[p.weaponIndex].ammo << "\n";
    cout << "\nW/A/S/D Move | C Combat | I Inventory | P Shop";
    cout << " | Q Quests | V Save | X Exit\n";
}

void showStats(const Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nPLAYER STATISTICS\n";
    cout << "------------------------------------------------------------\n";
    cout << "Name        : " << p.name << '\n';
    cout << "Level       : " << p.level << '\n';
    cout << "XP          : " << p.xp << '\n';
    cout << "HP          : " << p.hp << "/" << p.maxHp << '\n';
    cout << "Energy      : " << p.energy << "/" << MAX_ENERGY << '\n';
    cout << "Armor       : " << p.armor << '\n';
    cout << "Gold        : " << p.gold << '\n';
    cout << "Score       : " << p.score << '\n';
    cout << "Kills       : " << p.kills << '\n';
    cout << "Shots       : " << p.shots << '\n';
    cout << "Wins        : " << p.wins << '\n';
    cout << "Losses      : " << p.losses << '\n';
    cout << "Potions     : " << p.potions << '\n';
    cout << "Grenades    : " << p.grenades << '\n';

    cout << "\nWEAPONS\n";
    for (size_t i = 0; i < p.weapons.size(); i++)
    {
        cout << i + 1 << ". ";
        cout << p.weapons[i].name;
        cout << " | Damage: " << p.weapons[i].damage;
        cout << " | Ammo: " << p.weapons[i].ammo << "/" << p.weapons[i].maxAmmo;
        if (static_cast<int>(i) == p.weaponIndex)
            cout << " [EQUIPPED]";
        cout << '\n';
    }

    pauseGame();
}

void showInventory(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nINVENTORY\n";
    cout << "------------------------------------------------------------\n";
    cout << "Potions  : " << p.potions << '\n';
    cout << "Grenades : " << p.grenades << '\n';

    cout << "\nITEMS\n";
    if (p.inventory.empty())
    {
        cout << "No items.\n";
    }
    else
    {
        for (size_t i = 0; i < p.inventory.size(); i++)
        {
            cout << i + 1 << ". " << p.inventory[i] << '\n';
        }
    }

    cout << "\n1. Use Potion\n";
    cout << "2. Equip Weapon\n";
    cout << "3. Back\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    if (choice == 1)
    {
        if (p.potions <= 0)
        {
            cout << "You have no potions.\n";
        }
        else if (p.hp >= p.maxHp)
        {
            cout << "Your health is already full.\n";
        }
        else
        {
            p.potions--;
            p.hp = min(p.maxHp, p.hp + 40);
            cout << "You restored health.\n";
        }
        pauseGame();
    }
    else if (choice == 2)
    {
        cout << "\nSelect weapon:\n";
        for (size_t i = 0; i < p.weapons.size(); i++)
        {
            cout << i + 1 << ". " << p.weapons[i].name << '\n';
        }

        int w;
        cin >> w;

        if (w >= 1 && w <= static_cast<int>(p.weapons.size()))
        {
            p.weaponIndex = w - 1;
            cout << "Weapon equipped.\n";
        }
        else
        {
            cout << "Invalid weapon.\n";
        }

        pauseGame();
    }
}

Enemy createRandomEnemy(const Player &p)
{
    int roll = randomInt(1, 100);
    Enemy e;

    if (roll <= 45)
    {
        e.name = "Walker";
        e.maxHp = 45 + p.level * 5;
        e.damage = 8 + p.level * 2;
        e.defense = 2 + p.level;
        e.gold = 25 + p.level * 4;
        e.xp = 30 + p.level * 5;
        e.boss = false;
    }
    else if (roll <= 75)
    {
        e.name = "Runner";
        e.maxHp = 35 + p.level * 4;
        e.damage = 13 + p.level * 2;
        e.defense = 1 + p.level;
        e.gold = 35 + p.level * 5;
        e.xp = 40 + p.level * 6;
        e.boss = false;
    }
    else if (roll <= 93)
    {
        e.name = "Brute";
        e.maxHp = 90 + p.level * 8;
        e.damage = 18 + p.level * 3;
        e.defense = 7 + p.level * 2;
        e.gold = 70 + p.level * 8;
        e.xp = 80 + p.level * 10;
        e.boss = false;
    }
    else
    {
        e.name = "Mutant";
        e.maxHp = 130 + p.level * 10;
        e.damage = 24 + p.level * 3;
        e.defense = 10 + p.level * 2;
        e.gold = 120 + p.level * 10;
        e.xp = 120 + p.level * 12;
        e.boss = false;
    }

    e.hp = e.maxHp;
    return e;
}

Enemy createBoss(const Player &p)
{
    Enemy boss;
    boss.name = "THE NECRO LORD";
    boss.maxHp = 600 + p.level * 80;
    boss.hp = boss.maxHp;
    boss.damage = 35 + p.level * 5;
    boss.defense = 15 + p.level * 3;
    boss.gold = 1500;
    boss.xp = 1000;
    boss.boss = true;
    return boss;
}

void updateQuests(Player &p)
{
    for (auto &q : quests)
    {
        if (q.name == "First Blood")
            q.progress = p.kills;

        if (q.name == "Hunter")
            q.progress = p.kills;

        if (q.name == "Survivor")
            q.progress = p.level;

        if (q.name == "Collector")
            q.progress = p.gold;

        if (!q.completed && q.progress >= q.target)
        {
            q.completed = true;
            p.gold += q.rewardGold;
            p.xp += q.rewardXp;
            p.score += q.rewardGold + q.rewardXp;
            cout << "\nQUEST COMPLETE: " << q.name << '\n';
            cout << "Reward: " << q.rewardGold << " gold and ";
            cout << q.rewardXp << " XP.\n";
        }
    }
}

void levelUp(Player &p)
{
    int needed = p.level * 100;

    while (p.xp >= needed)
    {
        p.xp -= needed;
        p.level++;
        p.maxHp += 15;
        p.hp = p.maxHp;
        p.energy = MAX_ENERGY;
        p.armor += 2;
        cout << "\n*** LEVEL UP! ***\n";
        cout << "You are now level " << p.level << ".\n";
        cout << "Max HP increased.\n";
        cout << "Armor increased.\n";
        needed = p.level * 100;
    }

    updateQuests(p);
}

void enemyAttack(Player &p, Enemy &e)
{
    int raw = e.damage + randomInt(-3, 5);
    int damage = max(1, raw - p.armor);

    if (randomInt(1, 100) <= 10)
    {
        damage *= 2;
        cout << e.name << " lands a critical hit!\n";
    }

    p.hp -= damage;
    cout << e.name << " attacks you for " << damage << " damage.\n";

    if (p.hp < 0)
        p.hp = 0;
}

bool playerAttack(Player &p, Enemy &e)
{
    Weapon &weapon = p.weapons[p.weaponIndex];

    if (weapon.ammo <= 0)
    {
        cout << "No ammo! You need to reload.\n";
        return false;
    }

    weapon.ammo--;
    p.shots++;

    int damage = weapon.damage + randomInt(-4, 8);
    bool critical = randomInt(1, 100) <= 15;

    if (critical)
    {
        damage *= 2;
        cout << "CRITICAL HIT!\n";
    }

    int finalDamage = max(1, damage - e.defense);
    e.hp -= finalDamage;

    cout << "You hit " << e.name << " for ";
    cout << finalDamage << " damage.\n";

    if (e.hp < 0)
        e.hp = 0;

    return true;
}

void reloadWeapon(Player &p)
{
    Weapon &weapon = p.weapons[p.weaponIndex];

    if (weapon.ammo == weapon.maxAmmo)
    {
        cout << "Magazine is already full.\n";
        return;
    }

    int missing = weapon.maxAmmo - weapon.ammo;
    weapon.ammo += missing;

    cout << "Reloaded " << weapon.name << ".\n";
}

bool combat(Player &p, Enemy e)
{
    clearScreen();
    printTitle();

    cout << "\nA hostile " << e.name << " appears!\n";

    while (p.hp > 0 && e.hp > 0)
    {
        cout << "\n------------------------------------------------------------\n";
        cout << p.name << " HP: " << p.hp << "/" << p.maxHp;
        cout << " | Energy: " << p.energy << '\n';
        cout << e.name << " HP: " << e.hp << "/" << e.maxHp << '\n';
        cout << "------------------------------------------------------------\n";

        cout << "1. Attack\n";
        cout << "2. Heavy Attack\n";
        cout << "3. Grenade\n";
        cout << "4. Potion\n";
        cout << "5. Reload\n";
        cout << "6. Defend\n";
        cout << "7. Run\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        bool enemyTurn = true;

        if (choice == 1)
        {
            playerAttack(p, e);
        }
        else if (choice == 2)
        {
            if (p.energy < 25)
            {
                cout << "Not enough energy.\n";
                enemyTurn = false;
            }
            else
            {
                p.energy -= 25;
                int damage = p.weapons[p.weaponIndex].damage * 2;
                damage += randomInt(0, 15);
                damage = max(1, damage - e.defense / 2);
                e.hp -= damage;
                p.shots++;
                if (p.weapons[p.weaponIndex].ammo > 0)
                    p.weapons[p.weaponIndex].ammo--;
                cout << "Heavy attack deals " << damage << " damage.\n";
            }
        }
        else if (choice == 3)
        {
            if (p.grenades <= 0)
            {
                cout << "No grenades left.\n";
                enemyTurn = false;
            }
            else
            {
                p.grenades--;
                int damage = 55 + randomInt(0, 30);
                e.hp -= damage;
                cout << "Grenade explodes for " << damage << " damage!\n";
            }
        }
        else if (choice == 4)
        {
            if (p.potions <= 0)
            {
                cout << "No potions left.\n";
                enemyTurn = false;
            }
            else if (p.hp == p.maxHp)
            {
                cout << "Health is already full.\n";
                enemyTurn = false;
            }
            else
            {
                p.potions--;
                int heal = 40 + randomInt(0, 15);
                p.hp = min(p.maxHp, p.hp + heal);
                cout << "You heal for " << heal << " HP.\n";
            }
        }
        else if (choice == 5)
        {
            reloadWeapon(p);
            enemyTurn = false;
        }
        else if (choice == 6)
        {
            int oldArmor = p.armor;
            p.armor += 12;
            cout << "You defend and temporarily gain protection.\n";
            enemyAttack(p, e);
            p.armor = oldArmor;
            enemyTurn = false;
        }
        else if (choice == 7)
        {
            if (e.boss)
            {
                cout << "You cannot run from the final boss!\n";
            }
            else if (randomInt(1, 100) <= 55)
            {
                cout << "You escaped successfully.\n";
                return false;
            }
            else
            {
                cout << "Escape failed!\n";
            }
        }
        else
        {
            cout << "Invalid choice.\n";
            enemyTurn = false;
        }

        if (e.hp <= 0)
            break;

        if (enemyTurn)
        {
            enemyAttack(p, e);
        }

        p.energy = min(MAX_ENERGY, p.energy + 8);
    }

    if (p.hp <= 0)
    {
        p.losses++;
        cout << "\nYou were defeated by " << e.name << ".\n";
        cout << "Game Over.\n";
        return false;
    }

    p.wins++;
    p.kills++;

    int goldReward = e.gold;
    int xpReward = e.xp;

    p.gold += goldReward;
    p.xp += xpReward;
    p.score += goldReward + xpReward;

    cout << "\nYou defeated " << e.name << "!\n";
    cout << "Gold +" << goldReward << '\n';
    cout << "XP +" << xpReward << '\n';

    if (randomInt(1, 100) <= 25)
    {
        p.potions++;
        cout << "Enemy dropped a potion!\n";
    }

    if (randomInt(1, 100) <= 15)
    {
        p.grenades++;
        cout << "Enemy dropped a grenade!\n";
    }

    levelUp(p);
    pauseGame();

    return true;
}

void showQuests(const Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nQUEST LOG\n";
    cout << "------------------------------------------------------------\n";

    for (const auto &q : quests)
    {
        cout << "\n"
             << q.name << '\n';
        cout << q.description << '\n';
        cout << "Progress: " << q.progress << "/" << q.target << '\n';

        if (q.completed)
            cout << "Status: COMPLETED\n";
        else
            cout << "Status: ACTIVE\n";
    }

    pauseGame();
}

void healAtCamp(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nSAFE CAMP\n";
    cout << "------------------------------------------------------------\n";
    cout << "A safe camp restores your health and energy.\n";

    p.hp = p.maxHp;
    p.energy = MAX_ENERGY;

    cout << "Health restored.\n";
    cout << "Energy restored.\n";

    pauseGame();
}

void shop(Player &p)
{
    while (true)
    {
        clearScreen();
        printTitle();

        cout << "\nSHOP\n";
        cout << "------------------------------------------------------------\n";
        cout << "Your Gold: " << p.gold << "\n\n";

        cout << "1. Buy Potion - 50 gold\n";
        cout << "2. Buy Grenade - 75 gold\n";
        cout << "3. Buy Armor Upgrade - 250 gold\n";
        cout << "4. Buy Shotgun - 400 gold\n";
        cout << "5. Buy Assault Rifle - 800 gold\n";
        cout << "6. Buy Plasma Gun - 1500 gold\n";
        cout << "7. Full Heal - 100 gold\n";
        cout << "8. Back\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            if (p.gold >= 50)
            {
                p.gold -= 50;
                p.potions++;
                cout << "Potion purchased.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 2)
        {
            if (p.gold >= 75)
            {
                p.gold -= 75;
                p.grenades++;
                cout << "Grenade purchased.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 3)
        {
            if (p.gold >= 250)
            {
                p.gold -= 250;
                p.armor += 3;
                cout << "Armor upgraded.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 4)
        {
            if (p.gold >= 400)
            {
                p.gold -= 400;
                p.weapons[1].ammo = p.weapons[1].maxAmmo;
                cout << "Shotgun purchased.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 5)
        {
            if (p.gold >= 800)
            {
                p.gold -= 800;
                p.weapons[2].ammo = p.weapons[2].maxAmmo;
                cout << "Assault Rifle purchased.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 6)
        {
            if (p.gold >= 1500)
            {
                p.gold -= 1500;
                p.weapons[3].ammo = p.weapons[3].maxAmmo;
                cout << "Plasma Gun purchased.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 7)
        {
            if (p.gold >= 100)
            {
                p.gold -= 100;
                p.hp = p.maxHp;
                cout << "Fully healed.\n";
            }
            else
                cout << "Not enough gold.\n";
        }
        else if (choice == 8)
        {
            return;
        }
        else
        {
            cout << "Invalid choice.\n";
        }

        pauseGame();
    }
}

void randomEvent(Player &p)
{
    int event = randomInt(1, 100);

    if (event <= 12)
    {
        cout << "\nYou found a hidden supply box!\n";
        int gold = randomInt(30, 100);
        p.gold += gold;
        cout << "You found " << gold << " gold.\n";

        if (randomInt(1, 100) <= 50)
        {
            p.potions++;
            cout << "You also found a potion.\n";
        }
    }
    else if (event <= 20)
    {
        cout << "\nYou stepped on broken glass.\n";
        int damage = randomInt(3, 12);
        p.hp = max(1, p.hp - damage);
        cout << "You lost " << damage << " HP.\n";
    }
    else if (event <= 26)
    {
        cout << "\nYou discovered an abandoned medical station.\n";
        p.hp = min(p.maxHp, p.hp + 25);
        cout << "You restored some health.\n";
    }
    else if (event <= 30)
    {
        cout << "\nYou discovered an old weapon crate.\n";
        p.grenades++;
        cout << "You found a grenade.\n";
    }
}

void tryMove(Player &p, char direction)
{
    int nx = p.x;
    int ny = p.y;

    if (direction == 'w' || direction == 'W')
        ny--;

    if (direction == 's' || direction == 'S')
        ny++;

    if (direction == 'a' || direction == 'A')
        nx--;

    if (direction == 'd' || direction == 'D')
        nx++;

    if (!isWalkable(nx, ny))
    {
        cout << "You cannot move there.\n";
        pauseGame();
        return;
    }

    p.x = nx;
    p.y = ny;

    randomEvent(p);

    int encounterChance = 18 + p.level * 2;

    if (randomInt(1, 100) <= encounterChance)
    {
        Enemy e = createRandomEnemy(p);
        combat(p, e);
    }
}

void bossFight(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nYou enter the boss area...\n";
    cout << "The ground begins to shake.\n";
    cout << "A massive creature appears.\n\n";

    Enemy boss = createBoss(p);

    bool survived = combat(p, boss);

    if (survived && p.hp > 0)
    {
        cout << "\n============================================================\n";
        cout << "                    VICTORY!\n";
        cout << "============================================================\n";
        cout << "You defeated THE NECRO LORD.\n";
        cout << "The city is finally safe.\n";
        cout << "Final Score: " << p.score << "\n";
        cout << "Total Kills: " << p.kills << "\n";
        cout << "Final Level: " << p.level << "\n";
        cout << "============================================================\n";
        pauseGame();
    }
}

bool saveGame(const Player &p, const string &filename)
{
    ofstream out(filename);

    if (!out)
        return false;

    out << p.name << '\n';
    out << p.x << '\n';
    out << p.y << '\n';
    out << p.hp << '\n';
    out << p.maxHp << '\n';
    out << p.energy << '\n';
    out << p.level << '\n';
    out << p.xp << '\n';
    out << p.gold << '\n';
    out << p.score << '\n';
    out << p.potions << '\n';
    out << p.grenades << '\n';
    out << p.kills << '\n';
    out << p.shots << '\n';
    out << p.wins << '\n';
    out << p.losses << '\n';
    out << p.armor << '\n';
    out << p.weaponIndex << '\n';

    out << p.weapons.size() << '\n';

    for (const auto &w : p.weapons)
    {
        out << w.name << '\n';
        out << w.damage << '\n';
        out << w.cost << '\n';
        out << w.ammo << '\n';
        out << w.maxAmmo << '\n';
    }

    out << p.inventory.size() << '\n';

    for (const auto &item : p.inventory)
        out << item << '\n';

    out.close();

    return true;
}

bool loadGame(Player &p, const string &filename)
{
    ifstream in(filename);

    if (!in)
        return false;

    getline(in, p.name);

    in >> p.x;
    in >> p.y;
    in >> p.hp;
    in >> p.maxHp;
    in >> p.energy;
    in >> p.level;
    in >> p.xp;
    in >> p.gold;
    in >> p.score;
    in >> p.potions;
    in >> p.grenades;
    in >> p.kills;
    in >> p.shots;
    in >> p.wins;
    in >> p.losses;
    in >> p.armor;
    in >> p.weaponIndex;

    size_t weaponCount;
    in >> weaponCount;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    p.weapons.clear();

    for (size_t i = 0; i < weaponCount; i++)
    {
        Weapon w;
        getline(in, w.name);
        in >> w.damage;
        in >> w.cost;
        in >> w.ammo;
        in >> w.maxAmmo;
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        p.weapons.push_back(w);
    }

    size_t itemCount;
    in >> itemCount;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    p.inventory.clear();

    for (size_t i = 0; i < itemCount; i++)
    {
        string item;
        getline(in, item);
        p.inventory.push_back(item);
    }

    in.close();

    return true;
}

void saveGameMenu(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nSAVE GAME\n";
    cout << "------------------------------------------------------------\n";
    cout << "1. Save to slot 1\n";
    cout << "2. Save to slot 2\n";
    cout << "3. Save to slot 3\n";
    cout << "4. Back\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    string file;

    if (choice == 1)
        file = "zombie_save_1.dat";
    else if (choice == 2)
        file = "zombie_save_2.dat";
    else if (choice == 3)
        file = "zombie_save_3.dat";
    else
        return;

    if (saveGame(p, file))
        cout << "Game saved successfully.\n";
    else
        cout << "Could not save the game.\n";

    pauseGame();
}

bool loadGameMenu(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nLOAD GAME\n";
    cout << "------------------------------------------------------------\n";
    cout << "1. Load slot 1\n";
    cout << "2. Load slot 2\n";
    cout << "3. Load slot 3\n";
    cout << "4. Back\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    string file;

    if (choice == 1)
        file = "zombie_save_1.dat";
    else if (choice == 2)
        file = "zombie_save_2.dat";
    else if (choice == 3)
        file = "zombie_save_3.dat";
    else
        return false;

    if (loadGame(p, file))
    {
        cout << "Game loaded successfully.\n";
        pauseGame();
        return true;
    }

    cout << "No valid save found.\n";
    pauseGame();
    return false;
}

void changeWeapon(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nWEAPON SELECT\n";
    cout << "------------------------------------------------------------\n";

    for (size_t i = 0; i < p.weapons.size(); i++)
    {
        cout << i + 1 << ". ";
        cout << p.weapons[i].name;
        cout << " | Damage " << p.weapons[i].damage;
        cout << " | Ammo " << p.weapons[i].ammo;
        cout << '\n';
    }

    cout << "\nChoose weapon: ";

    int choice;
    cin >> choice;

    if (choice >= 1 && choice <= static_cast<int>(p.weapons.size()))
    {
        p.weaponIndex = choice - 1;
        cout << "Weapon changed.\n";
    }
    else
    {
        cout << "Invalid weapon.\n";
    }

    pauseGame();
}

void rest(Player &p)
{
    clearScreen();
    printTitle();

    cout << "\nREST AREA\n";
    cout << "------------------------------------------------------------\n";
    cout << "You rest for a while.\n";

    p.energy = MAX_ENERGY;

    int heal = randomInt(10, 30);
    p.hp = min(p.maxHp, p.hp + heal);

    cout << "Energy fully restored.\n";
    cout << "Recovered " << heal << " HP.\n";

    pauseGame();
}

void printHelp()
{
    clearScreen();
    printTitle();

    cout << "\nHELP\n";
    cout << "------------------------------------------------------------\n";
    cout << "W A S D : Move around the map\n";
    cout << "C       : Start a random combat encounter\n";
    cout << "I       : Open inventory\n";
    cout << "P       : Open shop\n";
    cout << "Q       : View quests\n";
    cout << "V       : Save game\n";
    cout << "T       : View statistics\n";
    cout << "E       : Equip weapon\n";
    cout << "R       : Rest\n";
    cout << "H       : Help\n";
    cout << "X       : Exit to main menu\n";
    cout << "\nFind the B tile to challenge the final boss.\n";
    pauseGame();
}

bool confirmExit()
{
    cout << "\nAre you sure you want to exit? (y/n): ";

    char choice;
    cin >> choice;

    return choice == 'y' || choice == 'Y';
}

void checkSpecialTile(Player &p)
{
    char tile = worldMap[p.y][p.x];

    if (tile == 'B')
    {
        if (p.level < 5)
        {
            cout << "\nThe boss gate is locked.\n";
            cout << "Reach level 5 to enter.\n";
            pauseGame();
        }
        else
        {
            bossFight(p);
        }
    }
}

void randomShopEncounter(Player &p)
{
    if (randomInt(1, 100) <= 5)
    {
        cout << "\nA travelling merchant appears!\n";
        cout << "He has supplies for sale.\n";
        pauseGame();
        shop(p);
    }
}

void gameLoop(Player &p)
{
    resetQuests();

    while (p.hp > 0)
    {
        drawMap(p);

        char command;
        cin >> command;

        if (command == 'w' || command == 'W' ||
            command == 'a' || command == 'A' ||
            command == 's' || command == 'S' ||
            command == 'd' || command == 'D')
        {
            tryMove(p, command);
            checkSpecialTile(p);
            randomShopEncounter(p);
        }
        else if (command == 'c' || command == 'C')
        {
            Enemy e = createRandomEnemy(p);
            combat(p, e);
        }
        else if (command == 'i' || command == 'I')
        {
            showInventory(p);
        }
        else if (command == 'p' || command == 'P')
        {
            shop(p);
        }
        else if (command == 'q' || command == 'Q')
        {
            showQuests(p);
        }
        else if (command == 'v' || command == 'V')
        {
            saveGameMenu(p);
        }
        else if (command == 't' || command == 'T')
        {
            showStats(p);
        }
        else if (command == 'e' || command == 'E')
        {
            changeWeapon(p);
        }
        else if (command == 'r' || command == 'R')
        {
            rest(p);
        }
        else if (command == 'h' || command == 'H')
        {
            printHelp();
        }
        else if (command == 'x' || command == 'X')
        {
            if (confirmExit())
                return;
        }
        else
        {
            cout << "Unknown command. Press H for help.\n";
            pauseGame();
        }

        if (p.hp <= 0)
        {
            cout << "\nYou died.\n";
            pauseGame();
            return;
        }
    }
}

string askPlayerName()
{
    clearScreen();
    printTitle();

    string name;

    cout << "\nEnter your survivor name: ";
    cin >> name;

    if (name.empty())
        name = "Survivor";

    return name;
}

void newGame()
{
    string name = askPlayerName();
    Player p = createPlayer(name);

    clearScreen();
    printTitle();

    cout << "\nWelcome, " << p.name << ".\n";
    cout << "The city has fallen.\n";
    cout << "Your mission is to survive and defeat the Necro Lord.\n";
    cout << "Reach level 5 and enter the B area.\n";

    pauseGame();

    gameLoop(p);
}

void loadedGame()
{
    Player p;

    if (!loadGameMenu(p))
        return;

    gameLoop(p);
}

void gameTips()
{
    clearScreen();
    printTitle();

    vector<string> tips =
        {
            "Save often before entering dangerous areas.",
            "Heavy attacks consume energy but deal huge damage.",
            "Grenades are especially useful against bosses.",
            "Upgrade armor if enemies start hitting too hard.",
            "The Plasma Gun is expensive but extremely powerful.",
            "Potions can turn a losing battle around.",
            "Complete quests for extra gold and XP.",
            "Explore the entire map to find random events.",
            "The final boss requires level 5.",
            "Running is not possible against the final boss."};

    cout << "\nSURVIVAL TIPS\n";
    cout << "------------------------------------------------------------\n";

    for (size_t i = 0; i < tips.size(); i++)
        cout << i + 1 << ". " << tips[i] << '\n';

    pauseGame();
}

void settings()
{
    clearScreen();
    printTitle();

    cout << "\nSETTINGS\n";
    cout << "------------------------------------------------------------\n";
    cout << "This version uses a lightweight standard C++ console engine.\n";
    cout << "No external graphics or audio library is required.\n";
    cout << "\nGraphics Mode: ASCII\n";
    cout << "Combat Mode  : Turn Based\n";
    cout << "Save System  : Local Files\n";

    pauseGame();
}

void showAbout()
{
    clearScreen();
    printTitle();

    cout << "\nABOUT THE GAME\n";
    cout << "------------------------------------------------------------\n";
    cout << "Zombie Frontier is a survival RPG built entirely in C++.\n";
    cout << "It demonstrates many common game-programming concepts:\n\n";
    cout << "- Game loop\n";
    cout << "- State management\n";
    cout << "- Player systems\n";
    cout << "- Enemy AI decisions\n";
    cout << "- Combat calculations\n";
    cout << "- Inventory\n";
    cout << "- Shops\n";
    cout << "- Quests\n";
    cout << "- Level progression\n";
    cout << "- Random events\n";
    cout << "- Save and load\n";
    cout << "- Map collision\n";
    cout << "- Boss battles\n";

    pauseGame();
}

void extraMenu()
{
    while (true)
    {
        clearScreen();
        printTitle();

        cout << "\nEXTRAS\n";
        cout << "------------------------------------------------------------\n";
        cout << "1. Survival Tips\n";
        cout << "2. Settings\n";
        cout << "3. About\n";
        cout << "4. Back\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
            gameTips();
        else if (choice == 2)
            settings();
        else if (choice == 3)
            showAbout();
        else if (choice == 4)
            return;
        else
        {
            cout << "Invalid choice.\n";
            pauseGame();
        }
    }
}

int calculateCriticalChance(const Player &p)
{
    return min(35, 10 + p.level * 2);
}

int calculatePlayerDefense(const Player &p)
{
    return p.armor + p.level;
}

int calculateWeaponPower(const Player &p)
{
    if (p.weaponIndex < 0 || p.weaponIndex >= static_cast<int>(p.weapons.size()))
        return 0;
    return p.weapons[p.weaponIndex].damage;
}

bool hasWeapon(const Player &p, const string &name)
{
    for (const auto &weapon : p.weapons)
    {
        if (weapon.name == name)
            return true;
    }
    return false;
}

void giveBonus(Player &p, int gold, int xp)
{
    p.gold += gold;
    p.xp += xp;
    p.score += gold + xp;
    levelUp(p);
}

void repairWeapons(Player &p)
{
    for (auto &weapon : p.weapons)
        weapon.ammo = weapon.maxAmmo;
}

void emergencyRecovery(Player &p)
{
    if (p.hp <= 20 && p.potions > 0)
    {
        p.potions--;
        p.hp = min(p.maxHp, p.hp + 35);
    }
}

int totalWeaponDamage(const Player &p)
{
    int total = 0;

    for (const auto &weapon : p.weapons)
        total += weapon.damage;

    return total;
}

int countFullMagazines(const Player &p)
{
    int count = 0;

    for (const auto &weapon : p.weapons)
    {
        if (weapon.ammo == weapon.maxAmmo)
            count++;
    }

    return count;
}

void debugPlayer(const Player &p)
{
    cout << "DEBUG PLAYER\n";
    cout << "Name: " << p.name << '\n';
    cout << "Position: " << p.x << "," << p.y << '\n';
    cout << "Level: " << p.level << '\n';
    cout << "XP: " << p.xp << '\n';
    cout << "Gold: " << p.gold << '\n';
    cout << "HP: " << p.hp << '\n';
    cout << "Armor: " << p.armor << '\n';
}

bool isBossArea(const Player &p)
{
    return worldMap[p.y][p.x] == 'B';
}

int distanceFromStart(const Player &p)
{
    return abs(p.x - 1) + abs(p.y - 1);
}

void awardExplorationXP(Player &p)
{
    int distance = distanceFromStart(p);

    if (distance > 10)
    {
        p.xp += 5;
        levelUp(p);
    }
}

void consumeEnergy(Player &p, int amount)
{
    p.energy = max(0, p.energy - amount);
}

bool canUseHeavyAttack(const Player &p)
{
    return p.energy >= 25;
}

bool canUseGrenade(const Player &p)
{
    return p.grenades > 0;
}

bool canUsePotion(const Player &p)
{
    return p.potions > 0 && p.hp < p.maxHp;
}

void refillEnergy(Player &p)
{
    p.energy = MAX_ENERGY;
}

void addInventoryItem(Player &p, const string &item)
{
    p.inventory.push_back(item);
}

bool removeInventoryItem(Player &p, const string &item)
{
    auto it = find(p.inventory.begin(), p.inventory.end(), item);

    if (it == p.inventory.end())
        return false;

    p.inventory.erase(it);
    return true;
}

int inventoryCount(const Player &p)
{
    return static_cast<int>(p.inventory.size());
}

void printWeaponDetails(const Weapon &w)
{
    cout << "\nWeapon: " << w.name << '\n';
    cout << "Damage: " << w.damage << '\n';
    cout << "Ammo: " << w.ammo << "/" << w.maxAmmo << '\n';
    cout << "Cost: " << w.cost << '\n';
}

void printEnemyDetails(const Enemy &e)
{
    cout << "\nEnemy: " << e.name << '\n';
    cout << "HP: " << e.hp << "/" << e.maxHp << '\n';
    cout << "Damage: " << e.damage << '\n';
    cout << "Defense: " << e.defense << '\n';
    cout << "Gold: " << e.gold << '\n';
    cout << "XP: " << e.xp << '\n';
}

int estimateEnemyThreat(const Enemy &e)
{
    return e.hp + e.damage * 5 + e.defense * 3;
}

int estimatePlayerPower(const Player &p)
{
    return p.maxHp + p.armor * 5 + calculateWeaponPower(p) * 4 + p.level * 20;
}

bool isPlayerStrongEnough(const Player &p, const Enemy &e)
{
    return estimatePlayerPower(p) >= estimateEnemyThreat(e);
}

void displayBattleAdvice(const Player &p, const Enemy &e)
{
    if (isPlayerStrongEnough(p, e))
        cout << "Battle advice: You have a reasonable chance.\n";
    else
        cout << "Battle advice: Consider upgrading before fighting.\n";
}

int getLevelRequirementForBoss()
{
    return 5;
}

bool bossUnlocked(const Player &p)
{
    return p.level >= getLevelRequirementForBoss();
}

void printBossRequirement(const Player &p)
{
    cout << "Boss requirement: Level " << getLevelRequirementForBoss() << ".\n";

    if (bossUnlocked(p))
        cout << "Boss is unlocked.\n";
    else
        cout << "Boss is still locked.\n";
}

int getExperienceRequirement(int level)
{
    return max(100, level * 100);
}

int getMissingExperience(const Player &p)
{
    return max(0, getExperienceRequirement(p.level) - p.xp);
}

void printProgress(const Player &p)
{
    cout << "Level: " << p.level << '\n';
    cout << "XP: " << p.xp << "/" << getExperienceRequirement(p.level) << '\n';
    cout << "XP Needed: " << getMissingExperience(p) << '\n';
}

void printMapLegend()
{
    cout << "\nMAP LEGEND\n";
    cout << "@ Player\n";
    cout << "# Wall\n";
    cout << ". Road\n";
    cout << "B Boss Area\n";
}

bool isNearBoss(const Player &p)
{
    int bx = -1;
    int by = -1;

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if (worldMap[y][x] == 'B')
            {
                bx = x;
                by = y;
            }
        }
    }

    if (bx == -1)
        return false;

    return abs(p.x - bx) + abs(p.y - by) <= 2;
}

void bossWarning(const Player &p)
{
    if (isNearBoss(p) && !bossUnlocked(p))
    {
        cout << "\nWARNING: You are approaching the boss area.\n";
        cout << "Reach level " << getLevelRequirementForBoss() << " first.\n";
    }
}

int countCompletedQuests()
{
    int count = 0;

    for (const auto &q : quests)
    {
        if (q.completed)
            count++;
    }

    return count;
}

int totalQuestRewards()
{
    int total = 0;

    for (const auto &q : quests)
        total += q.rewardGold + q.rewardXp;

    return total;
}

void printQuestSummary()
{
    cout << "Completed quests: " << countCompletedQuests();
    cout << "/" << quests.size() << '\n';
    cout << "Total possible rewards: " << totalQuestRewards() << '\n';
}

void restoreDefaultWeapons(Player &p)
{
    if (p.weapons.empty())
    {
        p.weapons.push_back({"Rusty Pistol", 18, 0, 30, 30});
        p.weapons.push_back({"Shotgun", 32, 400, 12, 12});
        p.weapons.push_back({"Assault Rifle", 45, 800, 25, 25});
        p.weapons.push_back({"Plasma Gun", 70, 1500, 20, 20});
    }
}

void validatePlayer(Player &p)
{
    p.hp = max(0, min(p.hp, p.maxHp));
    p.energy = max(0, min(p.energy, MAX_ENERGY));
    p.level = max(1, p.level);
    p.gold = max(0, p.gold);
    p.potions = max(0, p.potions);
    p.grenades = max(0, p.grenades);

    if (!isInside(p.x, p.y) || !isWalkable(p.x, p.y))
    {
        p.x = 1;
        p.y = 1;
    }

    restoreDefaultWeapons(p);

    if (p.weaponIndex < 0 ||
        p.weaponIndex >= static_cast<int>(p.weapons.size()))
    {
        p.weaponIndex = 0;
    }
}

void saveCheckpoint(Player &p)
{
    validatePlayer(p);

    if (saveGame(p, "zombie_checkpoint.dat"))
        cout << "Checkpoint saved.\n";
}

bool loadCheckpoint(Player &p)
{
    if (!loadGame(p, "zombie_checkpoint.dat"))
        return false;

    validatePlayer(p);
    return true;
}

void deleteCheckpoint()
{
    remove("zombie_checkpoint.dat");
}

int safeRandomDamage(int base, int variance)
{
    if (variance <= 0)
        return max(1, base);

    return max(1, base + randomInt(-variance, variance));
}

int applyDefense(int damage, int defense)
{
    return max(1, damage - defense);
}

int calculateGoldDrop(const Enemy &e)
{
    return max(0, e.gold + randomInt(-5, 10));
}

int calculateXpDrop(const Enemy &e)
{
    return max(0, e.xp + randomInt(-5, 10));
}

bool rollChance(int percentage)
{
    percentage = max(0, min(100, percentage));
    return randomInt(1, 100) <= percentage;
}

void awardKillScore(Player &p, const Enemy &e)
{
    int value = e.boss ? 1000 : 100;
    p.score += value;
}

void registerKill(Player &p, const Enemy &e)
{
    p.kills++;
    awardKillScore(p, e);
}

void grantEnemyRewards(Player &p, const Enemy &e)
{
    p.gold += calculateGoldDrop(e);
    p.xp += calculateXpDrop(e);
    registerKill(p, e);
    levelUp(p);
}

void randomLoot(Player &p)
{
    int roll = randomInt(1, 100);

    if (roll <= 20)
    {
        p.potions++;
        cout << "Loot: Potion.\n";
    }
    else if (roll <= 35)
    {
        p.grenades++;
        cout << "Loot: Grenade.\n";
    }
    else if (roll <= 45)
    {
        p.gold += 50;
        cout << "Loot: 50 bonus gold.\n";
    }
}

void printEndGameStats(const Player &p)
{
    cout << "\nFINAL STATISTICS\n";
    cout << "Name: " << p.name << '\n';
    cout << "Level: " << p.level << '\n';
    cout << "Score: " << p.score << '\n';
    cout << "Kills: " << p.kills << '\n';
    cout << "Wins: " << p.wins << '\n';
    cout << "Losses: " << p.losses << '\n';
    cout << "Gold: " << p.gold << '\n';
    cout << "Shots: " << p.shots << '\n';
}

void printWelcomeBanner()
{
    cout << "\n****************************************************\n";
    cout << "*             WELCOME TO ZOMBIE FRONTIER          *\n";
    cout << "****************************************************\n";
}

void printGameVersion()
{
    cout << "Version 1.0 | C++17 Edition\n";
}

void printControlsShort()
{
    cout << "WASD Move | C Fight | I Inventory | P Shop | Q Quests\n";
}

void printObjective()
{
    cout << "\nOBJECTIVE: Reach level 5 and defeat the Necro Lord.\n";
}

void printDivider()
{
    cout << "------------------------------------------------------------\n";
}

void printSeparator()
{
    cout << "============================================================\n";
}

void printSmallBanner(const string &text)
{
    printDivider();
    cout << text << '\n';
    printDivider();
}

int getMapWidth()
{
    return MAP_W;
}

int getMapHeight()
{
    return MAP_H;
}

char getTile(int x, int y)
{
    if (!isInside(x, y))
        return '#';

    return worldMap[y][x];
}

void setTile(int x, int y, char tile)
{
    if (isInside(x, y))
        worldMap[y][x] = tile;
}

bool hasLineOfSight(const Player &p, int tx, int ty)
{
    int dx = abs(tx - p.x);
    int dy = abs(ty - p.y);

    return dx + dy <= 6;
}

int calculateTravelCost(int distance)
{
    return max(0, distance * 2);
}

bool canTravel(const Player &p, int distance)
{
    return p.energy >= calculateTravelCost(distance);
}

void travelEnergy(Player &p, int distance)
{
    p.energy = max(0, p.energy - calculateTravelCost(distance));
}

void discoverArea(Player &p)
{
    if (randomInt(1, 100) <= 10)
    {
        cout << "You discovered a new area.\n";
        p.xp += 10;
        p.score += 10;
        levelUp(p);
    }
}

void movementPostProcess(Player &p)
{
    discoverArea(p);
    awardExplorationXP(p);
    bossWarning(p);
}

void printSystemInfo()
{
    cout << "\nSYSTEM INFO\n";
    cout << "Map Size: " << getMapWidth() << " x " << getMapHeight() << '\n';
    cout << "Max Energy: " << MAX_ENERGY << '\n';
    cout << "Boss Level: " << getLevelRequirementForBoss() << '\n';
}

void showDeveloperMenu(Player &p)
{
    while (true)
    {
        clearScreen();
        printTitle();

        cout << "\nDEVELOPER INFORMATION\n";
        cout << "1. Player Debug\n";
        cout << "2. System Info\n";
        cout << "3. Weapon Power\n";
        cout << "4. Enemy Simulation\n";
        cout << "5. Back\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            debugPlayer(p);
            pauseGame();
        }
        else if (choice == 2)
        {
            printSystemInfo();
            pauseGame();
        }
        else if (choice == 3)
        {
            cout << "Total weapon damage: ";
            cout << totalWeaponDamage(p) << '\n';
            cout << "Full magazines: ";
            cout << countFullMagazines(p) << '\n';
            pauseGame();
        }
        else if (choice == 4)
        {
            Enemy e = createRandomEnemy(p);
            printEnemyDetails(e);
            displayBattleAdvice(p, e);
            pauseGame();
        }
        else if (choice == 5)
        {
            return;
        }
        else
        {
            cout << "Invalid option.\n";
            pauseGame();
        }
    }
}

void extendedHelp()
{
    clearScreen();
    printTitle();

    cout << "\nADVANCED GAMEPLAY\n";
    printDivider();
    cout << "Combat is turn-based, so think before every action.\n";
    cout << "Heavy Attack uses energy but ignores part of enemy defense.\n";
    cout << "Defend temporarily increases armor for the enemy's attack.\n";
    cout << "Random events happen while exploring.\n";
    cout << "The shop lets you improve your equipment.\n";
    cout << "Quests provide additional progression rewards.\n";
    cout << "The final boss has a large health pool and cannot be escaped.\n";

    pauseGame();
}

void showStatisticsMenu(const Player &p)
{
    while (true)
    {
        clearScreen();
        printTitle();

        cout << "\nSTATISTICS MENU\n";
        cout << "1. Player Statistics\n";
        cout << "2. Quest Summary\n";
        cout << "3. Progress\n";
        cout << "4. Back\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            showStats(p);
        }
        else if (choice == 2)
        {
            printQuestSummary();
            pauseGame();
        }
        else if (choice == 3)
        {
            printProgress(p);
            pauseGame();
        }
        else if (choice == 4)
        {
            return;
        }
        else
        {
            cout << "Invalid choice.\n";
            pauseGame();
        }
    }
}

void showWeaponShopInfo()
{
    cout << "\nWEAPON INFORMATION\n";
    printDivider();

    Weapon pistol{"Rusty Pistol", 18, 0, 30, 30};
    Weapon shotgun{"Shotgun", 32, 400, 12, 12};
    Weapon rifle{"Assault Rifle", 45, 800, 25, 25};
    Weapon plasma{"Plasma Gun", 70, 1500, 20, 20};

    printWeaponDetails(pistol);
    printWeaponDetails(shotgun);
    printWeaponDetails(rifle);
    printWeaponDetails(plasma);
}

void shopInformation()
{
    clearScreen();
    printTitle();
    showWeaponShopInfo();
    pauseGame();
}

void gameModeInfo()
{
    clearScreen();
    printTitle();

    cout << "\nGAME MODE\n";
    printDivider();
    cout << "Single Player Survival RPG\n";
    cout << "Turn Based Combat\n";
    cout << "Exploration\n";
    cout << "Quest Progression\n";
    cout << "Boss Battle\n";
    cout << "Persistent Save Files\n";

    pauseGame();
}

void welcomeSequence()
{
    clearScreen();
    printTitle();
    printWelcomeBanner();
    printGameVersion();
    printControlsShort();
    printObjective();
    pauseGame();
}

void gameSessionSummary(const Player &p)
{
    cout << "\nSESSION SUMMARY\n";
    printDivider();
    cout << "Player: " << p.name << '\n';
    cout << "Level: " << p.level << '\n';
    cout << "Kills: " << p.kills << '\n';
    cout << "Gold: " << p.gold << '\n';
    cout << "Score: " << p.score << '\n';
}

bool safeSave(Player &p)
{
    validatePlayer(p);
    return saveGame(p, "zombie_autosave.dat");
}

void autosave(Player &p)
{
    if (safeSave(p))
        cout << "Autosave complete.\n";
}

void autosaveNotice()
{
    cout << "The game periodically saves your progress.\n";
}

void printCombatTips()
{
    cout << "\nCOMBAT TIPS\n";
    cout << "Attack normally when conserving energy.\n";
    cout << "Use Heavy Attack when the enemy is dangerous.\n";
    cout << "Use grenades for burst damage.\n";
    cout << "Heal before your HP becomes critically low.\n";
    cout << "Reload when there is no ammo.\n";
}

void combatTraining()
{
    clearScreen();
    printTitle();
    printCombatTips();
    pauseGame();
}

void mapTraining()
{
    clearScreen();
    printTitle();

    cout << "\nMAP TRAINING\n";
    printDivider();
    printMapLegend();
    cout << "\nWalls cannot be crossed.\n";
    cout << "The B tile contains the final boss.\n";

    pauseGame();
}

void questTraining()
{
    clearScreen();
    printTitle();

    cout << "\nQUEST TRAINING\n";
    printDivider();
    cout << "Quests update automatically as you progress.\n";
    cout << "Completing quests grants gold and XP.\n";

    pauseGame();
}

void tutorial()
{
    while (true)
    {
        clearScreen();
        printTitle();

        cout << "\nTUTORIAL\n";
        cout << "1. Map Tutorial\n";
        cout << "2. Combat Tutorial\n";
        cout << "3. Quest Tutorial\n";
        cout << "4. Back\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
            mapTraining();
        else if (choice == 2)
            combatTraining();
        else if (choice == 3)
            questTraining();
        else if (choice == 4)
            return;
        else
        {
            cout << "Invalid choice.\n";
            pauseGame();
        }
    }
}

void mainMenuExtra()
{
    while (true)
    {
        clearScreen();
        printTitle();

        cout << "\nMORE\n";
        cout << "1. Tutorial\n";
        cout << "2. Shop Information\n";
        cout << "3. Game Mode\n";
        cout << "4. Back\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
            tutorial();
        else if (choice == 2)
            shopInformation();
        else if (choice == 3)
            gameModeInfo();
        else if (choice == 4)
            return;
        else
        {
            cout << "Invalid choice.\n";
            pauseGame();
        }
    }
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    while (true)
    {
        clearScreen();
        printTitle();
        printMainMenu();

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            newGame();
        }
        else if (choice == 2)
        {
            loadedGame();
        }
        else if (choice == 3)
        {
            printInstructions();
        }
        else if (choice == 4)
        {
            printCredits();
        }
        else if (choice == 5)
        {
            clearScreen();
            cout << "Thanks for playing Zombie Frontier!\n";
            break;
        }
        else
        {
            cout << "Invalid option.\n";
            pauseGame();
        }
    }

    return 0;
}
