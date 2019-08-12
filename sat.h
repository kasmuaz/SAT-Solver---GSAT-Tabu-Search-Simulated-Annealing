#ifndef SAT_H_INCLUDED
#define SAT_H_INCLUDED

#include <vector>
#include <fstream>
#include <cmath>
using namespace std;


class Formula
{
public:
    int broj_varijabli;
    int broj_klauza;
    vector<vector<int>> klauze;

    Formula(string ime_fajla);
    Formula(const Formula& f);
    int prebrojZadovoljeneKlauze(vector<bool> dodjela);
    void ispisiFormulu();

};

Formula::Formula(string ime_fajla)
{
    ifstream unos(ime_fajla);
    string header;
    unos >> header;
    unos >> header;
    unos >> broj_varijabli;
    unos >> broj_klauza;
    while(!unos.eof())
    {
        vector<int> temp;
        int t;
        while(true)
        {
            unos >> t;
            unos.get();
            if(t == 0)
                break;
            else
                temp.push_back(t);
        }
        klauze.push_back(temp);
    }
}

Formula::Formula(const Formula& f)
{
    broj_klauza = f.broj_klauza;
    broj_varijabli = f.broj_varijabli;
    klauze = f.klauze;
}

void Formula::ispisiFormulu()
{
    cout << "Broj varijabli: " << broj_varijabli << "\nBroj klauza: " << broj_klauza << endl;

    for(int i = 0; i < klauze.size(); i++)
    {
        vector<string> vars;
        for(int j = 0; j < klauze[i].size(); j++)
        {
            string ime_var = "";
            if(klauze[i][j] < 0)
                ime_var = "(-)x" + to_string(-1*klauze[i][j]);
            else
                ime_var = "x" + to_string(klauze[i][j]);

            vars.push_back(ime_var);
        }
        cout << "(";
        for(int k = 0; k < vars.size(); k++)
        {
            cout << vars[k];
            if( k != vars.size()-1)
                cout << " + ";
        }
        cout << ")\n";
    }
}

int Formula::prebrojZadovoljeneKlauze(vector<bool> dodjela)
{
    int brojZadovoljenih = 0;

    for(int i = 0; i < klauze.size(); i++)
    {
        for(int j = 0; j < klauze[i].size(); j++)
        {
            if(klauze[i][j] > 0)
            {
                if(dodjela[klauze[i][j]])
                {
                    brojZadovoljenih++;
                    break;
                }
            }
            else
            {
                if(!dodjela[-1*klauze[i][j]])
                {
                    brojZadovoljenih++;
                    break;
                }
            }
        }
    }
    return brojZadovoljenih;
}

double randomDouble()
{
    int broj = rand()%100;
    double f = (double)(broj)/99.f;
    return f;
}

void  generisiRandomDodjeluVarijabli(vector<bool>& dodjela)
{
    for (int i = 1; i < dodjela.size(); i++)
    {
        if (randomDouble() > 0.5)
        {
            dodjela[i] = true;
        }
        else
        {
            dodjela[i] = false;
        }
    }
}

vector<bool> generisiSljedecuNajbolju(vector<bool>& dodjela, Formula& f)
{

    vector<bool> najbolja = dodjela;
    int brojTacnihNajbolja = 0;

    for (int i = 1; i < dodjela.size(); i++)
    {
        vector<bool> novaDodjela = dodjela;
        novaDodjela[i] = !novaDodjela[i];
        int brojTacnihNovaDodjela = f.prebrojZadovoljeneKlauze(novaDodjela);
        if(brojTacnihNovaDodjela > brojTacnihNajbolja)
        {
            najbolja = novaDodjela;
            brojTacnihNajbolja = brojTacnihNovaDodjela;
        }
    }
    return najbolja;
}

bool GSAT(Formula& f, int maxTries, int maxFlips,vector<bool>& dodjela, int& broj_tacnih_klauza)
{
    dodjela.resize(f.broj_varijabli + 1);
    vector<bool> najbolja;
    int brojTacnihNajbolja = 0;
    for (int i = 0; i < maxTries; i++)
    {
        generisiRandomDodjeluVarijabli(dodjela);
        for (int j = 0; j < maxFlips; j++)
        {
            broj_tacnih_klauza = f.prebrojZadovoljeneKlauze(dodjela);

            if (broj_tacnih_klauza == f.broj_klauza)
            {
                return true;
            }
            if(broj_tacnih_klauza > brojTacnihNajbolja)
            {
                najbolja = dodjela;
                brojTacnihNajbolja = broj_tacnih_klauza;
            }

            dodjela = generisiSljedecuNajbolju(dodjela, f);
        }
    }
    dodjela = najbolja;
    broj_tacnih_klauza = brojTacnihNajbolja;
    return false;
}

bool SimultanoKaljenje(Formula& f,vector<bool>& tren_dodjela, int& broj_tacnih_klauza, double Tstart, double Tmin, double alpha, int MAXTRIES)
{
    tren_dodjela.resize(f.broj_varijabli + 1);

    vector<bool> najboljaDodjela;
    int najboljaDodjelaTacnih = 0;
    double T;
    int i = 0;
    while(i  < MAXTRIES)
    {
        T = Tstart;
        generisiRandomDodjeluVarijabli(tren_dodjela);
        int tren_dodjela_tacnih = f.prebrojZadovoljeneKlauze(tren_dodjela);
        while(T > Tmin)
        {
            if(tren_dodjela_tacnih == f.broj_klauza)
                return true;
            if(tren_dodjela_tacnih > najboljaDodjelaTacnih)
            {
                najboljaDodjela = tren_dodjela;
                najboljaDodjelaTacnih = tren_dodjela_tacnih;
            }
            for(int j = 1; j < tren_dodjela.size(); j++)
            {
                auto novaDodjela = tren_dodjela;
                novaDodjela[j] = !novaDodjela[j];
                int novaTacnih = f.prebrojZadovoljeneKlauze(novaDodjela);
                int delta = novaTacnih - tren_dodjela_tacnih;
                if( 1/(1+exp(-delta/T)) > randomDouble())
                {
                    tren_dodjela[j] = !tren_dodjela[j];
                    tren_dodjela_tacnih = novaTacnih;
                }
            }
            T*=(1-alpha);
        }
        i++;
    }

    broj_tacnih_klauza = f.prebrojZadovoljeneKlauze(najboljaDodjela);
    tren_dodjela = najboljaDodjela;
    return false;
}

bool TabuSearch(Formula &f, int br_iteracija, vector<bool> &dodjela, int &broj_tacnih_klauza, int TABU)
{
    dodjela.resize(f.broj_varijabli + 1);
    vector<int> tabu(f.broj_varijabli + 1,0);

    generisiRandomDodjeluVarijabli(dodjela);

    vector<bool> najboljaDodjelaDoSad;
    int br_tacnih_uNDD = 0;

    for(int i = 0; i < br_iteracija; i++)
    {
        broj_tacnih_klauza = f.prebrojZadovoljeneKlauze(dodjela);

        if(broj_tacnih_klauza == f.broj_klauza)
            return true;
        if(broj_tacnih_klauza > br_tacnih_uNDD)
        {
            najboljaDodjelaDoSad = dodjela;
            br_tacnih_uNDD = broj_tacnih_klauza;
        }

        vector<bool> najboljaNovaDodjela;
        int br_tacnih_najbolja_ND = 0;
        vector<bool> novaDodjela;
        int indeks;

        for(int j = 1; j < dodjela.size(); j++)
        {
            if(tabu[j] != 0)
                continue;

            novaDodjela = dodjela;
            novaDodjela[j]  = !novaDodjela[j];
            if(f.prebrojZadovoljeneKlauze(novaDodjela) > br_tacnih_najbolja_ND)
            {
                indeks = j;
                br_tacnih_najbolja_ND = f.prebrojZadovoljeneKlauze(novaDodjela);
                najboljaNovaDodjela = novaDodjela;
            }
        }

        dodjela = najboljaNovaDodjela;
        for(int j = 1; j < tabu.size(); j++)
            if(tabu[j] > 0)
                tabu[j]--;
        tabu[indeks] = TABU;
    }
    broj_tacnih_klauza = br_tacnih_uNDD;
    dodjela = najboljaDodjelaDoSad;
    return false;
}


#endif // SAT_H_INCLUDED
