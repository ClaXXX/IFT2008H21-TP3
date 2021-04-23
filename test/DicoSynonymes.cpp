#include <gtest/gtest.h>
#include "DicoSynonymes.h"

using namespace TP3;
using namespace std;

string dicoFile = "../data/PetitDico.txt";


TEST(DicoSynonymes, Constructor) {
  ifstream ifs;
  ifs.open (dicoFile);
  DicoSynonymes *dic = new DicoSynonymes();
  DicoSynonymes *second_constructor = new DicoSynonymes(ifs);
  ifs.close();

  EXPECT_TRUE(dic->estVide());
  EXPECT_EQ(dic->nombreRadicaux(), 0);
  delete second_constructor;
  delete dic;
}

TEST(DicoSynonymes, similitude) {
  DicoSynonymes dic;
  EXPECT_FLOAT_EQ(dic.similitude("niche", "chiens"), 0);
  EXPECT_FLOAT_EQ(dic.similitude("pareil", "pareil"), 1);
  EXPECT_FLOAT_EQ(dic.similitude("mots", "mois"), .75);
}

TEST(DicoSynonymes, radical) {
  DicoSynonymes dic;

  dic.ajouterRadical("mot");
  //  std::cout << dic << std::endl;
  EXPECT_FALSE(dic.estVide());
  EXPECT_THROW(dic.ajouterRadical("mot"), logic_error);
  dic.supprimerRadical("mot");
  EXPECT_TRUE(dic.estVide());
  EXPECT_THROW(dic.supprimerRadical("mot"), logic_error);
  dic.ajouterRadical("Test");
  EXPECT_FALSE(dic.estVide());
  EXPECT_THROW(dic.supprimerRadical("mode"), logic_error);
}

TEST(DicoSynonymes, flexions) {
  DicoSynonymes dic;

  dic.ajouterRadical("mot");
  dic.ajouterRadical("produire");
  dic.ajouterRadical("finir");
  dic.ajouterFlexion("produire", "produit");
  EXPECT_THROW(dic.ajouterFlexion("produit", "produire"), logic_error);
  for (auto flex: dic.getFlexions("produire"))
    EXPECT_EQ(flex, "produit");
  dic.supprimerFlexion("produire", "produit");
  EXPECT_THROW(dic.supprimerFlexion("produire", "produit"), logic_error);
}

TEST(DicoSynonymes, synonymes) {
  DicoSynonymes dic;
  int groupe = -1;
  int gp = -1;

  EXPECT_THROW(dic.supprimerSynonyme("mot", "écrit", groupe), std::logic_error);
  dic.ajouterRadical("mot");
  dic.ajouterRadical("tester");
  dic.ajouterRadical("contenu");
  dic.ajouterRadical("étirer");
  EXPECT_THROW(dic.getSens("mot", 0), std::logic_error);
  dic.ajouterSynonyme("mot", "écrit", groupe);
  dic.ajouterSynonyme("tester", "casser", gp);
  dic.supprimerRadical("tester");
  EXPECT_THROW(dic.ajouterSynonyme("mot", "écrit", groupe), std::logic_error);
  auto vecteur = dic.getSynonymes("mot", groupe);
  EXPECT_NE(find(vecteur.begin(), vecteur.end(), "écrit"), vecteur.end());
  dic.supprimerSynonyme("mot", "écrit", groupe);
  EXPECT_THROW(dic.supprimerSynonyme("mot", "écrit", groupe), std::logic_error);
  dic.ajouterSynonyme("mot", "écrit", groupe);
  dic.supprimerRadical("mot");
  EXPECT_THROW(dic.supprimerSynonyme("mot", "écrit", groupe), std::logic_error);
}

TEST(DicoSynonymes, givenTest) {
  DicoSynonymes dic;
  std::string mot = "poursuivre";
  int groupe = -1;

  dic.ajouterRadical(mot);
  dic.ajouterFlexion(mot, "poursuivrons");
  dic.ajouterSynonyme(mot, "pourchasser", groupe);
  dic.supprimerRadical(mot);
}

TEST(DicoSynonymes, fromFile) {
  ifstream ifs;
  ifs.open (dicoFile);
  DicoSynonymes dic(ifs);
  ifs.close();
  string mot;

  std::cout << dic << std::endl;
  mot = dic.rechercherRadical("poursuivrons");
  int nb = dic.getNombreSens(mot);
  EXPECT_EQ(nb, 2);
  EXPECT_EQ(dic.getSens(mot, 0), "pourchasser");
  EXPECT_EQ(dic.getSens(mot, 1), "continuer");
  auto synonymes = dic.getSynonymes(mot, 1);
  EXPECT_NE(find(synonymes.begin(), synonymes.end(), "continuer"), synonymes.end());
  EXPECT_NE(find(synonymes.begin(), synonymes.end(), "perpetuer"), synonymes.end());
  EXPECT_NE(find(synonymes.begin(), synonymes.end(), "prolonger"), synonymes.end());
  EXPECT_NE(find(synonymes.begin(), synonymes.end(), "maintenir"), synonymes.end());
  EXPECT_NE(find(synonymes.begin(), synonymes.end(), "persister"), synonymes.end());
  auto flexions = dic.getFlexions("maintenir");
  EXPECT_NE(find(flexions.begin(), flexions.end(), "maintient"), flexions.end());
  EXPECT_NE(find(flexions.begin(), flexions.end(), "maintiens"), flexions.end());
  EXPECT_NE(find(flexions.begin(), flexions.end(), "maintiendrai"), flexions.end());
  EXPECT_NE(find(flexions.begin(), flexions.end(), "maintiendras"), flexions.end());
  EXPECT_NE(find(flexions.begin(), flexions.end(), "maintiendrons"), flexions.end());
  mot = dic.rechercherRadical("etudes");
  EXPECT_EQ(mot, "etude");
  EXPECT_EQ(dic.getNombreSens(mot), 1);
  EXPECT_EQ(dic.getSens(mot, 0), "apprentissage");
  flexions = dic.getFlexions("apprentissage");
  EXPECT_NE(find(flexions.begin(), flexions.end(), "apprentissages"), flexions.end());
  mot = dic.rechercherRadical("lentement");
  EXPECT_EQ(mot, "lent");
  EXPECT_EQ(dic.getNombreSens(mot), 1);
  EXPECT_EQ(dic.getSens(mot, 0), "douce");
  flexions = dic.getFlexions("douce");
  EXPECT_NE(find(flexions.begin(), flexions.end(), "doucement"), flexions.end());
}