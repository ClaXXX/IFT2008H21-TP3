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

#include "DicoSynonymes.h"

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

}//Fin du namespace