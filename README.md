
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
