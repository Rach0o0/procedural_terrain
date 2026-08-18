
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