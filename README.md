# LU2IN006

One can modify parameters in ```misc.h```.

todolist:
- Change append to strcat, or like this strcpy(t, c->T[i]->value); t += strlen(t);
- Use less buffer
- Add const as possible
- Test with hash conflicts
- Replace things by fts, stf

problems:
- We can add 'a/b/c', but then if we want to do save/restore, there will be problems
- We don't really check, when adding, if something is already added (like if there is f as folder already, then we may add f/a)