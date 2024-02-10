# fst_m1_rdv_snowman
Projet forké depuis [ssloy/tinykaboom](https://github.com/ssloy/tinykaboom), de Monsieur Sokolov.

## Travail demandé
Dans le cadre du cours de *Représentation des Données Visuelles* à la Faculté des Sciences et Technologies
de l'Université de Lorraine, dispensé par Monsieur Sokolov, nous devons réaliser un bonhomme de neige avant la
fin de l'hiver 2024.

## Licence
> [!WARNING]
> Ce projet n'était sous aucune licence après le fork du projet original (le 31 janvier 2023), par conséquent
> le droit d'auteur classique s'appliquait (tous droits réservés).
> 
> À compté du 9 février 2024, il est passé sous licence [CeCILL-B](https://cecill.info/licences/Licence_CeCILL-B_V1-fr.html).

Nous vous invitons à consulter le fichier [LICENSE](./LICENSE) pour plus d'informations sur la licence de ce projet.

## Auteurs
* [Antoine Contoux](https://github.com/ActxLeToucan)
* [Guillaume Renard](https://github.com/dranerine)
### Code tiers
* [Dmitry V. Sokolov](https://github.com/ssloy)
  * [tinykaboom](https://github.com/ssloy/tinykaboom), sous license [WTFPL](https://spdx.org/licenses/WTFPL), projet original
  * [tinyraytracer](https://github.com/ssloy/tinyraytracer), sous license [WTFPL](https://spdx.org/licenses/WTFPL),
  pour le code permettant de charger et d'écrire des images avec la bibliothèque [stb](https://github.com/nothings/stb)
* [Inigo Quilez](https://iquilezles.org/), auteur de la fonction du calcul de distance à un cylindre, sous license [MIT](https://spdx.org/licenses/MIT)
### Bibliothèques
* [stb](https://github.com/nothings/stb), sous license [MIT](https://spdx.org/licenses/MIT)

## Compilation
```sh
git clone https://github.com/ActxLeToucan/fst_m1_rdv_snowman.git
cd fst_m1_rdv_snowman
mkdir build
cd build
cmake ..  
make
```

## Utilisation
```sh
./fst_m1_rdv_snowman [factor]
```
* `factor` : flottant, facteur sur la taille de l'image (par défaut 1.0). Permet de générer une image plus petite
(et donc plus rapidement) par exemple lors du développement, ou plus grande pour une image finale.
