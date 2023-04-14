# LU2IN006

One can modify parameters in ```misc.h```.

## Mode d'utilisation

On ne travaille que dans un répertoire de travail, on ajoute soit un dossier en entier soit un fichier à ce répertoire.

### Structure des fichers

```
.mygit/.add (WorkTree représentant la zone de mémoire)
      /.current_branch (Nom de la branche courante)
      /.ref/HEAD (Hash du commit où il pointe)
           /master
           ...
```