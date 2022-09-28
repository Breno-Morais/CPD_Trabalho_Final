#include <iostream>
#include "./parser.hpp"
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <forward_list>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <iterator>
#include <algorithm>
#include <memory>

using namespace std;

struct Player{
    int fifa_id; // Id do Jogador
    string name; // Nome do Jogador
    string positions; // Posições do Jogador
    float rating; // Média de Avaliação
    int rcount; // Número de Avaliações
};

template <class Tkey, class Tvalue>
struct Node
{
    Tkey key;
    Tvalue value;
};

template <class Tkey, class Tvalue>
class hashtable
{
    private:
        forward_list<Node<Tkey, Tvalue>> *table;
        unsigned int hashfunction(int id)
        {
            return id % M;
        }
        unsigned int hashfunction(string id)
        {
            int hashI = 0;
            for(int i = 0; i < (int)id.length(); i++)
                hashI = (31 * hashI + id.at(i)) % M;

            return hashI;
        }
    public:
        int M; // Tamanho

        hashtable(int T_size = 1)
        {
            M = T_size;
            table = new forward_list<Node<Tkey, Tvalue>>[M];
        }

        ~hashtable()
        {
            delete[] table;
        }

        void insere(Tkey key, Tvalue value)
        {
            unsigned int hashI = hashfunction(key);

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x){
                if(key == x->key) // Se o elemento já existe
                    return;   // Não faz nada
            }
            Node<Tkey,Tvalue> myNode = {.key = key, .value = value};
            table[hashI].push_front(myNode);
        }

        void insere_array(Tkey key, int value)
        {
            unsigned int hashI = hashfunction(key);

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x){
                if(key == x->key) // Se o elemento já existe
                {
                    x->value.push_back(value);
                    return;
                }
            }
            Node<Tkey,Tvalue> myNode = {.key = key, .value = std::vector<int>{value}};
            table[hashI].push_front(myNode);
        }

        Node<Tkey, Tvalue>* operator[](Tkey key)
        {
            int hashI = hashfunction(key);

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x){
                if(key == x->key)
                {
                    return &(*x);
                }
            }

            Node<Tkey,Tvalue> myNode;
            return &myNode;
        }
};

template <typename T>
struct range_t
{
    T b, e;
    range_t(T x, T y) : b(x), e(y) {}
    T begin()
    {
        return b;
    }
    T end()
    {
        return e;
    }
};

template <typename T>
range_t<T> range(T b, T e)
{
    return range_t<T>(b, e);
}

vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        unsigned int index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            break;
        }
    }
    return result;
}

template<typename T>
void printVector(const T& t) {
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
}

int charAt(string str, int d)
{
    if (int(str.size()) <= d)
        return -1;
    else
        return (str.at(d))-65;
}

#define ALPHABET_SIZE 31


void printPlayer(int id, hashtable<int,Player> players)
{
    Player temp = players[id]->value;
    std::cout << id << ", " << temp.name << ", [" << temp.positions << "] ," << temp.rating << ", " << temp.rcount << std::endl;
}


class Trie_Node
{
    public:
        shared_ptr<Trie_Node> children[ALPHABET_SIZE];

        bool isWord;
        int id;

        Trie_Node()
        {
            for(int i = 0; i < ALPHABET_SIZE; i++)
            {
                children[i] = shared_ptr<Trie_Node>();
            }

            isWord = false;
            id = -1;
        }
};

class Trie
{
    public:
        shared_ptr<Trie_Node> root;
        int Total_Words;

        Trie()
        {
            root = shared_ptr<Trie_Node>(new Trie_Node());
            Total_Words = 0;
        }

        ~Trie()
        {
        }

        void InsertWord(string& str, int id)
        {
            shared_ptr<Trie_Node> current = root;
            for(int i = 0; i < str.size(); i++)
            {
                int c = ItoC(str[i]);

                /* Debugação
                if(c < 0)
                std::cout << "i: " << i << ", str:" << str << ", str[i]:" << str[i] << ", ItoC: " << c << std::endl;
                */

                if(current->children[c] == shared_ptr<Trie_Node>(nullptr))
                {
                    current->children[c] = shared_ptr<Trie_Node>(new Trie_Node());
                }

                current = (current->children[c]);
            }

            current->isWord = true;
            current->id = id;
        }

        bool SearchWord(string& str)
        {
            shared_ptr<Trie_Node> current = root;

            for(int i = 0; i < str.size(); i++)
            {
                if(current->children[ItoC(str[i])] == shared_ptr<Trie_Node>(nullptr))
                    return false;

                current = (current->children[ItoC(str[i])]);
            }

            if(current->isWord == true)
            {
                std::cout << "\nID: " << current->id << std::endl;
                return true;
            }

            return false;
        }

        int printQuery(const string query)
        {
            shared_ptr<Trie_Node> current = root;
            for(char c: query)
            {
                int ind = ItoC(c);

                if(!current->children[ind])
                    return 0;

                current = current->children[ind];
            }

            if(isLastNode(current))
            {
                std::cout << query << std::endl;
                return -1;
            }

            suggestionsRec(current, query);
            return 1;
        }

        void printPlayerQuery(hashtable<int,Player> players, const string query)
        {
            shared_ptr<Trie_Node> current = root;
            for(char c: query)
            {
                int ind = ItoC(c);

                if(!current->children[ind])
                    return;

                current = current->children[ind];
            }

            if(isLastNode(current))
            {
                printPlayer(current->id, players);
                return;
            }

            queryPlayer(current, query, players);
            return;
        }

private:

        bool isLastNode(shared_ptr<Trie_Node> current)
        {
            for(int i = 0; i < ALPHABET_SIZE; i++)
                if(current->children[i])
                    return false;
            return true;
        }

        void suggestionsRec(shared_ptr<Trie_Node> current, string currPrefix)
        {
            if(current->isWord)
            std::cout << currPrefix << std::endl;

            for(int i = 0; i < ALPHABET_SIZE; i++)
            {
                if(current->children[i])
                {
                    char child = 'a' + i;
                    suggestionsRec(current->children[i], currPrefix + child);
                }
            }
        }

        void queryPlayer(shared_ptr<Trie_Node> current, string currPrefix, hashtable<int,Player> players)
        {
            if(current->isWord)
                printPlayer(current->id, players);

            for(int i = 0; i < ALPHABET_SIZE; i++)
            {
                //std::cout << "CUR: " << current << " CH: " << current->children[i] << " i: " << i << std::endl;
                if(current->children[i])
                {
                    char child = CtoI(i);
                    queryPlayer(current->children[i], currPrefix + child, players);
                }
            }
        }

        int ItoC(char c)
        {
            switch(c)
            {
                case ' ':
                    return 26;
                    break;

                case '\'':
                    return 27;
                    break;

                case '-':
                    return 28;
                    break;

                case '.':
                    return 29;
                    break;

                case '"':
                    return 30;
                    break;

                default:
                    return static_cast<int>(std::tolower(c) - 'a');
            }
        }

        char CtoI(int i)
        {
            switch(i)
            {
                case 26:
                    return ' ';
                    break;

                case 27:
                    return '\'';
                    break;

                case 28:
                    return '-';
                    break;

                case 29:
                    return '.';
                    break;

                case 30:
                    return '"';
                    break;

                default:
                    return static_cast<int>('a' + i);
            }
        }
};

using namespace aria::csv;

int main() {
    hashtable<int,Player> players_table(18944 * 5);
    hashtable<int,vector<int>> user_ratings_table;
    hashtable<string,vector<int>> position_table(21);
    shared_ptr<Trie> TriePlayers(new Trie());

        /*
        if(comp == -1)
            std::cout << "Nenhuma outra palavra encontrada\n";
        else if(comp == 0)
            std::cout << "Nenhuma palavra encontrada\n";
        */

    std::ifstream fPlayer("players.csv");
    CsvParser parser(fPlayer);

    bool ignore_first = true;
    for (auto& row : parser) {
        if(ignore_first)
        {
            ignore_first = false;
            continue;
        }
        Player temp;
        temp.fifa_id = std::atoi(row[0].c_str());
        temp.name = row[1];
        temp.positions = row[2];
        temp.rating = 0.0f;
        temp.rcount = 0;

        players_table.insere(temp.fifa_id,temp);

        TriePlayers->InsertWord(temp.name, temp.fifa_id);

        vector<string> player_positions = split(temp.positions,",");
        for(string pos: player_positions)
            position_table.insere_array(pos, temp.fifa_id);

    }

    std::ifstream fRating("miniminirating.csv");
    CsvParser parser1(fRating);

    ignore_first = true;
    for (auto& row : parser1) {
        if(ignore_first)
        {
            ignore_first = false;
            continue;
        }
        unsigned int user_id = std::atoi(row[0].c_str());
        unsigned int player_id = std::atoi(row[1].c_str());
        float rating = std::atof(row[2].c_str());;
        Player* temp = &(players_table[player_id]->value);

        int i = temp->rcount;
        int i1 = (temp->rcount) + 1;

        if(temp->rcount == 0)
            temp->rating = rating;
        else
            temp->rating = temp->rating + (1.0f/i1)*(rating - temp->rating);

        temp->rcount += 1;
        user_ratings_table.insere_array(user_id, player_id);
    }

    while(true)
    {
        string nome;
        std::cout << "Pesquise jogadores: ";
        std::cin >> nome;

        TriePlayers->printPlayerQuery(players_table, nome);
    }

    return 0;
}

