/*
 * Atividade 3 - Cifra com Shift e Sequencia Numerica
 * Disciplina: Algoritmos e Pensamento Computacional
 *
 * Justificativa: O SHIFT fixo aplica um deslocamento base constante (Cesar),
 * enquanto a sequencia numerica (seq[i]) varia o deslocamento em cada posicao,
 * evitando que letras iguais gerem a mesma letra cifrada.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LETRAS 15

/* 1. Normaliza a palavra: minusculas, sem espacos/hifens/acentos/cedilha (max 15) */
int normalizarPalavra(const char *entrada, char *saida) {
    int j = 0;
    for (int i = 0; entrada[i] != '\0' && j < MAX_LETRAS; i++) {
        unsigned char c = (unsigned char)entrada[i];
        char letra = '\0';

        if (c == 0xC3 && entrada[i + 1] != '\0') { /* Trata acentos e 'ç' em UTF-8 */
            unsigned char b = (unsigned char)entrada[++i] | 0x20;
            if (b >= 0xA0 && b <= 0xA5) letra = 'a';
            else if (b == 0xA7)         letra = 'c';
            else if (b >= 0xA8 && b <= 0xAB) letra = 'e';
            else if (b >= 0xAC && b <= 0xAF) letra = 'i';
            else if (b >= 0xB2 && b <= 0xB6) letra = 'o';
            else if (b >= 0xB9 && b <= 0xBC) letra = 'u';
        } else if (isalpha(c)) {
            letra = (char)tolower(c);
        }

        if (letra >= 'a' && letra <= 'z') {
            saida[j++] = letra;
        }
    }
    saida[j] = '\0';
    return j;
}

/* 2. Funcoes de geracao de sequencias numericas */
void gerarPrimos(int seq[], int n) {
    int num = 2, cont = 0;
    while (cont < n) {
        int primo = 1;
        for (int d = 2; d * d <= num; d++) {
            if (num % d == 0) { primo = 0; break; }
        }
        if (primo) seq[cont++] = num;
        num++;
    }
}

void gerarFibonacci(int seq[], int n) {
    for (int i = 0; i < n; i++) {
        if (i < 2) seq[i] = 1;
        else seq[i] = seq[i - 1] + seq[i - 2];
    }
}

void gerarPA(int seq[], int n, int a1, int razao) {
    for (int i = 0; i < n; i++) seq[i] = a1 + i * razao;
}

void gerarPG(int seq[], int n, int a1, int razao) {
    int termo = a1;
    for (int i = 0; i < n; i++) {
        seq[i] = termo;
        termo *= razao;
    }
}

/* 3. Funcoes de deslocamento e cifragem: nova_letra = aplicaShift(letra, SHIFT + seq[i]) */
char aplicaShift(char letra, int deslocamento) {
    int pos = (letra - 'a' + (deslocamento % 26) + 26) % 26;
    return (char)('a' + pos);
}

void processarPalavra(const char *entrada, int shift, const int seq[], int sinal, char *saida) {
    int tam = (int)strlen(entrada);
    for (int i = 0; i < tam; i++) {
        saida[i] = aplicaShift(entrada[i], sinal * (shift + seq[i]));
    }
    saida[tam] = '\0';
}

/* 4. Forca bruta: testa diferentes sequencias para decifrar palavras de outros grupos */
void decifrarForcaBruta(const char *cifrada, int tamanho, int shift) {
    int seq[MAX_LETRAS];
    char tentativa[MAX_LETRAS + 1];

    printf("\n--- Decifrando \"%s\" (Tamanho: %d | SHIFT: %d) ---\n", cifrada, tamanho, shift);

    gerarPrimos(seq, tamanho);
    processarPalavra(cifrada, shift, seq, -1, tentativa);
    printf(" [Primos]        -> %s\n", tentativa);

    gerarFibonacci(seq, tamanho);
    processarPalavra(cifrada, shift, seq, -1, tentativa);
    printf(" [Fibonacci]     -> %s\n", tentativa);

    for (int r = 1; r <= 3; r++) {
        gerarPA(seq, tamanho, 1, r);
        processarPalavra(cifrada, shift, seq, -1, tentativa);
        printf(" [PA a1=1, r=%d]  -> %s\n", r, tentativa);
    }

    for (int q = 2; q <= 3; q++) {
        gerarPG(seq, tamanho, 1, q);
        processarPalavra(cifrada, shift, seq, -1, tentativa);
        printf(" [PG a1=1, q=%d]  -> %s\n", q, tentativa);
    }
}

/* 5. Programa principal */
int main(void) {
    char palavraNorm[MAX_LETRAS + 1], cifrada[MAX_LETRAS + 1];
    int seq[MAX_LETRAS];

    /* Cifragem da palavra do grupo (Ex.: "coração" -> "coracao", SHIFT = 4, Primos) */
    const char *palavraOriginal = "coração";
    int shiftGrupo = 4;
    int tam = normalizarPalavra(palavraOriginal, palavraNorm);

    gerarPrimos(seq, tam);
    processarPalavra(palavraNorm, shiftGrupo, seq, 1, cifrada);

    printf("=== CIFRAGEM DO GRUPO ===\n");
    printf("Palavra original    : %s\n", palavraOriginal);
    printf("Palavra normalizada : %s (%d letras)\n", palavraNorm, tam);
    printf("SHIFT escolhido     : %d | Sequencia: Numeros Primos\n", shiftGrupo);
    printf("Palavra cifrada     : %s\n", cifrada);

    /* Tentativa de decifragem (testando a palavra cifrada com tamanho e SHIFT) */
    decifrarForcaBruta(cifrada, tam, shiftGrupo);

    /* Exemplo extra: decifrando palavra de outro grupo ("algoritmo" em Fibonacci, SHIFT = 3) */
    decifrarForcaBruta("epluztjkz", 9, 3);

    return 0;
}