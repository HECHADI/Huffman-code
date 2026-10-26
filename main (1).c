#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Noeud {
    char lettre;
    int occurences;
    struct Noeud* droite;
    struct Noeud* gauche;
} Noeud;


Noeud* creefeuille() {
    Noeud* Nouv_feuille = (Noeud*)malloc(sizeof(Noeud));
    Nouv_feuille->lettre = '\0';
    Nouv_feuille->occurences = 0;
    Nouv_feuille->droite = NULL;
    Nouv_feuille->gauche = NULL;
    return Nouv_feuille;
}


void compter_frequences_fichier(const char* nom_fichier, Noeud* T[], int* taille) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        printf("Erreur lors de l'ouverture du fichier %s\n", nom_fichier);
        exit(1);
    }

    char c;
    while ((c = fgetc(fichier)) != EOF) {
        int found = 0;
        for (int i = 0; i < *taille; i++) {
            if (T[i]->lettre == c) {
                T[i]->occurences++;
                found = 1;
                break;
            }
        }
        if (!found) {
            Noeud* Nouv_feuille = creefeuille();
            Nouv_feuille->lettre = c;
            Nouv_feuille->occurences = 1;
            T[*taille] = Nouv_feuille;
            (*taille)++;
        }
    }

    fclose(fichier);
}


void afficher_codes(Noeud* racine, char* code) {
    if (racine == NULL) return;

    if (racine->lettre != '\0') {
        printf("Lettre: '%c' -> Code: %s\n", racine->lettre, code);
    }


    char code_gauche[strlen(code) + 2];
    strcpy(code_gauche, code);
    strcat(code_gauche, "0");
    afficher_codes(racine->gauche, code_gauche);


    char code_droite[strlen(code) + 2];
    strcpy(code_droite, code);
    strcat(code_droite, "1");
    afficher_codes(racine->droite, code_droite);
}


void generer_codes(Noeud* racine, char* code, char* codes[256]) {
    if (racine == NULL) return;

    if (racine->lettre != '\0') {
        codes[(unsigned char)racine->lettre] = strdup(code);
    }

    char code_gauche[strlen(code) + 2];
    strcpy(code_gauche, code);
    strcat(code_gauche, "0");
    generer_codes(racine->gauche, code_gauche, codes);

    char code_droite[strlen(code) + 2];
    strcpy(code_droite, code);
    strcat(code_droite, "1");
    generer_codes(racine->droite, code_droite, codes);
}


Noeud* min_1(Noeud* T[], int taille) {
    int i;
    Noeud* min1 = T[0];
    for (i = 1; i < taille; i++) {
        if (T[i]->occurences < min1->occurences) {
            min1 = T[i];
        }
    }
    return min1;
}


Noeud* min_2(Noeud* T[], int taille) {
    int i;
    Noeud* min2 = NULL;
    Noeud* min1 = min_1(T, taille);

    for (i = 0; i < taille; i++) {
        if (T[i] != min1 && (min2 == NULL || T[i]->occurences < min2->occurences)) {
            min2 = T[i];
        }
    }

    return min2;
}


void suppression(Noeud* T[], Noeud* tete, int* taille) {
    for (int i = 0; i < *taille; i++) {
        if (T[i] == tete) {
            for (int j = i; j < *taille - 1; j++) {
                T[j] = T[j + 1];
            }
            (*taille)--;
            break;
        }
    }
}


void compresser_fichier(const char* fichier_entree, const char* fichier_sortie, Noeud* arbre_huffman, char* codes[256]) {
    FILE* f_in = fopen(fichier_entree, "r");
    FILE* f_out = fopen(fichier_sortie, "wb");

    if (!f_in || !f_out) {
        printf("Erreur d'ouverture de fichier\n");
        exit(1);
    }

    char c;
    while ((c = fgetc(f_in)) != EOF) {
        char* code = codes[(unsigned char)c];
        for (int i = 0; code[i] != '\0'; i++) {
            fputc(code[i], f_out);
        }
    }

    fclose(f_in);
    fclose(f_out);
}

int main() {
    const char* fichier_entree = "C:\\Users\\HP\\.VirtualBox\\selectorwindow.log";
    const char* fichier_sortie = "moad2_compressed.bin";

    Noeud* T[256];
    int taille = 0;

    compter_frequences_fichier(fichier_entree, T, &taille);

    int c = taille;
    while (c > 1) {
        Noeud* min1 = min_1(T, c);
        Noeud* min2 = min_2(T, c);

        Noeud* Nouv_feuille = creefeuille();
        Nouv_feuille->occurences = min1->occurences + min2->occurences;
        Nouv_feuille->gauche = min1;
        Nouv_feuille->droite = min2;

        T[taille] = Nouv_feuille;
        taille++;

        suppression(T, min1, &taille);
        suppression(T, min2, &taille);

        c = taille;
    }

    char* codes[256] = {0};
    generer_codes(T[0], "", codes);

    compresser_fichier(fichier_entree, fichier_sortie, T[0], codes);

    printf("Compression terminée. Le fichier compressé est %s\n", fichier_sortie);

    return 0;
}
