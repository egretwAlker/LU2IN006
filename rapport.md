# Rapport

## Sujet synthétique

On fait un system de gestion des versionnes qui possede les fonctionnalites principales de git. Par exemple, on veut sauvegarder instantanes des fichiers, les rassembler, pouvoir naviguer entre differents versions de projet, creer et gerer les branches afin de trvailler simultanement sur plusieurs versions des projets et etre capable reunir les branches en une version finale.

## Structure des fichiers du projet

```
bin/ # les fichiers compilés
src/ # de la sources code
tests/ # les tests
Makefile
```

## Implémentation

- liste chaînée, table de hachage, arbre
- On fait attention aux ownerships des mémoires: une fonction reçoit, donner/librer
  ; en pratique, on utilise le keyword const pour aider, mais il existe des cas où on veut qu'une fonction modifie des paramètres,
  normalement selon les fonctionalités des fonctions, ce cas est bien compris en usage, on écrit aussi parfois des commentaires
  pour aider.
- On fait un maximal d'abstraction que possible: on réutilise des fonctions
- Les assert
- On cache données du programme dans .myGit pour que ...
- 

## Quelques nos propres fonctions en plus (différentes) du poly

merge
...
les constantes
misc
err
sscanf(s, "(%[^,],%[^)])", buf1, buf2); utilisation de scanf intéressante

## Mode d'utilisation

On interdit ) dans la commentaire.
