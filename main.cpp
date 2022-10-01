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
    string positions; // Posi��es do Jogador
    float rating; // M�dia de Avalia��o
    int rcount; // N�mero de Avalia��es
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
                if(key == x->key) // Se o elemento j� existe
                    return;   // N�o faz nada
            }
            Node<Tkey,Tvalue> myNode = {.key = key, .value = value};
            table[hashI].push_front(myNode);
        }

        void insere_array(Tkey key, int value)
        {
            unsigned int hashI = hashfunction(key);

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x){
                if(key == x->key) // Se o elemento j� existe
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

            for(auto x = table[hashI].begin(); x != table[hashI].end(); ++x)
            {
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

hashtable<int,Player> players_table(18944 * 5);
hashtable<int,vector<int>> user_ratings_table;
hashtable<string,vector<int>> position_table(21);

void printPlayer(int id)
{
    Player temp = players_table[id]->value;
    std::cout << id << ", " << temp.name << ", [" << temp.positions << "] ," << temp.rating << ", " << temp.rcount << std::endl;
}

class Trie
{
private:
        class Trie_Node
        {
            public:
                Trie_Node *children[ALPHABET_SIZE];

                bool isWord;
                int id;

                Trie_Node()
                {
                    for(int i = 0; i < ALPHABET_SIZE; i++)
                    {
                        children[i] = NULL;
                    }

                    isWord = false;
                    id = -1;
                }

                ~Trie_Node()
                {
                    for(int i = 0; i < ALPHABET_SIZE; i++)
                        if(children[i])
                        {
                            delete children[i];
                            children[i] = NULL;
                        }

                    delete this;
                }
        };

        bool isLastNode(Trie_Node *current)
        {
            for(int i = 0; i < ALPHABET_SIZE; i++)
                if(current->children[i])
                    return false;
            return true;
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

    public:
        Trie_Node *root;
        int Total_Words;

        Trie()
        {
            root = new Trie_Node();
            Total_Words = 0;
        }

        void InsertWord(string& str, int id)
        {
            Trie_Node *current = root;
            for(unsigned int i = 0; i < str.size(); i++)
            {
                int c = ItoC(str[i]);

                if(current->children[c] == NULL)
                {
                    current->children[c] = new Trie_Node();
                }

                current = (current->children[c]);
            }

            current->isWord = true;
            current->id = id;
        }

        bool SearchWord(string& str)
        {
            Trie_Node *current = root;

            for(unsigned int i = 0; i < str.size(); i++)
            {
                if(current->children[ItoC(str[i])] == NULL)
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

        void possibleSufixos(std::string& prefixo)
        {
            Trie_Node *current = root;
            int index;

            for(unsigned int i = 0; i < prefixo.length(); i++)
            {
                index = ItoC(prefixo.at(i));
                if(current->children[index] == NULL) 
                {
                    cout << "Não foi encontrado nenhum jogador." << endl;
                    return;
                }
                current = current->children[index];
            }

            caminha(prefixo, current);

            return;
        }

        void caminha(std::string prefixo, Trie_Node* current)
        {
            if(current->isWord)
                printPlayer(current->id);

            for(int i = 0; i < ALPHABET_SIZE; ++i)
            {
                if(current->children[i])
                    caminha(prefixo + CtoI(i), current->children[i]);
            }
        }
};

void quickSort(int *arr, int low, int high)
{
    int i = low;
    int j = high;
    float pivot = players_table[arr[(i + j) / 2]]->value.rating;
    int temp;

    while (i <= j)
    {
        while (players_table[arr[i]]->value.rating < pivot)
            i++;
        while (players_table[arr[j]]->value.rating > pivot)
            j--;
        if (i <= j)
        {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    if (j > low)
        quickSort(arr, low, j);
    if (i < high)
        quickSort(arr, i, high);
}


void user_search(int n)
{
    unsigned int vec_size = user_ratings_table[n]->value.size();
    unsigned int true_size;
    int top20[20];
    if(vec_size > 20)
        true_size = 20;
    else
        true_size = vec_size;

    for(int i = 0; i < true_size; i++)
    {
        top20[i] = players_table[user_ratings_table[n]->value[i]]->value.fifa_id;
    }
    quickSort(top20, 0, vec_size-1);

    for(int i = true_size; i < vec_size; i++)
    {
        float temp = players_table[user_ratings_table[n]->value[i]]->value.rating;
        float last_rating = players_table[top20[19]]->value.rating;
        if(temp > last_rating)
        {
            top20[19] = players_table[user_ratings_table[n]->value[i]]->value.fifa_id;
            quickSort(top20, 0, vec_size-1);
        }
    }

    for(unsigned int i = 0; i < true_size; i++)
        printPlayer(top20[i]);
}

vector<string> str_tokenizer(string s, char del)
{
    vector<string> vector_s;
    stringstream ss(s);
    string word;
    while (!ss.eof()) 
    {
        getline(ss, word, del);
        vector_s.push_back(word);
    }

    return vector_s;
}

using namespace aria::csv;

int main() {
    Trie *TriePlayers = new Trie();

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

        vector<string> player_positions = str_tokenizer(temp.positions, ',');
        for(string pos : player_positions)
            position_table.insere_array(pos, temp.fifa_id);

    }

    std::ifstream fRating("minirating.csv");
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

        int i1 = (temp->rcount) + 1;

        if(temp->rcount == 0)
            temp->rating = rating;
        else
            temp->rating = temp->rating + (1.0f/i1)*(rating - temp->rating);

        temp->rating = 0;

        temp->rcount += 1;
        user_ratings_table.insere_array(user_id, player_id);
    }

    while(true)
    {
        std::cout << "& ";
        std::string comando;
        std::getline(std::cin, comando);
        std::string delimiter = " ";

        size_t pos = comando.find(delimiter);
        std::string token;

        token = comando.substr(0, pos);
        comando.erase(0, pos + delimiter.length());

        if(token == "user")
        {
            user_search(std::atoi(comando.c_str()));
        } else if(token == "player")
        {
            TriePlayers->possibleSufixos(comando);
        } else if(token == "end")
            return 0;
    }

    return 0;
}

