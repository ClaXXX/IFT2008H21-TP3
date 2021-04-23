/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Étudiant(e)
* \version 0.5
* \date avril 2021
*
* Travail pratique numéro 3.
*
*/

#include <algorithm>
#include <new> // bad_alloc
#include "DicoSynonymes.h"
#include <algorithm>

namespace TP3
{

	// Méthode fournie
	void DicoSynonymes::chargerDicoSynonyme(std::ifstream& fichier)
	{
        if (!fichier.is_open())
            throw std::logic_error("DicoSynonymes::chargerDicoSynonyme: Le fichier n'est pas ouvert !");

		std::string ligne;
		std::string buffer;
		std::string radical;
		int cat = 1;

		while (!fichier.eof()) // tant qu'on peut lire dans le fichier
		{
            std::getline(fichier, ligne);

			if (ligne == "$")
			{
				cat = 3;
				std::getline(fichier, ligne);
			}
			if (cat == 1)
			{
				radical = ligne;
				ajouterRadical(radical);
				cat = 2;
			}
			else if (cat == 2)
			{
				std::stringstream ss(ligne);
				while (ss >> buffer)
					ajouterFlexion(radical, buffer);
				cat = 1;
			}
			else
			{
				std::stringstream ss(ligne);
				ss >> radical;
				ss >> buffer;
				int position = -1;
				ajouterSynonyme(radical, buffer, position);
				while (ss >> buffer)
					ajouterSynonyme(radical, buffer, position);
			}
		}
	}

	//Mettez l'implantation des autres méthodes demandées ici.
  DicoSynonymes::DicoSynonymes(): racine(nullptr), nbRadicaux(0) { }

  DicoSynonymes::DicoSynonymes(std::ifstream &fichier): racine(nullptr), nbRadicaux(0) {
    chargerDicoSynonyme(fichier);
  }

  DicoSynonymes::~DicoSynonymes() {
    if (!estVide())
      _libereArbre(racine);
  }

  void DicoSynonymes::_libereArbre(NoeudDicoSynonymes *arbre) {
    if (arbre->gauche) _libereArbre(arbre->gauche);
    if (arbre->droit) _libereArbre(arbre->droit);
    delete arbre;
  }

  int DicoSynonymes::_hauteur(NoeudDicoSynonymes *arbre) const {
    if (!arbre)
      return (-1);
    return arbre->hauteur;
  }

  bool DicoSynonymes::_debalancementAGauche(NoeudDicoSynonymes *arbre) const {
    if (!arbre)
      return false;
    return _hauteur(arbre->gauche) - _hauteur(arbre->droit) > 1;
  }

  bool DicoSynonymes::_debalancementADroite(NoeudDicoSynonymes *arbre) const {
    if (!arbre)
      return false;
    return _hauteur(arbre->droit) - _hauteur(arbre->gauche) > 1;
  }

  bool DicoSynonymes::_sousArbrePencheAGauche(NoeudDicoSynonymes *arbre) const {
    if (!arbre)
      return false;
    return _hauteur(arbre->gauche) > _hauteur(arbre->droit);
  }

  bool DicoSynonymes::_sousArbrePencheADroite(NoeudDicoSynonymes *arbre) const {
    if (!arbre)
      return false;
    return _hauteur(arbre->gauche) < _hauteur(arbre->droit);
  }

  void DicoSynonymes::_zigZigGauche(NoeudDicoSynonymes *&K2) {
    NoeudDicoSynonymes *K1 = K2->gauche;
    K2->gauche = K1->droit;
    K1->droit = K2;
    K2->hauteur = 1 + std::max(_hauteur(K2->gauche), _hauteur(K2->droit));
    K1->hauteur = 1 + std::max(_hauteur(K1->gauche), K2->hauteur);
    K2 = K1;
  }

  void DicoSynonymes::_zigZigDroit(NoeudDicoSynonymes *&K2) {
    NoeudDicoSynonymes *K1 = K2->droit;
    K2->droit = K1->gauche;
    K1->gauche = K2;
    K2->hauteur = 1 + std::max(_hauteur(K2->gauche), _hauteur(K2->droit));
    K1->hauteur = 1 + std::max(K2->hauteur, _hauteur(K1->droit));
    K2 = K1;
  }

  void DicoSynonymes::_zigZagGauche(NoeudDicoSynonymes *&K3) {
    _zigZigDroit(K3->gauche);
    _zigZigGauche(K3);
  }

  void DicoSynonymes::_zigZagDroit(NoeudDicoSynonymes *&K3) {
    _zigZigGauche(K3->droit);
    _zigZigDroit(K3);
  }

  void DicoSynonymes::_balancer(NoeudDicoSynonymes *&arbre) {
    if (_debalancementAGauche(arbre)) {
      _sousArbrePencheADroite(arbre->gauche) ? _zigZagGauche(arbre) : _zigZigGauche(arbre);
    } else if (_debalancementADroite(arbre)) {
      _sousArbrePencheAGauche(arbre->droit) ? _zigZagDroit(arbre) : _zigZigDroit(arbre);
    } else if (arbre) {
      arbre->hauteur = 1 + std::max(_hauteur(arbre->gauche), _hauteur(arbre->droit));
    }
  }

  void DicoSynonymes::_ajouterRadical(NoeudDicoSynonymes *&noeud, const std::string& motRadical) {
    if (!noeud) {
      noeud = new NoeudDicoSynonymes(motRadical);
      nbRadicaux++;
      return;
    } else if (noeud->radical > motRadical) {
      _ajouterRadical(noeud->gauche, motRadical);
    } else if (noeud->radical < motRadical) {
      _ajouterRadical(noeud->droit, motRadical);
    } else {
      throw std::logic_error("Un radical ne peut être ajouter deux fois");
    }
    _balancer(noeud);
  }
  
  void DicoSynonymes::ajouterRadical(const std::string& motRadical) {
    _ajouterRadical(racine, motRadical);
  }

  DicoSynonymes::NoeudDicoSynonymes *DicoSynonymes::_rechercheArbre(const std::string& motRadical) const {
    NoeudDicoSynonymes *temp = racine;

    if (estVide())
      throw std::logic_error("Aucun radical pour lequel ajouter la flexion");
    while (temp && temp->radical != motRadical) {
      if (temp->gauche && temp->radical > motRadical) temp = temp->gauche;
      else if (temp->droit && temp->radical < motRadical) temp = temp->droit;
      else throw std::logic_error("Le radical donné n'existe pas dans le dictionnaire");
    }
    return temp;
  }

  void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion) {
    NoeudDicoSynonymes *temp = _rechercheArbre(motRadical);
    auto it = std::find(temp->flexions.begin(), temp->flexions.end(), motFlexion);

    if (it != temp->flexions.end())
      throw std::logic_error("La flexion donnée est déjà existante");
    temp->flexions.push_back(motFlexion);
  }

  void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe) {
	  NoeudDicoSynonymes *temp = _rechercheArbre(motRadical);
	  NoeudDicoSynonymes *synonyme = nullptr;

	  if (numGroupe == -1) {
	    numGroupe = temp->appSynonymes.size();
	    temp->appSynonymes.push_back(groupesSynonymes.size());
	    groupesSynonymes.emplace_back();
	  } else if (numGroupe >= temp->appSynonymes.size()) {
      throw std::logic_error("Le numéro de groupe donné n'existe pas");
	  }
    try {
      ajouterRadical(motSynonyme);
    } catch (const std::logic_error &error) {}
    synonyme = _rechercheArbre(motSynonyme);
    if (std::find(groupesSynonymes[temp->appSynonymes[numGroupe]].begin(),
                  groupesSynonymes[temp->appSynonymes[numGroupe]].end(), synonyme) !=
      groupesSynonymes[temp->appSynonymes[numGroupe]].end()) {
      throw std::logic_error("Le synonyme est déjà présent dans la liste des synonymes pour ce radical");
    }
    groupesSynonymes[temp->appSynonymes[numGroupe]].push_back(synonyme);
  }

  void DicoSynonymes::_supprimerSuccDroite(NoeudDicoSynonymes *&arbre) {
    NoeudDicoSynonymes *temp = arbre->droit;
    NoeudDicoSynonymes *parent = arbre;

    while (temp->gauche) {
      parent = temp;
      temp = temp->gauche;
    }
    arbre->radical = temp->radical;
    arbre->flexions = temp->flexions;
    arbre->appSynonymes = temp->appSynonymes;
    // On ne veut pas que les groupes de synonymes soient désynchroniser avec l'arbre même si le cout d'une suppression a grandement augmenter
    for (const auto& groupe: groupesSynonymes)
      for (auto synonyme: groupe) if (synonyme == temp) synonyme = arbre;


    if (temp == parent->gauche)
      _supprimerRadical(parent->gauche, temp->radical);
    else
      _supprimerRadical(parent->droit, temp->radical);
  }

  void DicoSynonymes::_supprimerRadical(NoeudDicoSynonymes *&elem, const std::string &radical) {
    if (!elem)
      throw std::logic_error("Le radical n'existe pas");

    if (radical < elem->radical)
      _supprimerRadical(elem->gauche, radical);
    else if (radical > elem->radical)
      _supprimerRadical(elem->droit, radical);
    else if (elem->gauche && elem->droit) _supprimerSuccDroite(elem);
    else {
      NoeudDicoSynonymes *ancien = elem;
      elem = elem->droit ? elem->droit : elem->gauche;
      for (auto groupeSyn: ancien->appSynonymes)
        groupesSynonymes.erase(groupesSynonymes.begin() + groupeSyn);
      delete ancien;
      nbRadicaux--;
    }
    _balancer(elem);
  }

  void DicoSynonymes::supprimerRadical(const std::string& motRadical) {
    if (estVide())
      throw std::logic_error("L'abre ne contient aucun élément à supprimer");
    _supprimerRadical(racine, motRadical);
  }

  void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion) {
    NoeudDicoSynonymes *temp = _rechercheArbre(motRadical);
    auto it = std::find(temp->flexions.begin(), temp->flexions.end(), motFlexion);

    if (it == temp->flexions.end())
      throw std::logic_error("La flexion donnée n'existe pas");
    temp->flexions.erase(it);
  }

  void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonymes, int& numGroupe) {
	  NoeudDicoSynonymes *temp = _rechercheArbre(motRadical);
	  int index;

	  if (numGroupe >= temp->appSynonymes.size())
      throw std::logic_error("Le groupe donné n'exsiste pas");
	  index = temp->appSynonymes[numGroupe];

    auto synonyme = groupesSynonymes[index].begin();
    for (; synonyme != groupesSynonymes[index].end() && (*synonyme)->radical != motSynonymes; synonyme++);
    if (synonyme == groupesSynonymes[index].end())
      throw std::logic_error("Le synonyme est déjà présent dans la liste des synonymes pour ce radical");
    groupesSynonymes[index].erase(synonyme);
  }

  bool DicoSynonymes::estVide() const {
    return !(racine);
  }

  int DicoSynonymes::nombreRadicaux() const {
    return nbRadicaux;
  }

  // TODO: Retourne le radical du mot (qui est dans la liste des flexions) entré en paramètre
  // ERROR: logic_error si l'arbre est vide ou si la flexion n'existe pas
  std::string DicoSynonymes::rechercherRadical(const std::string& mot) const {
    NoeudDicoSynonymes *temp = racine;

    if (estVide())
      throw std::logic_error("Aucun mot ne sont présent dans le dictionnaire");
    while (temp) {
      if (similitude(temp->radical, mot) > .15) {
        if (temp->radical == mot)
          return temp->radical;
        auto it = std::find(temp->flexions.begin(), temp->flexions.end(), mot);
        if (it != temp->flexions.end()) {
          return temp->radical;
        }
      }
      if (!temp->gauche) break;
      else if (temp->radical > mot || !temp->droit) temp = temp->gauche;
      else if (temp->radical < mot) temp = temp->droit;
    }
    throw std::logic_error("La flexion n'existe pas");
  }

  // Sources: https://fr.wikipedia.org/wiki/Distance_de_Levenshtein
  int DicoSynonymes::_levenshtein(const std::string &w1, const std::string &w2) const {
    int D[w1.size()+1][w2.size()+1];
    int i, j, cost;

    for (i = 0; i <= w1.size(); i++) D[i][0] = i;
    for (j = 0; j <= w2.size(); j++) D[0][j] = j;
    for (i = 1; i <= w1.size(); i++) {
      for(j = 1; j <= w2.size(); j++) {
	if (w1[i-1] == w2[j-1]) cost = 0;
	else cost = 1;
	D[i][j] = std::min(D[i-1][j] + 1,
			   std::min(D[i][j-1] + 1,
				    D[i-1][j-1]+cost));
      }
    }
    return D[w1.size()][w2.size()];
  }

  float DicoSynonymes::similitude(const std::string &mot1, const std::string& mot2) const {
    return 1 - ((float)_levenshtein(mot1, mot2) / ((mot1.size() + mot2.size()) / 2));
  }

  int DicoSynonymes::getNombreSens(std::string radical) const {
    return (int )_rechercheArbre(radical)->appSynonymes.size();
  }

  std::string DicoSynonymes::getSens(std::string radical, int position) const {
    NoeudDicoSynonymes *temp = _rechercheArbre(radical);
    std::vector<std::string> synonymes;

    if (position >= temp->appSynonymes.size())
      throw std::logic_error("Groupe non existant");
    auto elem = groupesSynonymes[temp->appSynonymes.at(position)].begin();
    if (elem == groupesSynonymes[temp->appSynonymes.at(position)].end())
      throw std::logic_error("Groupe vide");
    return (*elem)->radical;
  }

  std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const {
	  NoeudDicoSynonymes *temp = _rechercheArbre(radical);
    std::vector<std::string> synonymes;

    if (position >= temp->appSynonymes.size())
      return synonymes;
    for (auto synonyme: groupesSynonymes[temp->appSynonymes.at(position)]) {
      synonymes.push_back(synonyme->radical);
    }
    return synonymes;
  }

  std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const {
    NoeudDicoSynonymes *temp = _rechercheArbre(radical);
    
    return std::vector<std::string>(temp->flexions.begin(), temp->flexions.end());
  }

}//Fin du namespace
