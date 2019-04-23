/* 
hsah.c
*/
#include <stdio.h>
#include <stdlib.h>
#include "integ_mac.h"
#include "hash.h"

#define MAX_HASH 10
#define HASH_KEY(key) key%MAX_HASH

struct node* hashTable[MAX_HASH];
struct node *get_hashNode()
{
  struct node *cp;
  cp = (struct node *)malloc(sizeof(struct node));
  cp->hashNext = NULL;
  return(cp);
}

void AddHashData(int key, struct node *node)
{
  int hash_key = HASH_KEY(key);
  if(hashTable[hash_key] == NULL)
  {
    hashTable[hash_key] = node;
  }
  else 
  {
    node->hashNext = hashTable[hash_key];
    hashTable[hash_key] = node;
  }
}

void DelHashData(int id)
{
  int hash_key = HASH_KEY(id);
  if (hashTable[hash_key] == NULL)
    return;
  
  // 삭제할 노드
  struct node* d_node = NULL;
  if(hashTable[hash_key]->id == id)
  {
    d_node = hashTable[hash_key];
    hashTable[hash_key] = hashTable[hash_key]->hashNext;
  }
  else 
  {
    struct node* c_node = hashTable[hash_key];
    struct node* n_node = c_node->hashNext;
    while(n_node)
    {
      if(n_node->id == id)
      {
        c_node->hashNext = n_node->hashNext;
        d_node = n_node;
        break;
      }
      c_node = n_node;
      n_node = c_node->hashNext;
    }
  }
  free(d_node);
}

struct node* FindHashData(int id)
{
  int hash_key = HASH_KEY(id);
  if(hashTable[hash_key] == NULL)
    return NULL;
  
  if(hashTable[hash_key]->id == id)
  {
    return hashTable[hash_key];
  }
  else
  {
    struct node* t_node = hashTable[hash_key];
    while(t_node->hashNext)
    {
      if(t_node->hashNext->id == id)
      {
        return t_node->hashNext;
      }
      t_node = t_node->hashNext;
    }
  }
  return NULL;
}

void PrintAllHashData()
{
  printf("\r\n 맥테이블 정보 \r\n");
  printf("---------------------\r\n");
  int i;
  for(i = 0; i < MAX_HASH; i++) {
    if(hashTable[i] != NULL)
    {
      struct node* t_node = hashTable[i];
      while(t_node->hashNext)
      {
        PrintHashData(t_node);
        t_node = t_node->hashNext;
      }
      PrintHashData(t_node);
    }
  }
  printf("\r\n");
}

void PrintHashData(struct node *t_node)
{
  int i, j;
  printf("\r\n** Integ Addr : ");
  for(i = 0; i < INTEG_ADDR_LEN; i++) {
    printf("%02X ", (t_node->data).integ_addr[i]);
  }
  printf(" | addrType : %s", addr_type_name[(t_node->data).addr_type]);
  printf("\r\n-----------\r\n");
  for(i = 0; i < MEDIA_NUM; i++) {
    printf("%s addr : ", media_name[i]);
    for(j = 0; j < media_addr_len[i]; j++) {
      printf("%02X ", (t_node->data).media_addr[i][j]);
    }
    printf("\r\n");
  }
  printf("\r\n-----------\r\n");
}