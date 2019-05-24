/*      

list.c

*/

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

struct node *Head = NULL;

struct node *get_node()
{
  struct node *cp;
  cp = (struct node *)malloc(sizeof(struct node));
  return(cp);
}

void free_node(struct node *np)
{
  free(np);
}

void insert_node_head(struct node *np)
{
  if (!Head) {
    Head = np;
    np->link = NULL;
  }
  else {
    np->link = Head;
    Head = np;
  }
}

void insert_node_tail(struct node *np)
{
  struct node *cp;
  if (!Head)
    Head = np;
  else {
    for (cp = Head; cp->link != NULL; cp = cp->link)
      ;
    cp->link = np;
  }
  np->link = NULL;
}

void insert_node_ascn(struct node *np)
{
  struct node *cp, *pp;
  if (!Head) {
    Head = np;
    np->link = NULL;
  }
  else {
    for (cp = Head, pp = NULL; cp != NULL && cp->data < np->data; pp = cp, cp = cp->link)
      ;
    if (pp == NULL) {
      np->link = Head;
      Head = np;
    } else {
      np->link = pp->link;
      pp->link = np;
    }
  }
}

void tour_list()
{
  struct node *cp;
  printf("\n");
  for (cp = Head; cp != NULL; cp = cp->link)
    printf("-->%c ", cp->data);
  printf("\n");
}

void free_list()
{
  struct node *cp;
  
  for ( ; Head != NULL; ) {
    cp = Head;
    Head = cp->link;
    free_node(cp);
  }
}
