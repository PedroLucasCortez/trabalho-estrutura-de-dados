#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDADE_MAX 5 // Capacidade da pilha do estacionamento

// Estrutura do Veículo
typedef struct {
    char placa[10];
    char proprietario[50];
    int manobras;
} Veiculo;

// Elemento para Lista e Fila
typedef struct No {
    Veiculo veiculo;
    struct No* prox;
} No;

// Pilha para o Estacionamento (Capacidade Limitada)
typedef struct {
    Veiculo itens[CAPACIDADE_MAX];
    int topo;
} PilhaEstacionamento;

// Fila de Espera
typedef struct {
    No* inicio;
    No* fim;
} FilaEspera;

// Lista Encadeada para Histórico de Saídas
typedef struct {
    No* cabeca;
} ListaHistorico;

// Protótipos das funções

void inicializar_pilha(PilhaEstacionamento* p);
int pilha_cheia(PilhaEstacionamento* p);
int pilha_vazia(PilhaEstacionamento* p);
void empilhar(PilhaEstacionamento* p, Veiculo v);
Veiculo desempilhar(PilhaEstacionamento* p);

void inicializar_fila(FilaEspera* f);
void enfileirar(FilaEspera* f, Veiculo v);
Veiculo desenfileirar(FilaEspera* f);
int fila_vazia(FilaEspera* f);

void inicializar_historico(ListaHistorico* h);
void inserir_historico(ListaHistorico* h, Veiculo v);
void ordenar_historico_por_manobras(ListaHistorico* h);
void exibir_historico(ListaHistorico* h);

void retirar_veiculo(PilhaEstacionamento* p, FilaEspera* f, ListaHistorico* h, char* placa);
Veiculo* buscar_veiculo(PilhaEstacionamento* p, char* placa);
void exibir_estado(PilhaEstacionamento* p, FilaEspera* f);
void liberar_memoria(FilaEspera* f, ListaHistorico* h);

// Função principal (menu)

int main() {
    PilhaEstacionamento estacionamento;
    FilaEspera fila_espera;
    ListaHistorico historico;

    inicializar_pilha(&estacionamento);
    inicializar_fila(&fila_espera);
    inicializar_historico(&historico);

    int opcao;
    do {
        printf("\n Estacionamento com manobrista \n");
        printf("1. Registrar Entrada de Veiculo\n");
        printf("2. Registrar Saida de Veiculo (Processa Manobras)\n");
        printf("3. Buscar Veiculo por Placa\n");
        printf("4. Exibir Ocupacao Atual e Fila de Espera\n");
        printf("5. Ordenar e Exibir Historico por Manobras (Bubble Sort)\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // Limpar buffer

        switch (opcao) {
            case 1: {
                Veiculo v;
                v.manobras = 0;
                printf("Placa do Veiculo: ");
                fgets(v.placa, 10, stdin); v.placa[strcspn(v.placa, "\n")] = 0;
                printf("Nome do Proprietario: ");
                fgets(v.proprietario, 50, stdin); v.proprietario[strcspn(v.proprietario, "\n")] = 0;

                if (!pilha_cheia(&estacionamento)) {
                    empilhar(&estacionamento, v);
                    printf("Veiculo estacioado com sucesso na pilha!\n");
                } else {
                    enfileirar(&fila_espera, v);
                    printf("Estacionamento cheio! Veiculo adicionado a fila de espera.\n");
                }
                break;
            }
            case 2: {
                if (pilha_vazia(&estacionamento)) {
                    printf("O estacionamento esta vazio!\n");
                } else {
                    char placa[10];
                    printf("Digite a placa do veiculo a retirar: ");
                    fgets(placa, 10, stdin); placa[strcspn(placa, "\n")] = 0;
                    retirar_veiculo(&estacionamento, &fila_espera, &historico, placa);
                }
                break;
            }
            case 3: {
                char placa[10];
                printf("Digite a placa para busca: ");
                fgets(placa, 10, stdin); placa[strcspn(placa, "\n")] = 0;
                Veiculo* v = buscar_veiculo(&estacionamento, placa);
                if (v) {
                    printf("Encontrado! Placa: %s | Dono: %s | Manobras ate agora: %d\n",
                           v->placa, v->proprietario, v->manobras);
                } else {
                    printf("Veiculo nao localizado no estacionamento!\n");
                }
                break;
            }
            case 4:
                exibir_estado(&estacionamento, &fila_espera);
                break;
            case 5:
                ordenar_historico_por_manobras(&historico);
                exibir_historico(&historico);
                break;
            case 0:
                printf("Encerrando o sistema...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    // Liberação obrigatória de memória
    liberar_memoria(&fila_espera, &historico);
    printf("Memória liberada com sucesso. Até logo!\n");

    return 0;
}

// Implementação das funções

// --- Operações da Pilha ---
void inicializar_pilha(PilhaEstacionamento* p) {
    p->topo = -1;
}

int pilha_cheia(PilhaEstacionamento* p) {
    return p->topo == CAPACIDADE_MAX - 1;
}

int pilha_vazia(PilhaEstacionamento* p) {
    return p->topo == -1;
}

void empilhar(PilhaEstacionamento* p, Veiculo v) {
    if (!pilha_cheia(p)) {
        p->itens[++(p->topo)] = v;
    }
}

Veiculo desempilhar(PilhaEstacionamento* p) {
    return p->itens[(p->topo)--];
}

// --- Operações da Fila ---
void inicializar_fila(FilaEspera* f) {
    f->inicio = f->fim = NULL;
}

int fila_vazia(FilaEspera* f) {
    return f->inicio == NULL;
}

void enfileirar(FilaEspera* f, Veiculo v) {
    No* novo = (No*)malloc(sizeof(No));
    novo->veiculo = v;
    novo->prox = NULL;
    if (f->fim == NULL) {
        f->inicio = f->fim = novo;
        return;
    }
    f->fim->prox = novo;
    f->fim = novo;
}

Veiculo desenfileirar(FilaEspera* f) {
    No* temp = f->inicio;
    Veiculo v = temp->veiculo;
    f->inicio = f->inicio->prox;
    if (f->inicio == NULL) f->fim = NULL;
    free(temp);
    return v;
}

// --- Operações do Histórico (Lista Encadeada) ---
void inicializar_historico(ListaHistorico* h) {
    h->cabeca = NULL;
}

void inserir_historico(ListaHistorico* h, Veiculo v) {
    No* novo = (No*)malloc(sizeof(No));
    novo->veiculo = v;
    novo->prox = h->cabeca;
    h->cabeca = novo;
}

void ordenar_historico_por_manobras(ListaHistorico* h) {
    if (h->cabeca == NULL) return;
    int trocou;
    No* ptr1;
    No* lptr = NULL;

    do {
        trocou = 0;
        ptr1 = h->cabeca;

        while (ptr1->prox != lptr) {
            if (ptr1->veiculo.manobras < ptr1->prox->veiculo.manobras) { // Decrescente
                Veiculo temp = ptr1->veiculo;
                ptr1->veiculo = ptr1->prox->veiculo;
                ptr1->prox->veiculo = temp;
                trocou = 1;
            }
            ptr1 = ptr1->prox;
        }
        lptr = ptr1;
    } while (trocou);
}

void exibir_historico(ListaHistorico* h) {
    if (h->cabeca == NULL) {
        printf("\nNenhum veiculo no historico de saídas.\n");
        return;
    }
    printf("\n Histórico de saídas (ordenado por manobras) \n");
    No* temp = h->cabeca;
    while (temp != NULL) {
        printf("Placa: %-8s | Dono: %-15s | Total de Manobras: %d\n",
               temp->veiculo.placa, temp->veiculo.proprietario, temp->veiculo.manobras);
        temp = temp->prox;
    }
}

// --- Lógica de Retirada e Manobra ---
void retirar_veiculo(PilhaEstacionamento* p, FilaEspera* f, ListaHistorico* h, char* placa) {
    PilhaEstacionamento aux;
    inicializar_pilha(&aux);
    int encontrado = 0;
    Veiculo alvo;

    // Desempilha os carros até encontrar a placa solicitada
    while (!pilha_vazia(p)) {
        Veiculo atual = desempilhar(p);
        if (strcmp(atual.placa, placa) == 0) {
            alvo = atual;
            encontrado = 1;
            break; // Achou o carro que quer sair
        } else {
            atual.manobras++; // Sofreu manobra
            empilhar(&aux, atual);
        }
    }

    if (encontrado) {
        printf(" Veiculo %s retirado com sucesso!\n", alvo.placa);
        inserir_historico(h, alvo);

        // Recoloca os carros temporários de volta na pilha principal
        while (!pilha_vazia(&aux)) {
            Veiculo v = desempilhar(&aux);
            v.manobras++; // Incrementa na volta
            empilhar(p, v);
        }

        // Se houver veículo na fila de espera, insere no estacionamento
        if (!fila_vazia(f)) {
            Veiculo proximo = desenfileirar(f);
            empilhar(p, proximo);
            printf(" Veiculo %s da fila de espera entrou no estacionamento!\n", proximo.placa);
        }
    } else {
        printf(" Placa nao encontrada na fileira!\n");
        // Devolve os carros da pilha auxiliar
        while (!pilha_vazia(&aux)) {
            empilhar(p, desempilhar(&aux));
        }
    }
}

// --- Busca Sequencial ---
Veiculo* buscar_veiculo(PilhaEstacionamento* p, char* placa) {
    for (int i = 0; i <= p->topo; i++) {
        if (strcmp(p->itens[i].placa, placa) == 0) {
            return &p->itens[i];
        }
    }
    return NULL;
}

// Relatório e Estado 
void exibir_estado(PilhaEstacionamento* p, FilaEspera* f) {
    printf("\n Estado atual do estacionamento \n");
    printf("Vagas Ocupadas: %d / %d\n", p->topo + 1, CAPACIDADE_MAX);
    for (int i = p->topo; i >= 0; i--) {
        printf(" [ Posicao %d ] Placa: %s | Dono: %s | Manobras: %d\n",
               i + 1, p->itens[i].placa, p->itens[i].proprietario, p->itens[i].manobras);
    }

    printf("\n Fila de espera \n");
    if (fila_vazia(f)) {
        printf(" Ninguem na fila de espera.\n");
    } else {
        No* temp = f->inicio;
        int pos = 1;
        while (temp != NULL) {
            printf(" [ Fila Posicao %d ] Placa: %s | Dono: %s\n",
                   pos++, temp->veiculo.placa, temp->veiculo.proprietario);
            temp = temp->prox;
        }
    }
}

void liberar_memoria(FilaEspera* f, ListaHistorico* h) {
    while (!fila_vazia(f)) {
        desenfileirar(f);
    }
    No* atual = h->cabeca;
    while (atual != NULL) {
        No* prox = atual->prox;
        free(atual);
        atual = prox;
    }
    h->cabeca = NULL;
}