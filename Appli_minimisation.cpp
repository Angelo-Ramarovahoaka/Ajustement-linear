#include <iostream>
#include <cstdio>  // Pour FILE*, popen(), pclose()
#include <cmath>

using namespace std;

class Fonction {
public:
    Fonction();
    ~Fonction();
    void min(float& x0, int& iteration, float& alpha);
    void plot();
    void display(float x0, int iteration, float alpha);
    void menu(float& x0, int& iteration, float& alpha);
private:
    float x;
    float y;
};

Fonction::Fonction() : x(0), y(0) {
}

Fonction::~Fonction() {
}

void Fonction::plot() {
    FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe) {
        // Définir le titre du graphe
        fprintf(gnuplotPipe, "set title 'Minimum de la courbe'\n");

        // Tracer la courbe de fonction
        fprintf(gnuplotPipe, "plot [x=-6:7] (x*x*cos(x) - x)/10 title 'courbe', \\\n");
        fprintf(gnuplotPipe, "'-' with points pointtype 3 title 'minimum'\n");

        // Envoyer les points de données
        fprintf(gnuplotPipe, "%f %f\n", x, y);

        // Indiquer la fin des points de données
        fprintf(gnuplotPipe, "e\n");
        fflush(gnuplotPipe);

        // Fermer le pipe
        pclose(gnuplotPipe);
    } else {
        cerr << "Erreur à l'ouverture de gnuplot" << endl;
    }
}

void Fonction::min(float& x0, int& iteration, float& alpha) {
    x = x0;

    for (int i = 0; i < iteration; i++) {
        // Calculer le gradient
        float pas_x = (2 * x * cos(x) - x * x * sin(x) - 1) / 10;

        // Mettre à jour x en fonction du gradient et du taux d'apprentissage
        x -= alpha * pas_x;
    }

    // Calculer la valeur de y après optimisation
    y = (x * x * cos(x) - x) / 10;

    // Afficher les résultats
    Fonction::display(x0, iteration, alpha);
    Fonction::plot();
}

void Fonction::menu(float& x0, int& iteration, float& alpha){
    int choice;
    cout << "CHOIX : \n\n";
    cout << "1 : Pour changer le point initial x0 \n";
    cout << "2 : Pour changer le pas Alpha \n";
    cout << "3 : Pour changer l'iteration \n";
    cout << "Autre : pour les valeurs par defaut \n";
    
    cout << "\nEntrer votre choix\n";
    cin >> choice;
    switch (choice)
    {
    case 1 :
        cout << "Entrer la valeur initiale x0 \n";
        cin >> x0;
        break;
    case 2 :
        cout << "Entrer la valeur de alpha \n";
        cin >> alpha;
        break;
    case 3 :
        cout << "Entrer la valeur de l'iteration \n";
        cin >> iteration;
        break;
    default:
        break;
    }
}

void Fonction::display(float x0, int iteration, float alpha){
    cout << "x0 = " << x0 << endl;
    cout << "iteration = " << iteration << endl;
    cout << "alpha = " << alpha << endl;
    cout << "Le minimum est (" << x << "," << y << ")" << endl; 
}

int main() {
    Fonction ft;

    float x0 = 6;
    int iteration = 1000;
    float alpha = 0.1;
    ft.menu(x0, iteration,alpha);
    ft.min(x0, iteration,alpha);
    return 0;
}
