#include <stdio.h>
#include <stdlib.h>
#include "list.h"


//Inicializa o primeiro elemento da lista
ListNode* initList()
{
    return NULL;
}

//Lista todos os elementos da lista
void showNodes(ListNode* List)
{
    ListNode* Temp; //nodo auxiliar para percorrer a lista

    if(List == NULL)
        printf("Empty list");
    else
        for(Temp = List; Temp != NULL; Temp = Temp->NextNode)
            printf("Valor do dado: %d\n",Temp->handle->handleNumber);
}

//Insere elementos na lista
ListNode* insert(ListNode* List, FileHandle* handle)
{
    ListNode* NewNode; //novo elemento
    ListNode* Temp; //nodo auxiliar para percorrer a lista
    ListNode* Ant = NULL; //nodo auxiliar que armazena o nodo anterior na varredura da lista

    NewNode = (ListNode*)malloc(sizeof(ListNode)); //Aloca memoria para o novo nodo
    NewNode->handle = handle;

    Temp = List;
    while(Temp !=NULL)         //percorre a lista até a ultima posição
    {
        Ant = Temp;
        Temp = Temp->NextNode;
    }

    if(Ant == NULL) //insere elemento na primeira posição da lista, caso ela esteja vazia
    {
        NewNode->NextNode = NULL;
        List = NewNode;
    }
    else //caso contrario, insere em outras posições
    {
        NewNode->NextNode = NULL;
        Ant->NextNode = NewNode;
    }

    return List;
}

//consulta um determinado elemento na lista
ListNode* search(ListNode* List, unsigned int handleNumber)
{
    ListNode* Temp; //nodo auxiliar para percorrer a lista

    Temp = List;
    while( (Temp != NULL) && (Temp->handle->handleNumber != handleNumber) )
        Temp = Temp->NextNode;
    return Temp;

}

//remove um elemento na Lista
ListNode* removeNode(ListNode* List, unsigned int handleNumber)
{
    ListNode* Temp; //nodo auxiliar para percorrer a lista
    ListNode* Ant = NULL; //nodo auxiliar que armazena o nodo anterior

    Temp = List;
    while( (Temp != NULL) && (Temp->handle->handleNumber != handleNumber) )
    {
        Ant = Temp;
        Temp = Temp->NextNode;
    }

    //Verifica se achou o elemento
    if(Temp == NULL) //se não achou
        return List;
    else //caso contrário
    {
        if(Ant == NULL) //remove o primeiro elemento
            List = Temp->NextNode;
        else //remove do meio ou do final
           Ant->NextNode = Temp->NextNode;
    }
    free(Temp->handle->descriptor);
    free(Temp->handle);
    free(Temp);
    return List;
}

//destroi a lista
ListNode* destroyList(ListNode* List)
{
	ListNode* Temp; //nodo auxiliar para percorrer a lista

	while(List != NULL)
	{
		Temp = List;
		List = List->NextNode;
		free(Temp);
	}
	free(List);	
	return NULL;
}
