#include <iostream>
#include "sat.h"

using namespace std;

int main()
{
    Formula f("test10.cnf");

    cout <<"Broj klauzula: " << f.broj_klauza << "\n";
    vector<bool> dodjela;
    int tacnih = 0;

    if(TabuSearch(f,1000,dodjela,tacnih,10))
        cout << "RIJESENO\n";
    else
    {
        cout << "NIJE\n";
        cout << "tacnih klauzula TABU: " << tacnih << "\n";
    }
    tacnih = 0;
    if(GSAT(f,5,1000,dodjela,tacnih))
        cout << "RIJESENO";
    else
    {
        cout << "NIJE\n";
        cout << "tacnih klauzula GSAT: " <<tacnih << endl;
    }
    tacnih = 0;
    if(SimultanoKaljenje(f,dodjela,tacnih,10,0.1,0.01,5))
        cout << "RIJESENO\n";
    else
    {
        cout << "NIJE\n";
        cout << "tacnih klauzala Simultano Kaljenje: " << tacnih << "\n";
    }
    return 0;
}
