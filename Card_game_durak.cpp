#include <iostream>
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

// Глобальная колода
Cards deck36[9];
Cards deck52[13];
char suit[4] = { '\3', '\4', '\6', '\5' };
int fall_array_id = 0;
int bot_def_id;

//Карты, вышедшие из игры
char fall_array[52][2];

//6 карт у бота и игрока
Hand bot_hand[6];
Hand user_hand[6];

void CreateDeck36()
{
    // Создание колоды 36 карт
    const int size = 9;
    char card_low_to_high[size] = { '6', '7', '8', '9', 'X', 'J', 'Q', 'K', 'A' };

    for (int i = 0, j = 6; i < size; i++, j++)
    {
        deck36[i].card = card_low_to_high[i];
        deck36[i].power = j;
    }

    /*for (int i = 0; i < size; i++)
    {
        cout << deck36[i].card << " " << deck36[i].power << endl;
    }*/
}

void CreateDeck52()
{
    // Создание колоды 52 карт
    const int size = 13;
    char card_low_to_high[size] = { '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K', 'A' };

    for (int i = 0, j = 2; i < size; i++, j++)
    {
        deck52[i].card = card_low_to_high[i];
        deck52[i].power = j;
    }

    /*for (int i = 0; i < size; i++)
    {
        cout << deck52[i].card << " " << deck52[i].power << endl;
    }*/
}

//Меню
bool Menu()
{
    int choice;
    cout << "██████╗░██╗░░░██╗██████╗░░█████╗░██╗░░██╗\n██╔══██╗██║░░░██║██╔══██╗██╔══██╗██║░██╔╝\n██║░░██║██║░░░██║██████╔╝███████║█████═╝░\n██║░░██║██║░░░██║██╔══██╗██╔══██║██╔═██╗░\n██████╔╝╚██████╔╝██║░░██║██║░░██║██║░╚██╗\n╚═════╝░░╚═════╝░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝" << endl;

    while (true)
    {
        cout << "Выберите сложность! \n1 - 36 карт \n2 - 52 карты \nВаш выбор -------> " << endl;
        cin >> choice;

        if (choice == 1) {
            return true;
        }

        else if (choice == 2) {
            return false;
        }

        else {
            cout << "Неверный выбор!" << endl;
        }
    }
}

//Проверка на совпадающие карты
bool Fall_check(char card_hand, char suit_hand, bool easy_mode, int full_deck)
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
    }

    return false;

}

//Раздача карт
void Selection_card(Hand hands[6], bool easy_mode, int full_deck, char trump_suit, int drop_card)
{
    int start = 0;
    int end = (easy_mode) ? 8 : 12;
    for (int i = 6 - drop_card; i < 6; i++)
    {
        bool fall_check_bad = true;
        while (true)
        {
            int r = rand() % (end - start + 1) + start;
            char card_hand = (easy_mode) ? deck36[r].card : deck52[r].card;
            char suit_hand = suit[rand() % 4];
            int power_hand = (easy_mode) ? deck36[r].power : deck52[r].power;
            fall_check_bad = Fall_check(card_hand, suit_hand, easy_mode, full_deck);

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
int Low_Trump(Hand hands[6], char trump_suit)
{
    int low_trump = 99;
    for (int i = 0; i < 6; i++)
    {
        if (trump_suit == hands[i].s)
        {
            low_trump = hands[i].p;
            break;
        }
    }
    for (int i = 0; i < 6; i++)
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
    int low_trump_bot = Low_Trump(bot_hand, trump_suit);
    //Поиск минимального козыря у игрока
    int low_trump_user = Low_Trump(user_hand, trump_suit);

    if (low_trump_bot < low_trump_user) {
        cout << "Первый ход за ботом!" << endl;
        return false;
    }
    else if (low_trump_user < low_trump_bot) {
        cout << "Первый ход за игроком!" << endl;
        return true;
    }
    return false;
}

//Сдвиг карт
void Card_move(Hand hands[6], int card_id, int drop_card)
{
    for (int i = card_id; i < 6; i++)
    {
        char temp_c = hands[i + 1].c;
        char temp_s = hands[i + 1].s;
        int temp_p = hands[i + 1].p;

        hands[i].c = temp_c;
        hands[i].s = temp_s;
        hands[i].p = temp_p;
    }
    hands[5].c = ' ';
    hands[5].s = ' ';
    hands[5].p = 0;
}

//Бот выбирает карту
void BotChooseCard(Hand bot_hands[6], Hand user_hands[6], int drop_card_bot, int card_id, char trump_suit)
{
    int bot_low_defence = 0;
    for (int i = 0; i < 6; i++)
    {
        if (bot_hands[i].p > user_hands[card_id].p and bot_hands[i].s == user_hands[card_id].s or bot_hands[i].s == trump_suit) {
            bot_def_id = i;
            bot_low_defence = bot_hands[i].p;
            break;
        }
    }

    for (int i = 0; i < 6; i++)
    {
        if (bot_hands[i].p > user_hands[card_id].p and bot_hands[i].p < bot_low_defence and bot_hands[i].s == user_hands[card_id].s) {
            bot_def_id = i;
            bot_low_defence = bot_hands[i].p;
        }
    }

    if (bot_low_defence == 0 or bot_low_defence < user_hands[card_id].p) {
        cout << "Бот не может покрыться." << endl;
        return;
    }

    cout << "Бот покрылся: " << bot_hands[bot_def_id].c << " " << bot_hands[bot_def_id].s << endl;
    drop_card_bot++;

    Card_move(bot_hand, bot_def_id, drop_card_bot);
}

//Сама игра
void Game(bool easy_mode)
{
    int start = 0;
    int end = (easy_mode) ? 8 : 12;
    int drop_card_user = 6, drop_card_bot = 6;

    int full_deck = (easy_mode) ? 36 : 52;
    cout << "\6 \3 Да начнётся игра! \5 \4" << endl << endl;

    char trump_suit = suit[rand() % 4];
    cout << "Козырная масть: " << trump_suit << endl;

    //Подбор карт для бота
    Selection_card(bot_hand, easy_mode, full_deck, trump_suit, drop_card_bot);

    //Подбор карт для игрока
    cout << "Ваши карты: " << endl;
    Selection_card(user_hand, easy_mode, full_deck, trump_suit, drop_card_user);

    drop_card_user = 0;
    drop_card_bot = 0;

    for (int i = 0; i < 6; i++)
    {
        cout << i + 1 << " - " << user_hand[i].c << " " << user_hand[i].s << endl;
    }
    /*cout << endl;
    for (int i = 0; i < 6; i++)
    {
        cout << i + 1 << " - " << bot_hand[i].c << " " << bot_hand[i].s << " " << bot_hand[i].p << endl;
    }*/
    //Проверка хода
    bool user_turn = UserTurn(trump_suit);

    if (user_turn) {
        int card_id;

        while (drop_card_user != 6)
        {
            cout << "Выберите карту, которой вы будете ходить... " << endl;
            cin >> card_id;

            if (user_hand[card_id - 1].s != '\3' and user_hand[card_id - 1].s != '\4' and user_hand[card_id - 1].s != '\6' and user_hand[card_id - 1].s != '\5')
            {
                cout << "Карта отсутствует." << endl;
                continue;
            }

            cout << "Вы сходили: " << user_hand[card_id - 1].c << " " << user_hand[card_id - 1].s << endl;
            drop_card_user++;

            //Бот защищается
            BotChooseCard(bot_hand, user_hand, drop_card_bot, card_id - 1, trump_suit);

            //Вызов функции по сдвигу
            Card_move(user_hand, card_id - 1, drop_card_user);


            for (int i = 0; i < 6; i++) {
                cout << i + 1 << " - " << user_hand[i].c << " " << user_hand[i].s << endl;
            }
            /*cout << "Карты бота: " << endl;

            for (int i = 0; i < 6; i++) {
                cout << i + 1 << " - " << bot_hand[i].c << " " << bot_hand[i].s << " " << bot_hand[i].p << endl;
            }*/
        }
    }
    /*else if (!user_turn)
    {

    }*/
}




int main()
{
    setlocale(0, "");
    srand(time(nullptr));

    bool easy_mode = Menu();

    if (easy_mode) {
        CreateDeck36();
    }

    else if (!easy_mode) {
        CreateDeck52();
    }

    Game(easy_mode);
    cout << endl;
    /*for (int i = 0; i < 52; i++)
    {
        cout << fall_array[i][0] << " " << fall_array[i][1] << endl;
    }*/
    return 0;
}