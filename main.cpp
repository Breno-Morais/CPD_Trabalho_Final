#include "./parser.hpp"
#include "estruturas.hpp"

using namespace std;

hashtable<int,Player> players_table(100000);
hashtable<int,vector<Avaliacao>> user_ratings_table(500000);
hashtable<string,vector<int>> position_table(21);

void printPlayer(int id);
void quickSort(int *arr, int low, int high);
void quickSortAv(Avaliacao *arr, int low, int high);
void user_search(int n);
void position_search(int n, std::string pos);

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

        vector<string> player_positions = split(temp.positions,", ");

        for(string pos: player_positions)
            position_table.insere_array<int>(pos, temp.fifa_id);

    }

    std::ifstream fRating("rating.csv");
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

        temp->rcount += 1;
        user_ratings_table.insere_array<Avaliacao>(user_id, {player_id, rating});
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
        }
        else if(token == "player")
        {
            TriePlayers->possibleSufixos(comando);
        }
        else if(token.substr(0,3) == "top")
        {
            token.erase(0,3);
            int n = std::atoi(token.c_str());
            if((comando.at(0) == '\'') && (comando.at(comando.length()-1) == '\''))
                position_search(n, comando.substr(1, comando.length()-2));
        }
        else if(token == "end")
            return 0;
    }

    return 0;
}

using namespace std;

void printPlayer(int id)
{
    Player temp = players_table[id]->value;
    std::cout << id << ", " << temp.name << ", [" << temp.positions << "] ," << temp.rating << ", " << temp.rcount << std::endl;
}

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

void quickSortAv(Avaliacao *arr, int low, int high)
{
    int i = low;
    int j = high;
    float pivot = arr[(i + j) / 2].nota;
    Avaliacao temp;

    while (i <= j)
    {
        while (arr[i].nota < pivot)
            i++;
        while (arr[j].nota > pivot)
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
        quickSortAv(arr, low, j);
    if (i < high)
        quickSortAv(arr, i, high);
}

void user_search(int n)
{
    if(user_ratings_table[n] == NULL)
    {
        std::cout << "USUARIO NAO ENCONTRADO"  << std::endl;
        return;
    }
    unsigned int vec_size = user_ratings_table[n]->value.size();
    unsigned int true_size;
    if(vec_size > 20)
        true_size = 20;
    else
        true_size = vec_size;

    Avaliacao top20[true_size];

    for(unsigned int i = 0; i < true_size; i++)
    {
        top20[i] = user_ratings_table[n]->value[i];
    }
    quickSortAv(top20, 0, true_size-1);

    for(unsigned int i = true_size; i < vec_size; i++)
    {
        float temp = user_ratings_table[n]->value[i].nota;
        float last_rating = top20[0].nota;
        if(temp > last_rating)
        {
            top20[0] = user_ratings_table[n]->value[i];
            quickSortAv(top20, 0, true_size-1);
        }
    }

    std::cout << "sofifa_id, name, positions, global_rating, count, rating" << std::endl;
    for(int i = true_size - 1; i >= 0; i--)
    {
        Player temp = players_table[top20[i].id_player]->value;
        std::cout << temp.fifa_id << ", " << temp.name << ", [" << temp.positions << "] ," << temp.rating << ", " << temp.rcount << ", " << top20[i].nota <<std::endl;
    }
}

void position_search(int n, std::string pos)
{
    if(position_table[pos] == NULL)
    {
        std::cout << "POSICAO NAO ENCONTRADA"  << std::endl;
        return;
    }
    unsigned int vec_size = position_table[pos]->value.size();
    unsigned int true_size;
    if(vec_size > n)
        true_size = n;
    else
        true_size = vec_size;

    int topN[true_size];
    int i = 0;
    int inc = 0;
    int valor_de_corte = 1000;
    while((i < true_size) && (i + inc < vec_size))
    {
        if(players_table[position_table[pos]->value[i + inc]]->value.rcount >= valor_de_corte)
        {
            topN[i] = players_table[position_table[pos]->value[i + inc]]->value.fifa_id;
            i++;
        }
        else
            inc++;
    }
    if(i == 0) // Se ele não achou ninguem com mais de 1000 avaliações, sai fora
        return;

    true_size = i - 1;
    quickSort(topN, 0, true_size);

    i = true_size;
    while(i < vec_size)
    {
        if(players_table[position_table[pos]->value[i]]->value.rcount >= valor_de_corte)
        {
            float temp = players_table[position_table[pos]->value[i]]->value.rating;
            float last_rating = players_table[topN[0]]->value.rating;
            if(temp > last_rating)
            {
                topN[0] = players_table[position_table[pos]->value[i]]->value.fifa_id;
                quickSort(topN, 0, true_size);
            }
        }
        i++;
    }

    std::cout << "sofifa_id, name, positions, global_rating, count" << std::endl;
    for(int i = true_size; i >= 0; i--)
        printPlayer(topN[i]);
}

