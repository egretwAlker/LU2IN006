# LU2IN006

One can modify parameters in ```misc.h```.

todolist:
- Change append to strcat, or like this strcpy(t, c->T[i]->value); t += strlen(t);
- Use less buffer
- Add const as possible
- Test with hash conflicts
- Replace things by fts, stf

problems:
- When do commit, we commit the current version of the file instead of the version when added; however, when doing list-add, we show the version when added
- Git add can't handle deletion of files

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