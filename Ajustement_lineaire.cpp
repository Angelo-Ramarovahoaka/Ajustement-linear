#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstdio>  // Pour FILE*, popen(), pclose()

using namespace std;

// Classe pour encapsuler un point
class Point {
public:
    double x;  
    double y;  

    // Constructeur pour initialiser un point avec des valeurs données ou par défaut
    Point(double x = 0, double y = 0);
};

// Implémentation du constructeur de la classe Point
Point::Point(double x, double y) {
    this->x = x;  
    this->y = y;  
}

// Classe pour lire les points de données depuis un fichier
class DataLoader {
public:
    // Méthode pour lire les points de données depuis un fichier
    vector<Point> readDataFromFile(const string& filename);
};

// Implémentation de la méthode pour lire les données
vector<Point> DataLoader::readDataFromFile(const string& filename) {
    vector<Point> points;  // Vecteur pour stocker les points lus
    ifstream file(filename);  

    if (!file.is_open()) {  
        cerr << "Erreur lors de l'ouverture du fichier " << filename << endl;
        return points;  
    }

    int numPoints;  // Nombre de points à lire
    file >> numPoints;  // Lit le nombre de points
    // file.ignore(); // Ignore le reste de la ligne

    string line;  
    while (getline(file, line)) {  // Lit chaque ligne du fichier

        stringstream ss(line);  // Crée un flux de chaîne pour traiter la ligne
        double x, y; 
        char comma;  // Caractère pour ignorer la virgule
        ss >> x >> comma >> y;  // Lit les coordonnées du point et ignore la virgule
        points.emplace_back(x, y);  // Ajoute le point au vecteur
    }

    file.close(); 

    return points; 
}

// Classe de base pour la régression linéaire
class LinearRegression {
protected:
    vector<Point> points;  
    double a;  // Coefficient directeur de la droite (pente)
    double b;  // Ordonnée à l'origine de la droite

public:
    // Constructeur pour initialiser avec les points fournis
    LinearRegression(const vector<Point>& points);

    // Méthode virtuelle pure pour ajuster le modèle (à définir dans les classes dérivées)
    virtual void fit() = 0;

    // Méthode pour générer un script Gnuplot
    virtual void generateGnuplotScript()=0;

    // Méthode pour afficher la droite de régression
    void predict() const;

};

// Implémentation du constructeur de la classe LinearRegression
LinearRegression::LinearRegression(const vector<Point>& points) {
    this->points = points;  
    this->a = 0;  
    this->b = 0;  
}

// Méthode pour afficher la droite de régression
void LinearRegression::predict() const {
    cout << "La droite de régression est y = " << a << "x + " << b << endl;
}


// Classe dérivée pour la méthode des moindres carrés
class MoindreCarre : public LinearRegression {
public:
    // Constructeur pour initialiser avec les points fournis
    MoindreCarre(const vector<Point>& points) : LinearRegression(points) {};

    // Méthode pour ajuster le modèle en utilisant la méthode des moindres carrés
    void fit();
    // Méthode pour générer un script Gnuplot pour visualiser la régression
    void generateGnuplotScript();
};

// Implémentation de la méthode d'ajustement pour la méthode des moindres carrés
void MoindreCarre::fit() {
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;  // Variables pour les sommes nécessaires
    int n = points.size();  // Nombre de points

    // Calcule les sommes nécessaires pour les formules des moindres carrés
    for (const auto& point : points) {  // Parcourt tous les points
        sumX += point.x;  // Somme des x
        sumY += point.y;  // Somme des y
        sumXY += point.x * point.y;  // Somme des x*y
        sumX2 += point.x * point.x;  // Somme des x^2
    }

    // Calcule les coefficients de la droite de régression
    a = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);  
    b = (sumY - a * sumX) / n;  
}


void MoindreCarre::generateGnuplotScript() {
    FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe) {
        // Définir le titre du graphe
        fprintf(gnuplotPipe, "set title 'Régression Linéaire - Moindre carrée'\n");
        fprintf(gnuplotPipe, "set xlabel 'X'\n");
        fprintf(gnuplotPipe, "set ylabel 'Y'\n");
        fprintf(gnuplotPipe, "set grid\n");
        fprintf(gnuplotPipe, "plot 'mesures1.txt' using 1:2 with points pointtype 7 title 'Data Points', \\\n");

        // Envoyer les points de données
        fprintf(gnuplotPipe, "     %f * x + %f title 'Ligne de Régression' with lines\n", a, b);

        // Indiquer la fin des points de données
        fprintf(gnuplotPipe, "pause -1\n");
        fflush(gnuplotPipe);

        // Fermer le pipe
        pclose(gnuplotPipe);
    } else {
        cerr << "Erreur à l'ouverture de gnuplot" << endl;
    }
}
// Classe dérivée pour la méthode de descente de gradient
class DescenteGradient : public LinearRegression {
private:
    double learningRate;  // Taux d'apprentissage pour la descente de gradient
    int iterations;  // Nombre d'itérations pour la descente de gradient
    vector<double> a_values;  // Historique des valeurs de la pente
    vector<double> b_values;  // Historique des valeurs de l'ordonnée à l'origine

public:
    // Constructeur pour initialiser avec les points, le taux d'apprentissage et le nombre d'itérations
    DescenteGradient(const vector<Point>& points, double learningRate, int iterations);

    // Méthode pour ajuster le modèle en utilisant la descente de gradient
    void fit();

    // Méthode pour générer un script Gnuplot avec des lignes intermédiaires
    void generateGnuplotScript();
};

// Implémentation du constructeur de la classe DescenteGradient
DescenteGradient::DescenteGradient(const vector<Point>& points, double learningRate, int iterations)
    : LinearRegression(points), learningRate(learningRate), iterations(iterations) {
    // Initialise la classe de base avec les points, et les paramètres spécifiques
}

// Implémentation de la méthode d'ajustement pour la descente de gradient
void DescenteGradient::fit() {
    int n = points.size();  // Nombre de points
    a = 0;  // Initialiser la pente à 0
    b = 0;  // Initialiser l'ordonnée à l'origine à 0

    // Effectue les itérations de la descente de gradient
    for (int i = 0; i < iterations; ++i) {
        double a_gradient = 0;  // Gradient pour le coefficient directeur
        double b_gradient = 0;  // Gradient pour l'ordonnée à l'origine

        // Calcule les gradients pour tous les points
        for (const auto& point : points) {
            double prediction = a * point.x + b;  // Calcule la prédiction pour le point
            a_gradient += -2 * point.x * (point.y - prediction);  // Calcule le gradient pour a
            b_gradient += -2 * (point.y - prediction);  // Calcule le gradient pour b
        }

        // Met à jour les coefficients en utilisant les gradients et le taux d'apprentissage
        a -= (a_gradient /n) * learningRate;  // Mise à jour de a
        b -= (b_gradient /n) * learningRate;  // Mise à jour de b

        // Enregistre les coefficients à certaines étapes
        if (i % 500 == 0) {
            a_values.push_back(a);  // Stocke la valeur de a
            b_values.push_back(b);  // Stocke la valeur de b
        }
    }
}

// Implémentation de la méthode pour générer un script Gnuplot
void DescenteGradient::generateGnuplotScript() {
    FILE* gnuplotPipe=popen("gnuplot -persistent","w");
    if (gnuplotPipe) {
        // Définir le titre du graphe
        fprintf(gnuplotPipe, "set title 'Régression Linéaire - Descente du gradient'\n");
        
        // Étiquettes des axes
        fprintf(gnuplotPipe, "set xlabel 'X'\n");
        fprintf(gnuplotPipe, "set ylabel 'Y'\n");
        
        // Afficher la grille
        fprintf(gnuplotPipe, "set grid\n");
        
        // Tracer les points de données
        fprintf(gnuplotPipe, "plot 'mesures1.txt' using 1:2 with points pointtype 7 title 'Data Points', \\\n");
        
        // Tracer les lignes intermédiaires
        for (size_t i = 0; i < a_values.size(); ++i) {
            fprintf(gnuplotPipe, "     %f * x + %f title 'Ligne Iter %lu' with lines, \\\n",
                    a_values[i], b_values[i], i * 500);
        }
        
        // Tracer la ligne de régression finale
        fprintf(gnuplotPipe, "     %f * x + %f title 'Ligne de Régression Finale' with lines\n", a, b);
        
        // Pause pour garder la fenêtre ouverte
        fprintf(gnuplotPipe, "pause -1\n");
        
        // Fermer le pipe
        pclose(gnuplotPipe);
    } else {
        std::cerr << "Erreur à l'ouverture de gnuplot" << std::endl;
    }
}

// Fonction principale
int main() {
    DataLoader dataLoader;  // Crée un objet DataLoader
    vector<Point> points = dataLoader.readDataFromFile("mesures1.txt");  // Lit les points depuis le fichier
    if (points.empty()) {  // Vérifie si aucun point n'a été lu
        cerr << "Aucun point lu du fichier." << endl;
        return 1;  // Termine le programme en cas d'erreur
    }

    cout << "=== Moindres Carrés ===" << endl;
    MoindreCarre moindreCarre(points);  // Crée un objet MoindreCarre avec les points
    moindreCarre.fit();  // Ajuste le modèle avec la méthode des moindres carrés
    moindreCarre.predict();  
    moindreCarre.generateGnuplotScript(); 

    cout << "=== Descente de Gradient ===" << endl;
    double learningRate = 0.01;  // Taux d'apprentissage pour la descente de gradient
    int iterations = 10000;  // Nombre d'itérations pour la descente de gradient
    DescenteGradient descenteGradient(points, learningRate, iterations);  // Crée un objet DescenteGradient avec les points, le taux d'apprentissage et les itérations
    descenteGradient.fit(); 
    descenteGradient.predict();  
    descenteGradient.generateGnuplotScript();  

    return 0; 
}
