#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#pragma warning(disable:4996)
#define MAX_C 7
#define MAX_P 20
#define ALL 1000
#define MAX_HAND 100
#define MAX_CARDS 600

using namespace std;

struct Cards
{
    int value;
    char colour[MAX_C];
};

struct Players
{
    Cards Hand[MAX_HAND];
    Cards Deck[MAX_HAND];
    int front[MAX_C];
    int handsize;
    int decksize;
    int score;
};

struct Piles
{
    Cards Card[MAX_HAND];
    int pilesize;
};

int compare_function(const void* a, const void* b)
{
    return (*(int*)a - *(int*)b);
}

void data_enter(int* n, int* num, int* threshold, int* g, int* gv, int* o, int* packsize, int* r); //wprowadzanie danych z konsoli
void arrays_zero(Players p[], int n, int num, Piles pile[]); //zerowanie tablic

int green_check(Cards card[], const char* colours[], int cardcounter); //sprawdzanie poprawnosci wartosci kart zielonych
int cards_numbers(int howmanyc[], const char* colours[], int num); //sprawdzanie poprawnosci ilosci kart
int cards_value(Cards card[], const char* colours[], int cardcounter, int howmanyc[], int goodcheck, int num);//sprawdzenie poprawnosci wartosci kart

void gamestate_ok(int cardcounter, const char* colours[], int num, int n, int active, int threshold, Players p[], Piles pile[], Cards card[], int howmanyc[]);//sprawdzenie ogolne poprawnosci parametrow gry

int same_colour_diff_piles(int num, Piles pile[], const char* colours[]); //sprawdzenie czy jeden kolor nie wystepuje na 2 roznych kociolkach

void load_game(Players p[], Piles pile[], const char* colours[], Cards card[], int* active, int* n, int* threshold, char filename[], FILE* file);//wczytanie stanu gry;

void pile_explosion(int irem, Players p[], Piles pile[], int threshold, int active); //sprawdzanie czy kociolek powinien wybuchnac
void game_state(int active, int n, int threshold, int num, Players p[], Piles pile[], char filename[], FILE* file); //wypisanie stanu gry
void simple_move(int* active, Piles pile[], Players p[], const char* colours[], int num, int n, int threshold); //wykonanie prostego posuniecia
void game_over(Players p[], int n, const char* colours[], int num); //koniec rozgrywki
void new_deck(int num, int g, int gv, int o, const char* colours[], Cards card[]);//tworzenie nowej talii
void cards_shuffle(Cards card[], int packsize); //tasowanie kart
void cards_deal(Cards card[], Players p[], int n, int packsize); //rozdanie kart graczom
void low_card(int* active, Piles pile[], Players p[], const char* colours[], int num, int n, int threshold); //zagranie niskiej karty




int main()
{
    const char* colours[MAX_C] = { "green","blue","red","violet","yellow","white","black" };
    int n, num, threshold, g, gv, o, packsize, r;

    char filename[50] = {};
    FILE* file;
    cout << "Podaj nazwe pliku tekstowego: \n";
    cin >> filename;
    if ((file = fopen(filename, "w")) == NULL)
    {
        cout << "Nie udalo sie otworzyc pliku!\n";
        exit(EXIT_FAILURE);
    }
    fclose(file);

    data_enter(&n, &num, &threshold, &g, &gv, &o, &packsize, &r);
    Cards* card;
    card = new Cards[packsize];
    Players* p;
    p = new Players[n + 2];
    Piles* pile;
    pile = new Piles[num + 1];
    int* points;
    points = new int[n + 2]();

    new_deck(num, g, gv, o, colours, card);

    for (int i = 0; i < r; i++)
    {
        int active = 1;
        arrays_zero(p, n, num, pile);
        cards_shuffle(card, packsize);
        cards_deal(card, p, n, packsize);
        game_state(active, n, threshold, num, p, pile, filename, file);
        load_game(p, pile, colours, card, &active, &n, &threshold, filename, file);
        while (p[active].handsize != 0)
        {
            Sleep(500);
            low_card(&active, pile, p, colours, num, n, threshold);
            //simple_move(&active, pile, p, colours, num, n, threshold);
            game_state(active, n, threshold, num, p, pile, filename, file);
            load_game(p, pile, colours, card, &active, &n, &threshold, filename, file);
        }
        game_over(p, n, colours, num);
        Sleep(3000);
        for (int j = 1; j <= n; j++)
        {
            points[j] += p[j].score;
        }
    }
    cout << "\n";
    cout << "KONIEC GRY!\n";
    for (int j = 1; j <= n; j++)
    {
        cout << "Wynik gracza " << j << " = " << points[j] << "\n";
    }

    delete[] points;
    delete[] p;
    delete[] pile;
    delete[] card;

    return 0;
}



void load_game(Players p[], Piles pile[], const char* colours[], Cards card[], int* active, int* n, int* threshold, char filename[], FILE* file)//////////////////////////////////////////////////////////////////////////
{
    int a = 0, c = 0, cardcounter = 0, num = 0; //zmienne pomocnicze opisuj¹ce miejsce w tabeli z odpowiedni¹ kart¹
    int howmanyc[MAX_C] = { 0 };
    char linia[ALL] = { '0' };
    char* token;
    if ((file = fopen(filename, "r")) == NULL)
    {
        cout << "Nie udalo sie otworzyc pliku!\n";
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 3; i++)
    {
        fgets(linia, ALL, file);
        token = strtok(linia, " :\n");
        while (token != NULL)
        {
            if (atoi(token) > 0)
            {
                if (i == 0)
                    *active = atoi(token);
                else if (i == 1)
                    (*n) = atoi(token);
                else
                    *threshold = atoi(token);
            }
            token = strtok(NULL, " :\n");
        }
    }

    for (int i = 1; i <= (*n); i++)
    {
        p[i].handsize = 0;
        p[i].decksize = 0;
        //wczytanie do kart na rece
        fgets(linia, ALL, file);
        token = strtok(linia, " :\n");
        while (token != NULL)
        {
            if (atoi(token) > 0 && c > 1)
            {
                p[i].Hand[a].value = atoi(token);
                card[cardcounter].value = atoi(token);
                a++;
            }
            else if (atoi(token) == 0 && c > 4)
            {
                strcpy(p[i].Hand[p[i].handsize].colour, token);
                p[i].handsize++;
                strcpy(card[cardcounter].colour, token);
                cardcounter++;
                for (int j = 0; j < MAX_C; j++)
                {
                    if (strcmp(token, colours[j]) == 0)
                    {
                        howmanyc[j]++;
                        break;
                    }
                }
            }
            token = strtok(NULL, " :\n");
            c++;
        }
        a = 0;
        c = 0;
        //wczytanie do kart przed graczem
        fgets(linia, ALL, file);
        token = strtok(linia, " :\n");
        while (token != NULL)
        {
            if (atoi(token) > 0 && c > 1)
            {
                p[i].Deck[a].value = atoi(token);
                card[cardcounter].value = atoi(token);
                a++;
            }
            else if (atoi(token) == 0 && c > 4)
            {
                strcpy(p[i].Deck[p[i].decksize].colour, token);
                p[i].decksize++;
                strcpy(card[cardcounter].colour, token);
                cardcounter++;
                for (int j = 0; j < MAX_C; j++)
                {
                    if (strcmp(token, colours[j]) == 0)
                    {
                        if (p[*active].handsize == 0)
                            p[i].front[j]++;
                        howmanyc[j]++;
                        break;
                    }
                }
            }
            token = strtok(NULL, " :\n");
            c++;
        }
        a = 0;
        c = 0;
        /*cout << i << " " << "player has " << p[i].handsize << " cards on hand\n";
        cout << i << " " << "player has " << p[i].decksize << " cards in front of him\n";*/ //wypisywanie do zadania 3ab
    }

    for (int j = 1; j < MAX_C; j++)
    {
        if (howmanyc[j] > 0)
            num++;
    }

    //wczytanie kart na stosy
    for (int i = 1; i <= num; i++)
    {
        pile[i].pilesize = 0;
        fgets(linia, ALL, file);
        token = strtok(linia, " :\n");
        while (token != NULL)
        {
            if (atoi(token) > 0 && c > 1)
            {
                pile[i].Card[a].value = atoi(token);
                card[cardcounter].value = atoi(token);
                a++;
            }
            else if (atoi(token) == 0 && c > 3)
            {
                strcpy(pile[i].Card[pile[i].pilesize].colour, token);
                pile[i].pilesize++;
                strcpy(card[cardcounter].colour, token);
                cardcounter++;
                for (int j = 0; j < MAX_C; j++)
                {
                    if (strcmp(token, colours[j]) == 0)
                    {
                        if (howmanyc[j] == 0)
                            num++;
                        howmanyc[j]++;
                        break;
                    }
                }
            }
            token = strtok(NULL, " :\n");
            c++;
        }
        //cout << "there are " << pile[i].pilesize << " cards on "<<i<<" pile\n"; ////wypisywanie do zadania 3b
        a = 0;
        c = 0;
    }
    gamestate_ok(cardcounter, colours, num, *n, *active, *threshold, p, pile, card, howmanyc);
}



void data_enter(int* n, int* num, int* threshold, int* g, int* gv, int* o, int* packsize, int* r)//////////////////////////////////////////////////////////////////////////////////////////
{
    cout << "Podaj liczbe graczy: \n";
    cin >> (*n);
    if ((*n) < 2 || (*n) > 20)
    {
        cout << "Nieprawidlowa liczba graczy!";
        exit(EXIT_FAILURE);
    }

    cout << "Podaj liczbe kociolkow: \n";
    cin >> (*num);
    if ((*num) < 1 || (*num) > 6)
    {
        cout << "Nieprawidlowa liczba kociolkow!";
        exit(EXIT_FAILURE);
    }

    cout << "Podaj wartosc po przekroczeniu ktorej nastepuje eksplozja kociolka: \n";
    cin >> (*threshold);
    if ((*threshold) < 1 || (*threshold) > 2000000000)
    {
        cout << "Nieprawidlowa wartosc!";
        exit(EXIT_FAILURE);
    }

    cout << "Podaj liczbe zielonych kart w talii: \n";
    cin >> (*g);
    if ((*g) < 1 || (*g) > 2000000000)
    {
        cout << "Nieprawidlowa liczba zielonych kart!";
        exit(EXIT_FAILURE);
    }

    cout << "Podaj wartosc zielonych kart w talii: \n";
    cin >> (*gv);
    if ((*gv) < 0 || (*gv) > 2000000000)
    {
        cout << "Nieprawidlowa wartosc zielonych kart!";
        exit(EXIT_FAILURE);
    }

    cout << "Podaj liczbe kart kazdego koloru: \n";
    cin >> (*o);
    if ((*o) < 1 || (*o) > 2000000000)
    {
        cout << "Nieprawidlowa liczba kart!";
        exit(EXIT_FAILURE);
    }
    cout << "Podaj liczbe rund: \n";
    cin >> (*r);
    if ((*r) < 1 || (*r) > 2000000000)
    {
        cout << "Nieprawidlowa liczba rund!";
        exit(EXIT_FAILURE);
    }

    *packsize = (*g) + ((*num) * (*o));
    if ((*packsize) < (*n))
    {
        cout << "Za malo kart na ta ilosc graczy!";
        exit(EXIT_FAILURE);
    }

}



void new_deck(int num, int g, int gv, int o, const char* colours[], Cards card[])///////////////////////////////////////////////////////////////////////////////////////////////////
{
    int count = 0;
    int* ov;
    ov = new int[o];
    cout << "Podaj wartosci kart kazdego koloru: \n";
    for (int i = 0; i < o; i++)
    {
        cin >> ov[i];
    }

    for (int i = 0; i <= num; i++)
    {
        if (i == 0)
            for (int j = 0; j < g; j++)
            {
                card[j].value = gv;
                strcpy(card[j].colour, colours[0]);
                //cout << card[j].value << " " << card[j].colour << " "; //zadanie 1stos
            }
        else
        {
            for (int j = g + (i - 1) * o; j < (g + (i - 1) * o) + o; j++)
            {
                card[j].value = ov[count];
                strcpy(card[j].colour, colours[i]);
                //cout << card[j].value << " " << card[j].colour << " "; //zadanie 1stos
                if (count < o - 1)
                    count++;
                else
                    count = 0;
            }

        }
    }
    delete[] ov;
}



void game_state(int active, int n, int threshold, int num, Players p[], Piles pile[], char filename[], FILE* file)//////////////////////////////////////////////////////////////////////////////////////////////
{
    if ((file = fopen(filename, "w")) == NULL)
    {
        cout << "Nie udalo sie otworzyc pliku!\n";
        exit(EXIT_FAILURE);
    }
    cout << "active player = " << active << "\n";
    fprintf(file, "active player = %d \n", active);
    cout << "players number = " << n << "\n";
    fprintf(file, "players number = %d \n", n);
    cout << "explosion threshold = " << threshold << "\n";
    fprintf(file, "explosion threshold = %d \n", threshold);

    for (int i = 1; i <= n; i++)
    {
        cout << i << " player hand cards: ";
        fprintf(file, "%d player hand cards: ", i);
        if (p[i].handsize > 0)
        {
            for (int j = 0; j < p[i].handsize; j++)
            {
                if (p[i].Hand[j].value > 0 && j == p[i].handsize - 1)
                {
                    cout << p[i].Hand[j].value << " " << p[i].Hand[j].colour << "\n";
                    fprintf(file, "%d %s\n", p[i].Hand[j].value, p[i].Hand[j].colour);
                }
                else if (p[i].Hand[j].value > 0)
                {
                    cout << p[i].Hand[j].value << " " << p[i].Hand[j].colour << " ";
                    fprintf(file, "%d %s ", p[i].Hand[j].value, p[i].Hand[j].colour);
                }
                else if (j == p[i].handsize - 1)
                {
                    cout << "\n";
                    fprintf(file, "\n");
                }
            }
        }
        else
        {
            cout << "\n";
            fprintf(file, "\n");
        }
        cout << i << " player deck cards: ";
        fprintf(file, "%d player deck cards: ", i);

        if (p[i].decksize > 0)
        {
            for (int j = 0; j < p[i].decksize; j++)
            {
                if (p[i].Deck[j].value > 0 && j == p[i].decksize - 1)
                {
                    cout << p[i].Deck[j].value << " " << p[i].Deck[j].colour << "\n";
                    fprintf(file, "%d %s\n", p[i].Deck[j].value, p[i].Deck[j].colour);
                }
                else if (p[i].Deck[j].value > 0)
                {
                    cout << p[i].Deck[j].value << " " << p[i].Deck[j].colour << " ";
                    fprintf(file, "%d %s ", p[i].Deck[j].value, p[i].Deck[j].colour);
                }
                else if (j == p[i].decksize - 1)
                {
                    cout << "\n";
                    fprintf(file, "\n");
                }
            }
        }
        else
        {
            cout << "\n";
            fprintf(file, "\n");
        }
    }
    for (int i = 1; i <= num; i++)
    {
        cout << i << " pile cards: ";
        fprintf(file, "%d pile cards: ", i);
        if (pile[i].pilesize > 0)
        {
            for (int j = 0; j < pile[i].pilesize; j++)
            {
                if (pile[i].Card[j].value > 0 && j == pile[i].pilesize - 1)
                {
                    cout << pile[i].Card[j].value << " " << pile[i].Card[j].colour << "\n";
                    fprintf(file, "%d %s\n", pile[i].Card[j].value, pile[i].Card[j].colour);
                }
                else if (pile[i].Card[j].value > 0)
                {
                    cout << pile[i].Card[j].value << " " << pile[i].Card[j].colour << " ";
                    fprintf(file, "%d %s ", pile[i].Card[j].value, pile[i].Card[j].colour);
                }
                else if (j == pile[i].pilesize - 1)
                {
                    cout << "\n";
                    fprintf(file, "\n");
                }
            }
        }
        else
        {
            cout << "\n";
            fprintf(file, "\n");
        }
    }
    fclose(file);
}



void arrays_zero(Players p[], int n, int num, Piles pile[])///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    for (int i = 1; i <= n; i++)
    {
        p[i].handsize = 0;
        p[i].decksize = 0;
        for (int j = 0; j <= num; j++)
        {
            p[i].front[j] = 0;
        }
    }
    for (int i = 1; i <= num; i++)
    {
        pile[i].pilesize = 0;
    }
}



int green_check(Cards card[], const char* colours[], int cardcounter)///////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    //int greencounter = 0; //zadanie 4a stos
    int flag = 0;
    for (int i = 0; i < cardcounter; i++)
    {
        if (strcmp(card[i].colour, colours[0]) == 0 && flag == 0)
        {
            flag = card[i].value;
            //greencounter++;
        }
        else if (strcmp(card[i].colour, colours[0]) == 0)
        {
            //greencounter++;
            if (card[i].value != flag)
            {
                cout << "Different green cards values occurred\n";
                return 0;
            }
        }
    }
    return 1;
    /*if (flag != 0)
            cout << "Found " << greencounter << " green cards, all with " << flag << " value\n";
    else
            cout << "Green cards does not exist\n";*/ //zad 4a stos
}



int cards_numbers(int howmanyc[], const char* colours[], int num)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    int flag = 0;
    for (int i = 1; i <= num; i++)
    {
        if (flag == 0)
        {
            flag = howmanyc[i];
        }
        else
        {
            if (howmanyc[i] != flag)
            {
                /*cout << "At least two colors with a different number of cards were found: \n";
                for (int j = 1; j <= num; j++)
                {
                                cout << colours[j] << " cards are " << howmanyc[j] << "\n";
                }*/
                return 0;
            }
        }
    }
    //cout << "The number cards of all colors is equal: " << howmanyc[1] << "\n";
    return 1;
}



int cards_value(Cards card[], const char* colours[], int cardcounter, int howmanyc[], int goodcheck, int num)////////////////////////////////////////////////////////////////////////
{
    int m = 0;
    int valcheck[MAX_C][MAX_HAND] = { 0 };
    for (int i = 1; i <= num; i++)
    {
        for (int j = 0; j < cardcounter; j++)
        {
            if (strcmp(card[j].colour, colours[i]) == 0)
            {
                valcheck[i][m] = card[j].value;
                m++;
            }
        }
        m = 0;
        qsort(&valcheck[i], howmanyc[i], sizeof(int), compare_function);
    }
    if (goodcheck == 1)
    {
        for (int i = 0; i < howmanyc[1]; i++)
        {
            int flag = 0;
            for (int j = 1; j <= num; j++)
            {
                if (flag == 0)
                    flag = valcheck[j][i];
                else if (valcheck[j][i] != flag)
                {
                    cout << "The values of cards of all colors are not identical:\n";
                    for (int k = 1; k <= num; k++)
                    {
                        cout << colours[k] << " cards values: ";
                        for (int l = 0; l < howmanyc[k]; l++)
                        {
                            if (l == howmanyc[l] - 1)
                            {
                                cout << valcheck[k][l] << "\n";
                            }
                            else
                                cout << valcheck[k][l] << " ";
                        }
                    }
                    return 0;
                }

            }
        }
        return 1;
        /*cout << "The values of cards of all colors are identical:\n";
        for (int i = 0; i < howmanyc[1]; i++)
        {
                cout << valcheck[1][i] << " ";
        }*/
    }
    else
    {
        cout << "The values of cards of all colors are not identical:\n";
        for (int i = 1; i <= num; i++)
        {
            cout << colours[i] << " cards values: ";
            for (int j = 0; j < howmanyc[i]; j++)
            {
                if (j == howmanyc[i] - 1)
                {
                    cout << valcheck[i][j] << "\n";
                }
                else
                    cout << valcheck[i][j] << " ";
            }
        }
        return 0;
    }
}



void gamestate_ok(int cardcounter, const char* colours[], int num, int n, int active, int threshold, Players p[], Piles pile[], Cards card[], int howmanyc[])////////////////////////////////
{
    int handflag = 0;
    int pileflag = 0;
    int explflag = 0;
    int rest = cardcounter % n;

    int greencheck = green_check(card, colours, cardcounter);
    int goodcheck = cards_numbers(howmanyc, colours, num);
    int cardsvalue = cards_value(card, colours, cardcounter, howmanyc, goodcheck, num);

    //sprawdzenie poprawnosci ilosci kart na rece graczy
    if (active <= rest)
    {
        for (int i = 1; i < active; i++)
        {
            if (p[active].handsize - p[i].handsize != 1)
            {
                cout << "The number of players cards on hand is wrong\n";
                handflag = 1;
                break;
            }
        }
        if (handflag == 0)
        {
            for (int i = active + 1; i <= rest; i++)
            {
                if (p[i].handsize != p[active].handsize)
                {
                    cout << "The number of players cards on hand is wrong\n";
                    handflag = 1;
                    break;
                }
            }
        }
        if (handflag == 0)
        {
            for (int i = rest + 1; i <= n; i++)
            {
                if (p[active].handsize - p[i].handsize != 1)
                {
                    cout << "The number of players cards on hand is wrong\n";
                    handflag = 1;
                    break;
                }
            }
        }
    }
    else
    {
        for (int i = 1; i <= rest; i++)
        {
            if (p[active].handsize != p[i].handsize)
            {
                cout << "The number of players cards on hand is wrong\n";
                handflag = 1;
                break;
            }
        }
        if (handflag == 0)
        {
            for (int i = rest + 1; i < active; i++)
            {
                if (p[active].handsize - p[i].handsize != 1)
                {
                    cout << "The number of players cards on hand is wrong\n";
                    handflag = 1;
                    break;
                }
            }
        }
        if (handflag == 0)
        {
            for (int i = active + 1; i <= n; i++)
            {
                if (p[active].handsize != p[i].handsize)
                {
                    cout << "The number of players cards on hand is wrong\n";
                    handflag = 1;
                    break;
                }
            }
        }
    }

    //sprawdzenie poprawnosci kolorow na stosach
    for (int i = 1; i <= num; i++)
    {
        char colourflag[7] = {};
        for (int j = 0; j < pile[i].pilesize; j++)
        {
            if (j == 0)
            {
                strcpy(colourflag, pile[i].Card[j].colour);
            }
            else if (strcmp(colourflag, colours[0]) == 0)
            {
                strcpy(colourflag, pile[i].Card[j].colour);
            }
            else if (strcmp(pile[i].Card[j].colour, colours[0]) == 0);
            else if (strcmp(colourflag, pile[i].Card[j].colour) != 0)
            {
                cout << "Two different colors were found on the " << i << " pile\n";
                pileflag = 1;
                break;
            }
        }
    }
    //sprawdzenie poprawnosci kolorow miedzy roznymi kociolkami
    int samecolourdiffpiles = same_colour_diff_piles(num, pile, colours);

    //sprawdzenie poprawnosci wybuchu kociolka
    for (int i = 1; i <= num; i++)
    {
        int sum = 0;
        for (int j = 0; j < pile[i].pilesize; j++)
        {
            if (sum + pile[i].Card[j].value > threshold)
            {
                cout << "Pile number " << i << " should explode earlier\n";
                explflag = 1;
                break;
            }
            else
                sum += pile[i].Card[j].value;
        }
    }

    if (handflag == 0 && pileflag == 0 && explflag == 0 && samecolourdiffpiles == 1 && cardsvalue == 1 && greencheck == 1)
    {
        //cout << "Current state of the game is ok\n"; //wypis zad6 stos
    }
    else
    {
        cout << "ERROR!!";
        exit(EXIT_FAILURE);
    }
}



int same_colour_diff_piles(int num, Piles pile[], const char* colours[])////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    int* colourdiffpiles = new int[num + 1]();
    for (int i = 1; i <= num; i++)
    {
        int colflag = 0;
        for (int j = 0; j < pile[i].pilesize; j++)
        {
            for (int k = 1; k <= num; k++)
            {
                if (strcmp(pile[i].Card[j].colour, colours[k]) == 0)
                {
                    colourdiffpiles[k]++;
                    if (colourdiffpiles[k] > 1)
                    {
                        //cout << "At least one colour was found on different piles\n";
                        delete[] colourdiffpiles;
                        return 0;
                    }
                    colflag = 1;
                    break;
                }
            }
            if (colflag == 1)
                break;
        }
    }
    delete[] colourdiffpiles;
    return 1;
}



void pile_explosion(int irem, Players p[], Piles pile[], int threshold, int active)/////////////////////////////////////////////////////////////////////////////////////////////////
{
    int sum = 0;
    for (int i = 0; i < pile[irem].pilesize; i++)
    {
        sum += pile[irem].Card[i].value;
    }
    if (sum > threshold)
    {
        int oldsize, j = 0;
        oldsize = p[active].decksize;
        p[active].decksize += pile[irem].pilesize;
        for (int i = oldsize; i < p[active].decksize; i++)
        {
            p[active].Deck[i].value = pile[irem].Card[j].value;
            strcpy(p[active].Deck[i].colour, pile[irem].Card[j].colour);
            pile[irem].Card[j].value = -1;
            strcpy(pile[irem].Card[j].colour, "");
            j++;
        }
        pile[irem].pilesize = 0;
    }

}



void simple_move(int* active, Piles pile[], Players p[], const char* colours[], int num, int n, int threshold)////////////////////////////////////////////////////////////////////////
{
    int irem = 0;
    //sprawdzenie czy 1wsza karta jest zielona
    if (strcmp(p[*active].Hand[0].colour, colours[0]) == 0)
    {
        pile[1].Card[pile[1].pilesize].value = p[*active].Hand[0].value;
        strcpy(pile[1].Card[pile[1].pilesize].colour, p[*active].Hand[0].colour);
        pile[1].pilesize++;
        irem = 1;
    }
    else
    {
        int flag = 0;
        for (int i = 1; i <= num; i++)
        {
            for (int j = 0; j < pile[i].pilesize; j++)
            {
                if (strcmp(p[*active].Hand[0].colour, pile[i].Card[j].colour) == 0) //sprawdzenie czy kolor karty ma juz swoj stos
                {
                    pile[i].Card[pile[i].pilesize].value = p[*active].Hand[0].value;
                    strcpy(pile[i].Card[pile[i].pilesize].colour, p[*active].Hand[0].colour);
                    pile[i].pilesize++;
                    irem = i;
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
                break;
        }
        if (flag == 0) //jesli nie ma stosu tego koloru wrzucamy do pierwszego mozliwego
        {
            for (int i = 1; i <= num; i++)
            {
                int sflag = 0;
                for (int j = 0; j < pile[i].pilesize; j++)
                {
                    if (strcmp(colours[0], pile[i].Card[j].colour) != 0)
                    {
                        sflag = 1;
                        break;
                    }
                }
                if (sflag == 0)
                {
                    pile[i].Card[pile[i].pilesize].value = p[*active].Hand[0].value;
                    strcpy(pile[i].Card[pile[i].pilesize].colour, p[*active].Hand[0].colour);
                    pile[i].pilesize++;
                    irem = i;
                    break;
                }
            }
        }
    }
    p[*active].Hand[0].value = -1;
    strcpy(p[*active].Hand[0].colour, "");

    pile_explosion(irem, p, pile, threshold, *active);

    if ((*active) + 1 <= n)
        (*active)++;
    else
        (*active) = 1;
}



void game_over(Players p[], int n, const char* colours[], int num)////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    for (int i = 1; i <= num; i++)
    {
        int maxi = 0, flag = 0, jrem = 0;
        for (int j = 1; j <= n; j++)
        {
            if (p[j].front[i] > maxi)
            {
                maxi = p[j].front[i];
                jrem = j;
                flag = 1;
            }
            else if (p[j].front[i] == maxi)
            {
                flag = 0;
            }
        }
        if (flag == 1)
        {
            cout << "Na kolor " << colours[i] << " odporny jest gracz " << jrem << "\n";
            p[jrem].front[i] = 0;
        }
    }
    for (int i = 1; i <= n; i++)
    {
        p[i].score = 0;
        for (int j = 0; j <= num; j++)
        {
            if (j == 0)
            {
                p[i].score += (p[i].front[j] * 2);
            }
            else
                p[i].score += p[i].front[j];
        }
        cout << "Wynik gracza " << i << " = " << p[i].score << "\n";
    }

}



void cards_shuffle(Cards card[], int packsize)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    Cards buffer;
    int random;
    srand(time(NULL));
    for (int i = 0; i < packsize; i++)
    {
        random = rand() % packsize;
        buffer.value = card[i].value;
        strcpy(buffer.colour, card[i].colour);
        card[i].value = card[random].value;
        strcpy(card[i].colour, card[random].colour);
        card[random].value = buffer.value;
        strcpy(card[random].colour, buffer.colour);
    }
}



void cards_deal(Cards card[], Players p[], int n, int packsize)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    int player = 1;

    for (int i = 0; i < packsize; i++)
    {
        p[player].Hand[p[player].handsize].value = card[i].value;
        strcpy(p[player].Hand[p[player].handsize].colour, card[i].colour);
        p[player].handsize++;
        if (player + 1 <= n)
            player++;
        else
            player = 1;
    }
}



void low_card(int* active, Piles pile[], Players p[], const char* colours[], int num, int n, int threshold)////////////////////////////////////////////////////////////////////////
{
    int irem = 0, mini = 2000000000, ifmore = 0, crem = 0, greencheck = 0;
    for (int i = 0; i < p[*active].handsize; i++)
    {
        if (p[*active].Hand[i].value < mini)
        {
            mini = p[*active].Hand[i].value;
            ifmore = 0;
            crem = i;
            if (strcmp(p[*active].Hand[i].colour, colours[0]) == 0)
            {
                greencheck = 1;
            }
            else
            {
                greencheck = 0;
            }
        }
        else if (p[*active].Hand[i].value == mini)
        {
            if (greencheck == 1)
            {
                ifmore = 0;
            }
            else if (strcmp(p[*active].Hand[i].colour, colours[0]))
            {
                ifmore = 0;
                greencheck = 1;
                crem = i;
            }
            else
                ifmore = 1;
        }
    }
    if (ifmore == 0)//jesli nie ma wiecej kart o najmniejszej wartosci to ruszamy ta najmniejsza
    {
        if (strcmp(p[*active].Hand[crem].colour, colours[0]) == 0)//jesli jest zielona
        {
            int lowestpile = 2000000000;
            for (int i = 1; i <= num; i++)
            {
                if (pile[i].pilesize == 0)
                {
                    irem = i;
                    break;
                }
                else
                {
                    int sum = 0;
                    for (int j = 0; j < pile[i].pilesize; j++)
                    {
                        sum += pile[i].Card[j].value;
                    }
                    if (sum < lowestpile)
                    {
                        lowestpile = sum;
                        irem = i;
                    }
                }
            }
        }
        else
        {
            int flag = 0;
            for (int i = 1; i <= num; i++)
            {
                for (int j = 0; j < pile[i].pilesize; j++)
                {
                    if (strcmp(p[*active].Hand[crem].colour, pile[i].Card[j].colour) == 0) //sprawdzenie czy kolor karty ma juz swoj stos
                    {
                        irem = i;
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                    break;
            }
            if (flag == 0) //jesli nie ma stosu tego koloru wrzucamy do pierwszego mozliwego
            {
                for (int i = 1; i <= num; i++)
                {
                    int sflag = 0;
                    for (int j = 0; j < pile[i].pilesize; j++)
                    {
                        if (strcmp(colours[0], pile[i].Card[j].colour) != 0)
                        {
                            sflag = 1;
                            break;
                        }
                    }
                    if (sflag == 0)
                    {
                        irem = i;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        int lowestpile = 2000000000;
        int flag = 0;
        for (int i = 0; i < p[*active].handsize; i++)
        {
            if (mini == p[*active].Hand[i].value)
            {
                for (int j = 1; j <= num; j++)
                {
                    int sum = 0;
                    for (int k = 0; k < pile[j].pilesize; k++)
                    {
                        sum += pile[j].Card[k].value;
                        if (strcmp(p[*active].Hand[i].colour, pile[j].Card[k].colour) == 0) //sprawdzenie czy kolor karty ma juz swoj stos
                        {
                            flag = 1;
                        }
                    }
                    if (flag == 1 && sum < lowestpile)
                    {
                        lowestpile = sum;
                        irem = j;
                        crem = i;
                    }
                }
                if (flag == 0) //jesli nie ma stosu tego koloru wrzucamy do pierwszego mozliwego
                {
                    for (int j = 1; j <= num; j++)
                    {
                        int sum = 0;
                        int sflag = 0;
                        for (int k = 0; k < pile[j].pilesize; k++)
                        {
                            if (strcmp(colours[0], pile[j].Card[k].colour) != 0)
                            {
                                sflag = 1;
                                break;
                            }
                            else
                            {
                                sum += pile[j].Card[k].value;
                            }
                        }
                        if (sflag == 0)
                        {
                            if (sum < lowestpile)
                            {
                                crem = i;
                                irem = j;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    pile[irem].Card[pile[irem].pilesize].value = p[*active].Hand[crem].value;
    strcpy(pile[irem].Card[pile[irem].pilesize].colour, p[*active].Hand[crem].colour);
    pile[irem].pilesize++;

    p[*active].Hand[crem].value = -1;
    strcpy(p[*active].Hand[crem].colour, "");

    pile_explosion(irem, p, pile, threshold, *active);

    if ((*active) + 1 <= n)
        (*active)++;
    else
        (*active) = 1;
}