
## Bruit de Perlin

### NAIVE SOLUTION
pour chaque pixel, on tire un nombre au hasard
-> inutilisable, 2 pixels voisins n'ont aucun rapport entre eux

Il faut du hasard cohérent!

### PERLIN 
n'aie du hasard qu'aux noeuds d'une grille grossière, et interpole entre eux
ex : une case de la grille fait 64 pixels, tous les 64 pixels -> hasard

### Gradients
Mais on met des gradients, pas des valeurs
A chque noeud, il met pas une altitude mais une direction (un ptit vecteur tiré au hasard), elle dit : "vu d'ici, le terrain monte dans cette direction"

### Consequences
- Chaque coin est un panneau indicateur, il possède une flèche 
- Pour un point quelconque dans une case, il demande aux quatre coins : "d'après toi, quelle est mon altitude?"
- Chaque coin répond avec un *produit scalaire entre sa flèche et le vecteur qui va de lui jusqu'au point" 
- On mélange les 4 réponses
- Au niveau d'un noeud, le vecteur qui va du noeud à lui même est nul -> le bruit de Perlin vaut zéro à tous les noeuds de la grille (extremum tombent donc au milieu des cases, à des endroits imprévisibles, et la grille devient invisible)

### Pk on mélange 
Pour combiner les 4 valuers, on fait une interpolation bilinéaire (on mélange les deux du haut, les deux du bas et les deux résultats)
Interpolation linéaire de base : a + t * (b-a)
mais la pente est constante et on peut voir les cassure 
-> Astuce est d'utiliser une courbe qui ecrase le t aux extrémites (fade)

### Stocker les gradients
Stocker un gradient par noeudest impossible, la grille est infinie
La solution est une table de 256 nombres mélangés une fois au démarrage
On utilise comme fonction de hachage, on combine les coordonnées en piochant dedans, et on obtient un indice de gradient reproductible

## Fractional Brownian

### Idea
Si on regarde une chaîne de montagne de loin, on voit quelques grandes masses. 
On s'approche et on voir des vallées plus petites.
On s'approche encore, on voir des bosses, puis des rochers, puis des cailloux.

A chaque échelle, on retrouve le même genre de forme, mais en plus petit et en moins marqué. 
-> Structure fractale.

### fBm
On superpose plusieurs copies du même bruit. 
Chaque copie est deux fois plus serrée et deux fois plus falible que la précédente.
On les additionne.
Chaque copie s'appelle une *octave* (musique : monter d'une octave double la fréquence)

### Paramètres
- Lacunarité : facteur qui multiplie la fréquence à chaque octave 
- Persistance : facteur qui multiplie l'amplitude

### Result 
On obtient des masses de plus en plus texturées

## Domain warping

### Goal
On demandait "quelle est l'altitude au point p"
Le domain warping consiste à répondre : "l'altitude au point p, c'est celle que le bruit avait à un autre endroit, un peu à côté" 

C'est comme si on étirait une image, compressait, répliait dans tous les sens 

### Why 
- fBm fait des taches rondes, parce qu'il n'a aucune raison de préférer une direction
- Mais la nature ne fait jamais de taches rondes
- Le décalage aléatoire casse cette rondeur et fabrique des formes allongées et sinueuses

### 2D 
il faut deux décalages, un pour x, un pour y sinon on décale toujours en diagonale

### Strength
il dépend du CELL_SIZE
Les coordonnées de bruit valent pixel/CELL_SIZE

## Dérivées analytiques
Le bruit me donne une altitude. La dérivée me donne la pente au même endroit

### Usage
- normales : Une normale est la direction perpendiculaire au sol. 
- coloration par pente : La neige ne tient pas sur une paroi raide, donc au delà d'un certain angle c'est de la roche
- faux effet d'érosion : les vallées restent lisses et les crêtes deviennent nettes

## Mesh

C'est deux listes.
- Les sommets : une liste de points dans l'espace (chacun a trois coordonnées)
- Les faces : une liste de triangle. chaque triangle ne stocke pas des points, il stocke trois numéros (positions des sommets dans la première liste)

### Passage aux sommets
Chaque case de la Heightmap devient un sommet 
- position = (x, altitude, y) -> convention de Blender

### Passage aux triangles
Chaque case de la grille, délimitée par 4 sommets voisins, devient deux triangles.

### Ordre des sommets
Un triangle -> deux faces
On doit savoir laquelle est le dessus
-> si les trois sommets se lisent dans le sens inverse des aiguilles d'une montre, c'est ce côté qui est la face visible

Ca se calcule avec un produit vectoriel. 

### Normals
si on ne fournit rien, Blender calcule une normale par triangle.
On donne une normale par sommet et le moteur interpole alors entre les trois sommets pour varier l'éclairage. 
-> mensonge visuel inventé par Gouraud puis Phong 

## OBJ Format
Un fichier texte, une ligne par élément et un mot clé au début de chaque ligne.

v 12.5 60.2 8.0        un sommet, ses coordonnées x y z
vn 0.1 0.98 -0.15      une normale, ses composantes x y z
f 1//1 513//513 2//2   un triangle

### f
Une face liste ses trois sommets, et chaque sommet peut référencer 3 choses (position, une coordonnée de texture, une normale)

## Coloration par altitude et par pente

- L'altitude décide d'abord : sable en bas, herbe, roche, neige en haut
- La pente corrige ensuite : parroi raide -> neige + terre ne tient pas (roche)


# EROSION HYDRAULIQUE

On lâche des gouttes d'eau, une par une et on voit ce qu'ils font.

### Capacité
Une goutte peut porter une certaine quantité de sédiment. C'est sa capacité ! 
A chaque pas, elle compare ce qu'elle porte à ce qu'elle pourrait porter. 
- Si elle porte moins que sa capacité, il lui reste de la place. Elle creuse le sol et charge ce qu'elle a arraché
- Si elle porte plus, elle est en surcharge. Elle laisse tomber l'excédent.

## Naive version

Ce que fait une goutte : 
- elle regarde ses voisins et va vers le plus bas
- elle calcule combien de sédiment elle peut porter
- si elle porte moins, elle creuse. Si elle porte plus, elle dépose
- elle accélère en descendant
- elle s'évaporte un peu

Une goutte porte 5 nombres : 
- x, y : sa position, en entiers
- water : son volume d'eau
- speed : sa vitesse
- sediment : ce qu'elle transporte

### Calcul de capacité

capacite = denivele * vitesse * eau * facteur

-> facteur est un reglage sans signification
