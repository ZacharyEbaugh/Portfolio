#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "leak_detector_c.h"

#define add 1
#define query 2
#define MAXSIZE 100000

typedef struct trie{
	int count;
	int sumFreq;
	int curMax;

	struct trie* next[26];
}trie;

typedef struct indexLL{
	int data;
	struct indexLL* next;
}indexLL;

trie* init();
trie* del(trie* root, char key[], int depth);
int isEmpty(trie* root);
int search(trie* dictionary, char word[], int k, FILE* out);
void insert(trie* dictionary, char word[], int k, int count);
void process(trie* dictionary, FILE* in, FILE* out);
void printAll(trie* tree, char cur[], FILE* out);
void freeTrie(trie* root);
void listEmpty(indexLL *root);
indexLL *deleteLL(indexLL *root);
indexLL* insertLL(indexLL* root, int data);


int main(){
	//atexit(report_mem_leak);
	struct trie* dictionary = init();
	FILE* in = fopen("in.txt", "r");
	FILE* out = fopen("out.txt", "w");
	process(dictionary, in, out);
	fclose(in);
	fclose(out);
	freeTrie(dictionary);
	return 0;
}


void process(trie* dictionary, FILE* in, FILE* out){//runs through file and performs add or query
	int lines, command=0, totalCount;
	char tmpWord[MAXSIZE];
	fscanf(in, "%d", &lines);
	// printf("lines == %d\n", lines);
	for(int i=0; i<lines; i++){
		fscanf(in, "%d", &command);
		// printf("command: %d\n", command);
		// printf("Line %d\n", i+2);
		if(command==add){
			fscanf(in, "%s %d", tmpWord, &totalCount);
			// printf("%s\n", tmpWord);
			insert(dictionary, tmpWord, 0, totalCount);

		}
		if(command==query){
			fscanf(in, "%s", tmpWord);
			
			search(dictionary, tmpWord, 0, out);
		}
	}
}


int search(trie* dictionary, char word[], int k, FILE* out){//searches trie of last letter in word to predict next
	int nextIndex = word[k]-'a';
	if(k==strlen(word)){
		int tmpCount=0, tmpIndex=0;
		int tmpMax = dictionary->curMax;
		//printf("curMax == %d\n", tmpMax);
		indexLL* indexList = NULL;
		for(int i=0; i<26; i++){//check each pointer
			if(dictionary->next[i]!=NULL && (dictionary->next[i]->sumFreq>=tmpMax)){//checking to see most likely next letter
				
				tmpCount = dictionary->next[i]->count;
				tmpIndex = i+1;
				indexList = insertLL(indexList, tmpIndex);
			}			
		}
		if(indexList!=NULL){//prints characters found
			while(indexList!=NULL){
				fprintf(out,"%c", 'a'+indexList->data-1);
				indexList = deleteLL(indexList);
			}
			fprintf(out,"\n");
			
			free(indexList);
			return 1;
		}
		else{//if there are no next characters based on what is already inserted in the trie
			fprintf(out,"unknown word\n");
			return 0;
		}	
	}
	if(dictionary->next[nextIndex]!=NULL){
		return search(dictionary->next[nextIndex], word, k+1, out);//recursive call to increment to end of string
	}
	else{//detect words not inserted in trie
		fprintf(out,"unknown word\n");
		return 0;
	}
}


indexLL *deleteLL(indexLL *root){
	indexLL *tmp;
	tmp=root;
	root=root->next;
	free(tmp);
	return root;
}


indexLL* insertLL(indexLL* root, int data){//linked list used to hold index values 
	indexLL* t;
	indexLL* insertNode = malloc(sizeof(indexLL));
	insertNode->data = data;
	insertNode->next = NULL;
	if(root==NULL){
		root = insertNode;
		return root;
	}
	else{
		t=root;
		while(t->next!=NULL)
			t=t->next;
		t->next = insertNode;
	}
	return root;
}


trie* init(){
	trie* tree = malloc(sizeof(trie));
	tree->count = 0;
	tree->sumFreq=0;
	tree->curMax=0;
	for(int i=0; i<26; i++)
		tree->next[i] = NULL;
	return tree;
}


void insert(trie* dictionary, char word[], int k, int count){
	dictionary->sumFreq = dictionary->sumFreq + count;
	if(k==strlen(word)){//reaches end of string
		dictionary->count = count;
		return;
	}
	
	int nextIndex = word[k]-'a';
	if(dictionary->next[nextIndex]==NULL)
		dictionary->next[nextIndex] = init();
	
	if((dictionary->next[nextIndex]->sumFreq+count)>dictionary->curMax)
			dictionary->curMax = (dictionary->next[nextIndex]->sumFreq+count);
		
	
	insert(dictionary->next[nextIndex], word, k+1, count);//recursively increment to end of string
}


trie* del(trie* root, char key[], int depth){
	if(!root)
		return NULL;
	if(depth==strlen(key)){
		if(root->count)
			root->count=0;

		if(isEmpty(root)){
			free(root);
			root=NULL;
		}
		return root;
	}
	int index = key[depth] - 'a';
	root->next[index] = del(root->next[index], key, depth+1);
	if(isEmpty(root) && root->count == 0){
		free(root);
		root=NULL;
	}
	return root;
}


int isEmpty(trie* root){
	for(int i=0; i<26; i++){
		if(root->next[i])
			return 0;
	}
	return 1;
}


void freeTrie(trie* root){//free memory of trie
	if(!root)return;
	for(int i=0; i<26; i++){
		freeTrie(root->next[i]);
	}
	free(root);
}


void printAll(trie* tree, char cur[], FILE* out){
	if(tree==NULL) return;
	if(tree->count)
		fprintf(out,"%s %d\n", cur, tree->count);
	int len = strlen(cur);
	for(int i=0; i<26; i++){
		cur[len] = (char)('a'+i);
		cur[len+1] = '\0';
		printAll(tree->next[i], cur, out);
	}
}

void listEmpty(indexLL *root){
	if(root->next==NULL)
		printf("empty\n");
	else
		printf("not empty");
	return;
}