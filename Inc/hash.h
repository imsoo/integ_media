/* 
hash.h
*/

struct node
{
  int id;
  INTEG_TABLE data;
  struct node *hashNext;
};

struct node *get_hashNode();
void AddHashData(int key, struct node *node);
void DelHashData(int id);
struct node* FindHashData(int id);
void PrintAllHashData();
void PrintHashData(struct node *t_node);