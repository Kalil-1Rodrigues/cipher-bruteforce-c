/*
 * ============================================================================
 * Atividade 3 - Cifra Hibrida com Shift Fixo e Sequencia Numerica
 * Disciplina: Algoritmos e Pensamento Computacional (ALGPC)
 *
 * Objetivo do Projeto:
 *   Combinar um deslocamento fixo (SHIFT, estilo Cifra de Cesar) com o termo
 *   variavel de uma sequencia matematica (seq[i]) por meio da formula:
 *       nova_letra = aplicaShift(letra, SHIFT + seq[i])
 *   Isso impede que letras repetidas na palavra original gerem a mesma letra
 *   cifrada, aumentando a seguranca contra analise de frequencia.
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LETRAS 15 /* Limite maximo de caracteres exigido pela atividade */

/* Funcao auxiliar: limpa o '\n' (Enter) residual do buffer do teclado apos o scanf,
 * evitando que a leitura da proxima palavra no laco do-while seja pulada. */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ============================================================================
 * BLOCO 1: NORMALIZACAO DA ENTRADA
 * Prepara a palavra conforme as regras da atividade: apenas letras minusculas
 * de 'a' a 'z', sem espacos, hifens, acentos ou cedilha (maximo de 15 letras).
 * ============================================================================ */
int normalizarPalavra(const char *entrada, char *saida) {
    int j = 0;
    for (int i = 0; entrada[i] != '\0' && j < MAX_LETRAS; i++) {
        unsigned char c = (unsigned char)entrada[i];
        char letra = '\0';

        /* Caracteres acentuados e 'ç' em UTF-8 ocupam 2 bytes iniciados por 0xC3 */
        if (c == 0xC3 && entrada[i + 1] != '\0') {
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

        /* Descarta espacos, hifens e simbolos; mantem apenas 'a'..'z' */
        if (letra >= 'a' && letra <= 'z') {
            saida[j++] = letra;
        }
    }
    saida[j] = '\0';
    return j;
}

/* ============================================================================
 * BLOCO 2: GERADORES DE SEQUENCIAS NUMERICAS (seq[i])
 * Cada funcao preenche o vetor seq[] com 'n' termos (um para cada letra da
 * palavra), fornecendo o deslocamento variavel da cifra hibrida.
 * ============================================================================ */

/* Gera os 'n' primeiros Numeros Primos (ex: 2, 3, 5, 7, 11...) */
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

/* Gera uma Progressao Aritmetica (PA): soma constante da razao 'r' a partir de 'a1' */
void gerarPA(int seq[], int n, int termoInicial_a1, int razaoPA_r) {
    for (int i = 0; i < n; i++) {
        seq[i] = termoInicial_a1 + i * razaoPA_r;
    }
}

/* Gera uma Progressao Geometrica (PG): multiplicacao sucessiva pela razao 'q' */
void gerarPG(int seq[], int n, int termoInicial_a1, int razaoPG_q) {
    int termoAtual = termoInicial_a1;
    for (int i = 0; i < n; i++) {
        seq[i] = termoAtual;
        termoAtual *= razaoPG_q;
    }
}

/* ============================================================================
 * BLOCO 3: MOTOR DE DESLOCAMENTO E CIFRAGEM / DECIFRAGEM
 * Implementa a aritmetica modular circular no alfabeto de 26 letras ('a'..'z').
 * ============================================================================ */

/* Aplica o deslocamento total (SHIFT + seq[i]) garantindo rotacao circular [0..25] */
char aplicaShift(char letra, int deslocamento) {
    int posicaoAlfabeto = (letra - 'a' + (deslocamento % 26) + 26) % 26;
    return (char)('a' + posicaoAlfabeto);
}

/* Percorre a palavra aplicando a formula da cifra hibrida:
 *   direcao =  1 -> avanca no alfabeto (CIFRAR)
 *   direcao = -1 -> retrocede no alfabeto (DECIFRAR) */
void processarPalavra(const char *entrada, int shift, const int seq[], int direcao, char *saida) {
    int tamanho = (int)strlen(entrada);
    for (int i = 0; i < tamanho; i++) {
        int deslocamentoCombinado = direcao * (shift + seq[i]);
        saida[i] = aplicaShift(entrada[i], deslocamentoCombinado);
    }
    saida[tamanho] = '\0';
}

/* ============================================================================
 * BLOCO 4: EXECUCAO DAS SEQUENCIAS E FORCA BRUTA (PA E PG PARAMETRIZADAS)
 * Testa todas as sequencias previstas no projeto (Primos, Fibonacci, PA e PG).
 * Para PA e PG, cruza todas as combinacoes de termo inicial (a1) e razao (r ou q)
 * dentro do intervalo [inicioIntervaloPAPG .. fimIntervaloPAPG].
 * ============================================================================ */
void executarTodasSequencias(const char *palavra, int tamanho, int shift, int direcao,
                             int inicioIntervaloPAPG, int fimIntervaloPAPG) {
    int seq[MAX_LETRAS];
    char palavraProcessada[MAX_LETRAS + 1];

    if (direcao == 1) {
        printf("\n=== CIFRANDO \"%s\" (Tamanho: %d | SHIFT: %d) ===\n", palavra, tamanho, shift);
    } else {
        printf("\n=== DECIFRANDO POR FORCA BRUTA \"%s\" (Tamanho: %d | SHIFT: %d) ===\n", palavra, tamanho, shift);
    }

    /* 1. Teste com Numeros Primos */
    gerarPrimos(seq, tamanho);
    processarPalavra(palavra, shift, seq, direcao, palavraProcessada);
    printf(" [Numeros Primos]          -> %s\n", palavraProcessada);

    /* 2. Teste com Serie de Fibonacci */
    gerarFibonacci(seq, tamanho);
    processarPalavra(palavra, shift, seq, direcao, palavraProcessada);
    printf(" [Serie de Fibonacci]      -> %s\n", palavraProcessada);

    /* 3. Teste com todas as variacoes de Progressao Aritmetica (PA) no intervalo */
    printf("\n --- Progressao Aritmetica (PA): variando a1 e razao r de %d ate %d ---\n",
           inicioIntervaloPAPG, fimIntervaloPAPG);
    for (int termoInicial_a1 = inicioIntervaloPAPG; termoInicial_a1 <= fimIntervaloPAPG; termoInicial_a1++) {
        for (int razaoPA_r = inicioIntervaloPAPG; razaoPA_r <= fimIntervaloPAPG; razaoPA_r++) {
            gerarPA(seq, tamanho, termoInicial_a1, razaoPA_r);
            processarPalavra(palavra, shift, seq, direcao, palavraProcessada);
            printf(" [PA | a1=%d, razao r=%d]   -> %s\n", termoInicial_a1, razaoPA_r, palavraProcessada);
        }
    }

    /* 4. Teste com todas as variacoes de Progressao Geometrica (PG) no intervalo */
    printf("\n --- Progressao Geometrica (PG): variando a1 e razao q de %d ate %d ---\n",
           inicioIntervaloPAPG, fimIntervaloPAPG);
    for (int termoInicial_a1 = inicioIntervaloPAPG; termoInicial_a1 <= fimIntervaloPAPG; termoInicial_a1++) {
        for (int razaoPG_q = inicioIntervaloPAPG; razaoPG_q <= fimIntervaloPAPG; razaoPG_q++) {
            gerarPG(seq, tamanho, termoInicial_a1, razaoPG_q);
            processarPalavra(palavra, shift, seq, direcao, palavraProcessada);
            printf(" [PG | a1=%d, razao q=%d]   -> %s\n", termoInicial_a1, razaoPG_q, palavraProcessada);
        }
    }
}

/* ============================================================================
 * BLOCO 5: PROGRAMA PRINCIPAL (INTERFACE INTERATIVA EM LOOP)
 * Permite cifrar palavras do proprio grupo ou decifrar palavras de outros
 * grupos repetidamente ate o usuario optar por encerrar.
 * ============================================================================ */
int main(void) {
    char entradaBruta[64];
    char palavraNorm[MAX_LETRAS + 1];
    int opcaoModo, shift, inicioIntervaloPAPG, fimIntervaloPAPG, continuarExecucao;

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

        printf("Escolha a operacao desejada:\n");
        printf(" 1 - Cifrar palavra (gerar opcoes para enviar a outro grupo)\n");
        printf(" 2 - Decifrar palavra (forca bruta com tamanho e SHIFT conhecidos)\n");
        printf("Opcao: ");
        scanf("%d", &opcaoModo);

        if (opcaoModo != 1 && opcaoModo != 2) {
            printf("Opcao invalida! Digite 1 ou 2.\n");
            limparBuffer();
            continuarExecucao = 1;
            continue;
        }

        printf("Digite o valor do SHIFT fixo: ");
        scanf("%d", &shift);

        printf("\nConfiguracao das sequencias PA e PG (termo inicial a1 e razao r/q):\n");
        printf("Digite o valor INICIAL do intervalo para PA e PG (ex: 3): ");
        scanf("%d", &inicioIntervaloPAPG);
        printf("Digite o valor FINAL do intervalo para PA e PG (ex: 5): ");
        scanf("%d", &fimIntervaloPAPG);

        /* Garante que o inicio seja menor ou igual ao fim caso sejam invertidos */
        if (inicioIntervaloPAPG > fimIntervaloPAPG) {
            int auxiliar = inicioIntervaloPAPG;
            inicioIntervaloPAPG = fimIntervaloPAPG;
            fimIntervaloPAPG = auxiliar;
        }

        int direcao = (opcaoModo == 1) ? 1 : -1;
        executarTodasSequencias(palavraNorm, tamanhoPalavra, shift, direcao,
                                inicioIntervaloPAPG, fimIntervaloPAPG);

        if (opcaoModo == 1) {
            printf("\nDados da operação -> Tamanho: %d | SHIFT: %d\n",
                   tamanhoPalavra, shift);
        }

        printf("\n------------------------------------------------------------\n");
        printf("Deseja realizar outra cifragem ou decifragem? (1 - Sim / 0 - Sair): ");
        scanf("%d", &continuarExecucao);
        limparBuffer();

    } while (continuarExecucao == 1);

    printf("Programa encerrado.\n");
    return 0;
}
