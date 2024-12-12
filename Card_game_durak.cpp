#include <iostream>
#include <windows.h>
using namespace std;


// Структура карты
struct Cards
{
    char card;
    int power;
};

struct Hand
{
    char c;
    char s;
    int p;
};

//Глобальная колода
Cards deck36[9];
Cards deck52[13];
char suit[4] = { '\3', '\4', '\6', '\5' };
int fall_array_id = 0, bot_def_id, desk_attack_id, drop_card_user = 6, drop_card_bot = 6, full_deck, bot_atk_id, newSize_bot = 6, newSize_user = 6;
bool took = false, is_game_on = true, user_turn;

//Карты, вышедшие из игры
char fall_array[52][2];

//6 карт у бота и игрока
Hand* bot_hand = new Hand[6];
Hand* user_hand = new Hand[6];
Hand desk_attack[13];

void CreateDeck36()
{
    // Создание колоды 36 карт
    const int size = 9;
    char card_low_to_high[size] = { '6', '7', '8', '9', 'X', 'J', 'Q', 'K', 'A' };

    for (int i = 0, j = 6; i < size; i++, j++) {
        deck36[i].card = card_low_to_high[i];
        deck36[i].power = j;
    }
    full_deck = 36;
}

void CreateDeck52()
{
    // Создание колоды 52 карт
    const int size = 13;
    char card_low_to_high[size] = { '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K', 'A' };

    for (int i = 0, j = 2; i < size; i++, j++) {
        deck52[i].card = card_low_to_high[i];
        deck52[i].power = j;
    }
    full_deck = 52;
}

//Меню
bool Menu()
{
    int choice;
    cout << "██████╗░██╗░░░██╗██████╗░░█████╗░██╗░░██╗\n██╔══██╗██║░░░██║██╔══██╗██╔══██╗██║░██╔╝\n██║░░██║██║░░░██║██████╔╝███████║█████═╝░\n██║░░██║██║░░░██║██╔══██╗██╔══██║██╔═██╗░\n██████╔╝╚██████╔╝██║░░██║██║░░██║██║░╚██╗\n╚═════╝░░╚═════╝░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝" << endl;
    while (true)
    {
        cout << "Выберите сложность! \n1 - 36 карт \n2 - 52 карты \nВаш выбор -------> ";
        cin >> choice;

        if (choice == 1) {
            cout << "\6 \3 Да начнётся игра! \5 \4" << endl << endl;
            return true;
        }

        else if (choice == 2) {
            cout << "\6 \3 Да начнётся игра! \5 \4" << endl << endl;
            return false;
        }

        else {
            cout << "НЕВЕРНЫЙ ВЫБОР!" << endl;
        }
    }
}

//Проверка на совпадающие карты
bool Fall_check(char card_hand, char suit_hand, bool easy_mode)
{
    for (int i = 0; i < 52; i++)
    {
        if (card_hand == fall_array[i][0] and suit_hand == fall_array[i][1]) {
            return true;
        }

        else {
            continue;
        }
    }

    if (full_deck != 0) {
        full_deck--;
        fall_array[fall_array_id][0] = card_hand;
        fall_array[fall_array_id][1] = suit_hand;
        fall_array_id++;
    }

    else {
        cout << "\nКолода пуста!\n";
        return true;
    }
    return false;
}

//Раздача карт
void Selection_card(Hand hands[6], bool easy_mode, char trump_suit, int drop_card, int newSize)
{
    int start = 0;
    int end = (easy_mode) ? 8 : 12;
    for (int i = newSize - drop_card; i < newSize; i++)
    {
        bool fall_check_bad = true;
        while (true)
        {
            if (full_deck <= 0)
            {
                break;
            }
            int r = rand() % (end - start + 1) + start;
            char card_hand = (easy_mode) ? deck36[r].card : deck52[r].card;
            char suit_hand = suit[rand() % 4];
            int power_hand = (easy_mode) ? deck36[r].power : deck52[r].power;
            fall_check_bad = Fall_check(card_hand, suit_hand, easy_mode);

            if (!fall_check_bad)
            {
                hands[i].c = card_hand;
                hands[i].s = suit_hand;
                hands[i].p = power_hand;
                if (trump_suit == hands[i].s)
                {
                    hands[i].p += (easy_mode) ? 9 : 13;
                }
                break;
            }
        }
    }
}

//Поиск минимального козыря
int Low_Trump(Hand hands[6], char trump_suit, int newSize)
{
    int low_trump = 99;
    for (int i = 0; i < newSize; i++)
    {
        if (trump_suit == hands[i].s)
        {
            low_trump = hands[i].p;
            break;
        }
    }
    for (int i = 0; i < newSize; i++)
    {
        if (trump_suit == hands[i].s and low_trump > hands[i].p)
        {
            low_trump = hands[i].p;
        }
    }

    return low_trump;
}

//Кто ходит первым
bool UserTurn(char trump_suit)
{
    //Поиск минимального козыря у бота
    int low_trump_bot = Low_Trump(bot_hand, trump_suit, newSize_bot);
    //Поиск минимального козыря у игрока
    int low_trump_user = Low_Trump(user_hand, trump_suit, newSize_user);


    if (low_trump_bot < low_trump_user) {
        return false;
    }
    else if (low_trump_user < low_trump_bot) {
        return true;
    }
    else if (low_trump_user == low_trump_bot) {
        int r = rand() % 2;
        if (r == 1) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

//Сдвиг карт
void Card_move(Hand hands[6], int card_id, int drop_card, int newSize)
{
    for (int i = card_id; i < newSize; i++)
    {
        char temp_c = hands[i + 1].c;
        char temp_s = hands[i + 1].s;
        int temp_p = hands[i + 1].p;

        hands[i].c = temp_c;
        hands[i].s = temp_s;
        hands[i].p = temp_p;
    }
    hands[newSize - 1].c = ' ';
    hands[newSize - 1].s = ' ';
    hands[newSize - 1].p = 0;
}

//Бот выбирает карту
void BotChooseCard(Hand bot_hands[6], Hand user_hands[6], int card_id, char trump_suit)
{
    int bot_low_defence = 0;
    for (int i = 0; i < newSize_bot; i++)
    {
        if (bot_hands[i].p > user_hands[card_id].p and bot_hands[i].s == user_hands[card_id].s or bot_hands[i].s == trump_suit) {
            bot_def_id = i;
            bot_low_defence = bot_hands[i].p;
            break;
        }
    }

    for (int i = 0; i < newSize_bot; i++)
    {
        if (bot_hands[i].p > user_hands[card_id].p and bot_hands[i].p < bot_low_defence and bot_hands[i].s == user_hands[card_id].s) {
            bot_def_id = i;
            bot_low_defence = bot_hands[i].p;
        }
    }

    if (bot_low_defence == 0 or bot_low_defence < user_hands[card_id].p) {
        //Бот не может покрыться
        took = true;
        return;
    }

    cout << "Бот покрылся: " << bot_hands[bot_def_id].c << " " << bot_hands[bot_def_id].s << endl;
    desk_attack[desk_attack_id].c = bot_hands[bot_def_id].c;
    desk_attack[desk_attack_id].s = bot_hands[bot_def_id].s;
    desk_attack[desk_attack_id].p = bot_hands[bot_def_id].p;
    desk_attack_id++;
    drop_card_bot++;

    Card_move(bot_hand, bot_def_id, drop_card_bot, newSize_bot);
}

void BotAttack(Hand hands[6])
{
    int bot_low_attack = hands[0].p;
    for (int i = 0; i < newSize_bot; i++) {
        if (bot_low_attack > hands[i].p) {
            bot_low_attack = hands[i].p;
            bot_atk_id = i;
        }
    }

    cout << "Бот сходил: " << hands[bot_atk_id].c << " " << hands[bot_atk_id].s << endl;
    desk_attack[desk_attack_id].c = hands[bot_atk_id].c;
    desk_attack[desk_attack_id].s = hands[bot_atk_id].s;
    desk_attack[desk_attack_id].p = hands[bot_atk_id].p;
    desk_attack_id++;
    drop_card_bot++;

}

void UserDefence(char trump_suit)
{
    int user_def;
    cout << "0 - Взять карты со стола \nВыберите карту, которой вы будете крыться... ";
    cin >> user_def;

    if (user_def == 0)
    {
        cout << "Вы берете карты!";
        user_turn = false;
        Card_move(bot_hand, bot_atk_id, drop_card_bot, newSize_bot);
        return;
    }

    if (user_hand[user_def - 1].p > bot_hand[bot_atk_id].p and user_hand[user_def - 1].s == bot_hand[bot_atk_id].s or user_hand[user_def - 1].s == trump_suit)
    {
        cout << "Вы сходили: " << user_hand[user_def - 1].c << " " << user_hand[user_def - 1].s << endl;
        desk_attack[desk_attack_id].c = user_hand[user_def - 1].c;
        desk_attack[desk_attack_id].s = user_hand[user_def - 1].s;
        desk_attack[desk_attack_id].p = user_hand[user_def - 1].p;
        desk_attack_id++;
        drop_card_user++;

        user_turn = true;

        Card_move(user_hand, user_def - 1, drop_card_user, newSize_user);
        Card_move(bot_hand, bot_atk_id, drop_card_bot, newSize_bot);

        cout << "Карты бота: " << endl;
        for (int i = 0; i < newSize_bot; i++) {
            cout << i + 1 << " - " << bot_hand[i].c << " " << bot_hand[i].s << endl;
        }
    }

    else {
        cout << "Вы не можете покрыться выбранной картой! \n";
        UserDefence(trump_suit);
    }
}

//Сама игра
void Game(bool easy_mode)
{
    int start = 0;
    int end = (easy_mode) ? 8 : 12;
    full_deck = (easy_mode) ? 36 : 52;
    char trump_suit = suit[rand() % 4];

    //Проверка хода
    user_turn = UserTurn(trump_suit);

    //УДАЛИ ЭТО
    //user_turn = true;

    while (is_game_on)
    {
        cout << "Козырная масть: " << trump_suit << endl;
        if (!took) {
            //Подбор карт для бота
            Selection_card(bot_hand, easy_mode, trump_suit, drop_card_bot, newSize_bot);
        }
        else
        {
            took = false;
        }
        //Подбор карт для игрока
        Selection_card(user_hand, easy_mode, trump_suit, drop_card_user, newSize_user);
        cout << "Карт в колоде: " << full_deck << endl;
        cout << "Ваши карты: " << endl;

        drop_card_user = 0;
        drop_card_bot = 0;
        desk_attack_id = 0;

        for (int i = 0; i < newSize_user; i++)
        {
            cout << i + 1 << " - " << user_hand[i].c << " " << user_hand[i].s << endl;
        }

        if (user_turn) {
            cout << "Ход за игроком!" << endl;
            int card_id;

            //Цикл подкидывания карт
            while (drop_card_user != newSize_user)
            {
                int coincidence = 0;

                if (drop_card_user >= 1) {
                    cout << "0 - Бито" << endl;
                }
                cout << "Выберите карту, которой вы будете ходить... ";
                cin >> card_id;
                if (card_id == 0 and drop_card_user >= 1) {
                    system("cls");
                    cout << "Бито!" << endl;

                    //Ход переходит боту
                    user_turn = false;
                    break;
                }
                if ((user_hand[card_id - 1].s != '\3' and user_hand[card_id - 1].s != '\4' and user_hand[card_id - 1].s != '\6' and user_hand[card_id - 1].s != '\5') or card_id < 1 or card_id > 6) {
                    cout << "Карта отсутствует." << endl;
                    continue;
                }
                for (int i = 0; i < 13; i++) {
                    if (user_hand[card_id - 1].c == desk_attack[i].c) {
                        coincidence++;
                        break;
                    }
                }
                if (coincidence == 0 and drop_card_user >= 1) {
                    cout << "Такой карты нет на столе!" << endl;
                    continue;
                }

                cout << "Вы сходили: " << user_hand[card_id - 1].c << " " << user_hand[card_id - 1].s << endl;
                desk_attack[desk_attack_id].c = user_hand[card_id - 1].c;
                desk_attack[desk_attack_id].s = user_hand[card_id - 1].s;
                desk_attack[desk_attack_id].p = user_hand[card_id - 1].p;
                desk_attack_id++;
                drop_card_user++;

                //Бот защищается
                BotChooseCard(bot_hand, user_hand, card_id - 1, trump_suit);

                //Вызов функции по сдвигу
                Card_move(user_hand, card_id - 1, drop_card_user, newSize_user);

                for (int i = 0; i < 6; i++) {
                    cout << i + 1 << " - " << user_hand[i].c << " " << user_hand[i].s << endl;
                }

                /*cout << "Карты на столе ";
                for (int i = 0; i < 13; i++){
                    cout << desk_attack[i].c << " ";
                }
                cout << endl;*/

                if (took) {
                    system("cls");
                    cout << "Бот не может покрыться." << endl;
                    cout << "Поэтому он берет карты со стола!" << endl;
                    cout << "Карты бота: " << endl;

                    int act = newSize_bot - drop_card_bot;
                    newSize_bot = act + desk_attack_id;
                    Hand* newpointer = new Hand[newSize_bot];

                    for (int i = 0, j = 0; i < newSize_bot; i++)
                    {
                        if (i < act) {
                            newpointer[i].c = bot_hand[i].c;
                            newpointer[i].s = bot_hand[i].s;
                            newpointer[i].p = bot_hand[i].p;
                        }
                        else {
                            newpointer[i].c = desk_attack[j].c;
                            newpointer[i].s = desk_attack[j].s;
                            newpointer[i].p = desk_attack[j].p;
                            j++;
                        }
                    }

                    delete[] bot_hand;
                    bot_hand = newpointer;

                    for (int i = 0; i < newSize_bot; i++) {
                        cout << i + 1 << " - " << bot_hand[i].c << " " << bot_hand[i].s << endl;
                    }
                    user_turn = true;
                    break;
                }
            }

            //Уничтожение карт в desk_attack
            for (int i = 0; i < 13; i++)
            {
                desk_attack[i].c = ' ';
                desk_attack[i].s = ' ';
                desk_attack[i].p = 0;
            }
        }
        else if (!user_turn)
        {
            cout << "Ход за ботом!" << endl;
            BotAttack(bot_hand);
            UserDefence(trump_suit);
        }
    }
}




int main()
{
    setlocale(0, "");
    srand(time(nullptr));

    system("Color E4");

    bool easy_mode = Menu();

    if (easy_mode) {
        CreateDeck36();
    }

    else if (!easy_mode) {
        CreateDeck52();
    }

    Game(easy_mode);
    cout << endl;
    for (int i = 0; i < 52; i++)
    {
        cout << fall_array[i][0] << " " << fall_array[i][1] << endl;
    }
    delete[] bot_hand;
    delete[] user_hand;
    return 0;
}