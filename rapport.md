# Rapport

## Rapport sur l'implémentation

- On fait un maximal d'abstraction que possible: on réutilise des fonctions
- On fait attention aux ownerships des mémoires: une fonction reçoit, donner/librer
  ; en pratique, on utilise le keyword const pour aider, mais il existe des cas où on veut qu'une fonction modifie des paramètres,
  normalement selon les fonctionalités des fonctions, ce cas est bien compris en usage, on écrit aussi parfois des commentaires
  pour aider.

## Quelques nos propres fonctions en plus (différentes) du poly

merge
...
les constantes
misc
err
sscanf(s, "(%[^,],%[^)])", buf1, buf2); utilisation de scanf intéressante

## Structure des fichiers du projet

## Mode d'utilisation

On interdit ) dans la commentaire.
