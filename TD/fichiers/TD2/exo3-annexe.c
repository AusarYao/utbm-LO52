symrec *symtable = (symrec *)0;

symrec *getsymbol(char *name) {
  symrec *p = symtable;
  while(p) {
    if(!strcmp(name, p->name))
      return p;
    p = p -> next;
  }
  return (symrec *)0;
}

symrec putsymbol(char *name) {
  symrec *p = (symroc *)malloc(sizeof(symrec);
  strcpy(name, p->name);
  p->var = 0;
  p->next = symtable;
  symtable = p;
  return p;
}
