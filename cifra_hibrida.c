/*
 * ============================================================================
 * Atividade 3 - Cifra Hibrida com Shift Fixo e Sequencia Numerica
 * Disciplina: Algoritmos e Pensamento Computacional (ALGPC)
 *
 * Objetivo do Projeto:
 *   Combinar um deslocamento fixo (SHIFT) com o termo variavel de uma
 *   sequencia matematica (seq[i]) pela formula:
 *       nova_letra = aplicaShift(letra, SHIFT + seq[i])
 *   No modo de decifragem por forca bruta, o programa percorre todos os
 *   intervalos informados (SHIFT, a/q da PG e a/r da PA) por meio de lacos
 *   'for' aninhados e exibe todas as combinacoes linha por linha.
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LETRAS 15 /* Limite maximo de caracteres exigido pela atividade */

/* Funcao auxiliar: limpa o '\n' (Enter) residual do teclado apos o scanf */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Funcao auxiliar: garante que 'inicio' seja menor ou igual a 'fim' */
void garantirOrdem(int *inicio, int *fim) {
    if (*inicio > *fim) {
        int aux = *inicio;
        *inicio = *fim;
        *fim = aux;
    }
}

/* ============================================================================
 * BLOCO 1: NORMALIZACAO DA ENTRADA
 * Converte para minusculas ('a'..'z') e remove espacos, hifens, acentos e 'ç'.
 * ============================================================================ */
int normalizarPalavra(const char *entrada, char *saida) {
    int j = 0;
    for (int i = 0; entrada[i] != '\0' && j < MAX_LETRAS; i++) {
        unsigned char c = (unsigned char)entrada[i];
        char letra = '\0';

        if (c == 0xC3 && entrada[i + 1] != '\0') { /* Trata UTF-8 (acentos e 'ç') */
            unsigned char segundoByte = (unsigned char)entrada[++i] | 0x20;
            if (segundoByte >= 0xA0 && segundoByte <= 0xA5) letra = 'a';
            else if (segundoByte == 0xA7)                    letra = 'c';
            else if (segundoByte >= 0xA8 && segundoByte <= 0xAB) letra = 'e';
            else if (segundoByte >= 0xAC && segundoByte <= 0xAF) letra = 'i';
            else if (segundoByte >= 0xB2 && segundoByte <= 0xB6) letra = 'o';
            else if (segundoByte >= 0xB9 && segundoByte <= 0xBC) letra = 'u';
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

/* ============================================================================
 * BLOCO 2: GERADORES DE SEQUENCIAS NUMERICAS (seq[i])
 * ============================================================================ */

/* Gera os 'n' primeiros Numeros Primos (2, 3, 5, 7, 11...) */
void gerarPrimos(int seq[], int n) {
    int numeroAtual = 2, encontrados = 0;
    while (encontrados < n) {
        int ehPrimo = 1;
        for (int divisor = 2; divisor * divisor <= numeroAtual; divisor++) {
            if (numeroAtual % divisor == 0) {
                ehPrimo = 0;
                break;
            }
        }
        if (ehPrimo) seq[encontrados++] = numeroAtual;
        numeroAtual++;
    }
}

/* Gera os 'n' primeiros termos da Serie de Fibonacci (1, 1, 2, 3, 5, 8...) */
void gerarFibonacci(int seq[], int n) {
    for (int i = 0; i < n; i++) {
        if (i < 2) seq[i] = 1;
        else seq[i] = seq[i - 1] + seq[i - 2];
    }
}

/* Gera uma Progressao Aritmetica (PA) usando termo inicial 'a' e razao 'r' */
void gerarPA(int seq[], int n, int pa_a, int pa_r) {
    for (int i = 0; i < n; i++) {
        seq[i] = pa_a + i * pa_r;
    }
}

/* Gera uma Progressao Geometrica (PG) usando termo inicial 'a' e razao 'q' */
void gerarPG(int seq[], int n, int pg_a, int pg_q) {
    long long termoAtual = pg_a; /* long long evita estouro em potencias grandes */
    for (int i = 0; i < n; i++) {
        seq[i] = (int)(termoAtual % 26);
        termoAtual *= pg_q;
    }
}

/* ============================================================================
 * BLOCO 3: MOTOR DE DESLOCAMENTO E CIFRAGEM / DECIFRAGEM
 * ============================================================================ */
char aplicaShift(char letra, int deslocamento) {
    int posicaoAlfabeto = (letra - 'a' + (deslocamento % 26) + 26) % 26;
    return (char)('a' + posicaoAlfabeto);
}

void processarPalavra(const char *entrada, int shift, const int seq[], int direcao, char *saida) {
    int tamanho = (int)strlen(entrada);
    for (int i = 0; i < tamanho; i++) {
        int deslocamentoCombinado = direcao * (shift + seq[i]);
        saida[i] = aplicaShift(entrada[i], deslocamentoCombinado);
    }
    saida[tamanho] = '\0';
}

/* ============================================================================
 * BLOCO 4: FORCA BRUTA COMPLETA (IMPRIME TODAS AS COMBINACOES DOS FOR)
 * Cruza todos os valores de:
 *   - shift: [shiftInicio .. shiftFim]
 *   - PG:    a de [pg_a_inicio .. pg_a_fim] e q de [pg_q_inicio .. pg_q_fim]
 *   - PA:    a de [pa_a_inicio .. pa_a_fim] e r de [pa_r_inicio .. pa_r_fim]
 *   - Alem de Primos e Fibonacci para cada SHIFT testado.
 * ============================================================================ */
void executarForcaBrutaCompleta(const char *palavra, int tamanho, int direcao,
                                int shiftInicio, int shiftFim,
                                int pg_a_inicio, int pg_a_fim, int pg_q_inicio, int pg_q_fim,
                                int pa_a_inicio, int pa_a_fim, int pa_r_inicio, int pa_r_fim) {
    int seq[MAX_LETRAS];
    char palavraResultado[MAX_LETRAS + 1];
    int totalCombinacoes = 0;

    if (direcao == 1) {
        printf("\n================ CIFRANDO TODAS AS VARIACOES ================\n");
    } else {
        printf("\n========== DECIFRANDO POR FORCA BRUTA (TODAS AS LINHAS) ==========\n");
    }

    for (int shift = shiftInicio; shift <= shiftFim; shift++) {
        printf("\n-------------------- [ SHIFT = %d ] --------------------\n", shift);

        /* 1. Numeros Primos */
        gerarPrimos(seq, tamanho);
        processarPalavra(palavra, shift, seq, direcao, palavraResultado);
        printf(" [SHIFT=%d | Primos]               -> %s\n", shift, palavraResultado);
        totalCombinacoes++;

        /* 2. Serie de Fibonacci */
        gerarFibonacci(seq, tamanho);
        processarPalavra(palavra, shift, seq, direcao, palavraResultado);
        printf(" [SHIFT=%d | Fibonacci]            -> %s\n", shift, palavraResultado);
        totalCombinacoes++;

        /* 3. Todas as variacoes de PG (termo inicial 'a' e razao 'q') */
        for (int a = pg_a_inicio; a <= pg_a_fim; a++) {
            for (int q = pg_q_inicio; q <= pg_q_fim; q++) {
                gerarPG(seq, tamanho, a, q);
                processarPalavra(palavra, shift, seq, direcao, palavraResultado);
                printf(" [SHIFT=%d | PG: a=%d, q=%d]        -> %s\n", shift, a, q, palavraResultado);
                totalCombinacoes++;
            }
        }

        /* 4. Todas as variacoes de PA (termo inicial 'a' e razao 'r') */
        for (int a = pa_a_inicio; a <= pa_a_fim; a++) {
            for (int r = pa_r_inicio; r <= pa_r_fim; r++) {
                gerarPA(seq, tamanho, a, r);
                processarPalavra(palavra, shift, seq, direcao, palavraResultado);
                printf(" [SHIFT=%d | PA: a=%d, r=%d]        -> %s\n", shift, a, r, palavraResultado);
                totalCombinacoes++;
            }
        }
    }

    printf("\n==================================================================\n");
    printf("Total de combinacoes geradas: %d linhas.\n", totalCombinacoes);
}

/* ============================================================================
 * BLOCO 5: PROGRAMA PRINCIPAL (LOOP DO-WHILE)
 * ============================================================================ */
int main(void) {
    char entradaBruta[64];
    char palavraNorm[MAX_LETRAS + 1];
    int opcaoModo, continuarExecucao;
    int shiftInicio, shiftFim;
    int pg_a_inicio, pg_a_fim, pg_q_inicio, pg_q_fim;
    int pa_a_inicio, pa_a_fim, pa_r_inicio, pa_r_fim;

    do {
        printf("\n============================================================\n");
        printf("=== CIFRA HIBRIDA: SHIFT FIXO + SEQUENCIA NUMERICA (ALGPC) ===\n");
        printf("============================================================\n");

        printf("Digite a palavra (max. %d letras): ", MAX_LETRAS);
        fgets(entradaBruta, sizeof(entradaBruta), stdin);
        entradaBruta[strcspn(entradaBruta, "\r\n")] = '\0';

        int tamanhoPalavra = normalizarPalavra(entradaBruta, palavraNorm);
        if (tamanhoPalavra == 0) {
            printf("Erro: nenhuma letra valida encontrada. Tente novamente.\n");
            continuarExecucao = 1;
            continue;
        }

        printf("Palavra normalizada: %s (%d letras)\n\n", palavraNorm, tamanhoPalavra);

        printf("Escolha a operacao:\n");
        printf(" 1 - Cifrar\n");
        printf(" 2 - Decifrar (Forca Bruta)\n");
        printf("Opcao: ");
        scanf("%d", &opcaoModo);

        if (opcaoModo != 1 && opcaoModo != 2) {
            printf("Opcao invalida! Digite 1 ou 2.\n");
            limparBuffer();
            continuarExecucao = 1;
            continue;
        }

        /* Leitura do intervalo do SHIFT */
        printf("\n--- Configuracao do SHIFT (se for fixo, digite o mesmo numero nos dois) ---\n");
        printf("SHIFT inicial (ex: 1): ");
        scanf("%d", &shiftInicio);
        printf("SHIFT final   (ex: 5): ");
        scanf("%d", &shiftFim);
        garantirOrdem(&shiftInicio, &shiftFim);

        /* Leitura dos intervalos da PG (a e q) */
        printf("\n--- Configuracao da PG (termo inicial 'a' e razao 'q') ---\n");
        printf("PG -> 'a' inicial (ex: 4): ");
        scanf("%d", &pg_a_inicio);
        printf("PG -> 'a' final   (ex: 8): ");
        scanf("%d", &pg_a_fim);
        garantirOrdem(&pg_a_inicio, &pg_a_fim);

        printf("PG -> 'q' inicial (ex: 2): ");
        scanf("%d", &pg_q_inicio);
        printf("PG -> 'q' final   (ex: 6): ");
        scanf("%d", &pg_q_fim);
        garantirOrdem(&pg_q_inicio, &pg_q_fim);

        /* Leitura dos intervalos da PA (a e r) */
        printf("\n--- Configuracao da PA (termo inicial 'a' e razao 'r') ---\n");
        printf("PA -> 'a' inicial (ex: 6): ");
        scanf("%d", &pa_a_inicio);
        printf("PA -> 'a' final   (ex: 8): ");
        scanf("%d", &pa_a_fim);
        garantirOrdem(&pa_a_inicio, &pa_a_fim);

        printf("PA -> 'r' inicial (ex: 3): ");
        scanf("%d", &pa_r_inicio);
        printf("PA -> 'r' final   (ex: 7): ");
        scanf("%d", &pa_r_fim);
        garantirOrdem(&pa_r_inicio, &pa_r_fim);

        int direcao = (opcaoModo == 1) ? 1 : -1;
        executarForcaBrutaCompleta(palavraNorm, tamanhoPalavra, direcao,
                                   shiftInicio, shiftFim,
                                   pg_a_inicio, pg_a_fim, pg_q_inicio, pg_q_fim,
                                   pa_a_inicio, pa_a_fim, pa_r_inicio, pa_r_fim);

        printf("\n------------------------------------------------------------\n");
        printf("Deseja testar outra palavra ou decifrar? (1 - Sim / 0 - Sair): ");
        scanf("%d", &continuarExecucao);
        limparBuffer();

    } while (continuarExecucao == 1);

    printf("Programa encerrado.\n");
    return 0;
}
