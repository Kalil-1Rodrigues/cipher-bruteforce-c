/*
 * ============================================================================
 * Atividade 3 - Cifra Hibrida com Shift Fixo e Sequencia Numerica
 * Disciplina: Algoritmos e Pensamento Computacional (ALGPC)
 *
 * Descricao Geral:
 *   PRACIMA DELEEEEEEEEEEEEES
 *   Implementa uma cifra polialfabetica hibrida que soma um deslocamento fixo
 *   (SHIFT, estilo Cifra de Cesar) ao i-esimo termo de uma sequencia numerica
 *   (seq[i]), seguindo a formula: nova_letra = aplicaShift(letra, SHIFT + seq[i]).
 *   O deslocamento variavel por posicao evita que letras iguais na entrada
 *   gerem a mesma letra cifrada, dificultando a analise de frequencia.
 *
 * Convencoes Tecnicas:
 *   - O indice seq[0] corresponde ao primeiro termo matematico (n = 1).
 *   - PA e PG aplicam reducao modular (% 26) iterativa para evitar overflow.
 *   - Padrao C89/C99 estrito para compatibilidade total com Dev-C++ e GCC.
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h> /* Conversao numerica e controle (strtol, exit, atexit)     */
#include <string.h> /* Manipulacao e tamanho de strings (strlen, strcpy)        */
#include <ctype.h> /* Verificacao e conversao de caracteres (isalpha, tolower) */

// https://en.cppreference.com/w/c/header PARA AS LIBS


#ifdef _WIN32
#include <windows.h> /* Configura console do Windows para UTF-8 (SetConsoleCP)   */
#endif // NO linux E MacOS NÃO PRECISAM!!

#define MAX_LETRAS 15            /* Limite maximo de letras da palavra       */
#define TAM_LINHA 256            /* Tamanho do buffer de leitura de linha    */
#define LIMITE_PARAM 10000       /* Valores de SHIFT, a1, r, q: -10000..10000 */
#define LIMITE_LINHAS 1000000.0  /* Acima disso, pede confirmacao ao usuario */

/* ============================================================================
 * BLOCO 0: ENTRADA SEGURA, SANITIZACAO DE BUFFER E CONFIGURACAO DE CONSOLE
 * Evita falhas clasicas do scanf (como loops infinitos ao digitar letras ou
 * sobras de '\n' no buffer) lendo toda entrada via fgets e validando com strtol.
 * No Windows, ajusta o codepage para UTF-8 (65001) para preservar acentos.
 * ============================================================================ */
 
#ifdef _WIN32
static UINT cpEntradaOriginal;
static void restaurarConsole(void) {
    SetConsoleCP(cpEntradaOriginal);
}
#endif

static void prepararConsole(void) {
#ifdef _WIN32
    cpEntradaOriginal = GetConsoleCP();
    if (SetConsoleCP(65001)) {
        atexit(restaurarConsole);
    }
#endif
} 
// TODAS ESSAS FUNÇÕES ESTÃO NO:
// https://learn.microsoft.com/pt-br/windows/console/setconsolecp
// simplismente para o programa conseguir normalizar palavras 
// tanto cifradas e sem-cifra para utilizar sem problemas


/* Le uma linha inteira com fgets, remove o '\n'/'\r' e descarta o excesso.
 * Se a entrada acabar (EOF), encerra o programa de forma limpa. */
void lerLinha(char *buffer, int tamanho) {
    size_t len;
    int c;

    if (fgets(buffer, tamanho, stdin) == NULL) {
        printf("\nEntrada encerrada. Programa finalizado.\n");
        exit(0);
    }

    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        /* linha maior que o buffer: descarta o resto para nao sobrar lixo */
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\r') {
        buffer[len - 1] = '\0';
    }
}

/* Le um inteiro validado (repete ate o usuario digitar um valor valido). */
void lerInteiro(const char *mensagem, int *valor, int minimo, int maximo) {
    char linha[TAM_LINHA];
    char *fim;
    long numero;

    while (1) {
        printf("%s", mensagem);
        lerLinha(linha, (int)sizeof(linha));

        numero = strtol(linha, &fim, 10);
        while (*fim == ' ' || *fim == '\t') {
            fim++;
        }

        if (fim == linha || *fim != '\0') {
            printf("Entrada invalida: digite um numero inteiro.\n");
            continue;
        }
        if (numero < minimo || numero > maximo) {
            printf("Valor fora da faixa permitida (%d a %d).\n", minimo, maximo);
            continue;
        }

        *valor = (int)numero;
        return;
    }
}

/* Funcao auxiliar: garante que 'minimo' seja menor ou igual a 'maximo' */
void garantirOrdem(int *minimo, int *maximo) {
    if (*minimo > *maximo) {
        int aux = *minimo;
        *minimo = *maximo;
        *maximo = aux;
    }
}

/* ============================================================================
 * BLOCO 1: NORMALIZACAO E PREPARACAO DA PALAVRA
 * Atende aos requisitos de entrada da atividade: converte maiusculas para
 * minusculas ('a'..'z'), remove espacos, hifens, numeros e pontuacao,
 * converte caracteres acentuados e 'ç' (UTF-8 e Latin-1) para ASCII simples
 * e limita a string final ao maximo de 15 caracteres (MAX_LETRAS).
 * ============================================================================ */
 
char removerAcento(unsigned int codigo) {
    if (codigo < 0xC0 || codigo > 0xFF) return '\0';
    codigo = codigo | 0x20; /* maiuscula acentuada -> minuscula acentuada */

    if (codigo >= 0xE0 && codigo <= 0xE5) return 'a';
    if (codigo == 0xE7)                   return 'c';
    if (codigo >= 0xE8 && codigo <= 0xEB) return 'e';
    if (codigo >= 0xEC && codigo <= 0xEF) return 'i';
    if (codigo == 0xF1)                   return 'n';
    if (codigo >= 0xF2 && codigo <= 0xF6) return 'o';
    if (codigo >= 0xF9 && codigo <= 0xFC) return 'u';
    return '\0';
}

/* Retorna a quantidade de letras gravadas em 'saida'.
 * *truncada recebe 1 se havia mais de MAX_LETRAS letras na entrada. */
int normalizarPalavra(const char *entrada, char *saida, int *truncada) {
    int i = 0;
    int tamanho = 0;
    unsigned char c;
    unsigned char proximo;
    unsigned int codigo;
    char letra;

    *truncada = 0;

    while (entrada[i] != '\0') {
        c = (unsigned char)entrada[i];
        codigo = c;
        letra = '\0';
        i++;

        /* UTF-8: letras acentuadas ocupam 2 bytes (0xC3 + byte 0x80..0xBF) */
        proximo = (unsigned char)entrada[i];
        if (c == 0xC3 && proximo >= 0x80 && proximo <= 0xBF) {
            codigo = 0xC0 + (proximo & 0x3F);
            i++;
        }

        if (codigo < 0x80) {
            if (isalpha((int)codigo)) {
                letra = (char)tolower((int)codigo);
            }
        } else {
            letra = removerAcento(codigo);
        }

        if (letra >= 'a' && letra <= 'z') {
            if (tamanho < MAX_LETRAS) {
                saida[tamanho++] = letra;
            } else {
                *truncada = 1;
            }
        }
    }

    saida[tamanho] = '\0';
    return tamanho;
}

/* Pede a palavra ao usuario ate obter ao menos uma letra valida. */
int lerPalavra(const char *mensagem, char *saida) {
    char linha[TAM_LINHA];
    int tamanho;
    int truncada;

    while (1) {
        printf("%s (max. %d letras): ", mensagem, MAX_LETRAS);
        lerLinha(linha, (int)sizeof(linha));

        tamanho = normalizarPalavra(linha, saida, &truncada);
        if (tamanho == 0) {
            printf("Erro: nenhuma letra valida encontrada. Tente novamente.\n");
            continue;
        }

        printf("Palavra normalizada: %s (%d letras)\n", saida, tamanho);
        if (truncada) {
            printf("Aviso: a palavra tinha mais de %d letras; apenas as %d primeiras foram usadas.\n",
                   MAX_LETRAS, MAX_LETRAS);
        }
        return tamanho;
    }
}

/* ============================================================================
 * BLOCO 2: GERADORES DE SEQUENCIAS NUMERICAS (seq[i])
 * ============================================================================ */


/* Gera os 'n' primeiros Numeros Primos (2, 3, 5, 7, 11...), testando divisores
 * ate a raiz quadrada do candidato (divisor * divisor <= numeroAtual). */
void gerarPrimos(int seq[], int n) {
    int numeroAtual = 2;
    int encontrados = 0;
    int divisor;
    int ehPrimo;

    while (encontrados < n) {
        ehPrimo = 1;
        for (divisor = 2; divisor * divisor <= numeroAtual; divisor++) {
            if (numeroAtual % divisor == 0) {
                ehPrimo = 0;
                break;
            }
        }
        if (ehPrimo) {
            seq[encontrados++] = numeroAtual;
        }
        numeroAtual++;
    }
}

/* Gera os 'n' primeiros termos da Serie de Fibonacci (1, 1, 2, 3, 5, 8...),
 * onde cada termo a partir do terceiro e a soma dos dois anteriores. */
 
void gerarFibonacci(int seq[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (i < 2) seq[i] = 1;
        else       seq[i] = seq[i - 1] + seq[i - 2];
    }
}

/* Gera Progressao Aritmetica pela formula an = a1 + (n - 1) * r (com i = n - 1),
 * reduzindo modulo 26 para manter os deslocamentos dentro do alfabeto. */
 
void gerarPA(int seq[], int n, int a1, int r) {
    int i;
    for (i = 0; i < n; i++) {
        seq[i] = ((a1 + i * r) % 26 + 26) % 26;
    }
}

/* Gera Progressao Geometrica pela formula an = a1 * q^(n - 1). Aplica modulo 26
 * a cada multiplicacao (aritmetica modular) para impedir integer overflow em
 * potencias altas sem alterar o resultado final da cifragem. */
 
void gerarPG(int seq[], int n, int a1, int q) {
    int termo = ((a1 % 26) + 26) % 26;
    int razao = ((q % 26) + 26) % 26;
    int i;

    for (i = 0; i < n; i++) {
        seq[i] = termo;
        termo = (termo * razao) % 26;
    }
}

/* ============================================================================
 * BLOCO 3: MOTOR CRIPTOGRAFICO (ARITMETICA MODULAR CIRCULAR)
 * - aplicaShift: converte a letra para indice [0..25], soma o deslocamento e
 *   aplica ((x % 26) + 26) % 26 para garantir rotacao circular positiva tanto
 *   no avanco (cifragem) quanto no retrocesso (decifragem com valores negativos).
 * - processarPalavra: aplica direcao * ((shift % 26) + seq[i]) em cada letra,
 *   usando direcao = +1 para cifrar e direcao = -1 para decifrar.
 * ============================================================================ */
 
char aplicaShift(char letra, int deslocamento) {
    int posicaoAlfabeto = (letra - 'a' + (deslocamento % 26) + 26) % 26;
    return (char)('a' + posicaoAlfabeto);
}

/* direcao =  1 -> Cifrar (avanca no alfabeto)
 * direcao = -1 -> Decifrar (retrocede no alfabeto) */
void processarPalavra(const char *entrada, int shift, const int seq[], int direcao, char *saida) {
    int tamanho = (int)strlen(entrada);
    int i;
    int deslocamentoCombinado;

    for (i = 0; i < tamanho; i++) {
        deslocamentoCombinado = direcao * ((shift % 26) + seq[i]);
        saida[i] = aplicaShift(entrada[i], deslocamentoCombinado);
    }
    saida[tamanho] = '\0';
}

/* ============================================================================
 * BLOCO 4: ANALISE POR FORCA BRUTA (DECIFRAGEM EXAUSTIVA)
 * Percorre todo o espaco de chaves informado pelo usuario (intervalo de SHIFT,
 * sequencias fixas Fibonacci/Primos e lacos aninhados para PA [a1, r] e PG [a1, q]),
 * aplicando direcao = -1 para reverter a cifra e exibir todas as hipoteses.
 * Inclui estimativa previa para alertar caso a saida ultrapasse 1 milhao de linhas.
 * ============================================================================ */
 
/* Estima quantas linhas a forca bruta vai imprimir (double evita overflow). */

double estimarCombinacoes(int shiftMin, int shiftMax,
                          int pa_a1_min, int pa_a1_max, int pa_r_min, int pa_r_max,
                          int pg_a1_min, int pg_a1_max, int pg_q_min, int pg_q_max) {
    double nShift = (double)shiftMax - shiftMin + 1.0;
    double nPA = ((double)pa_a1_max - pa_a1_min + 1.0) * ((double)pa_r_max - pa_r_min + 1.0);
    double nPG = ((double)pg_a1_max - pg_a1_min + 1.0) * ((double)pg_q_max - pg_q_min + 1.0);
    return nShift * (2.0 + nPA + nPG); /* 2 = Fibonacci + Primos */
}

void decifrarForcaBruta(const char *cifrada, int tamanho,
                        int shiftMin, int shiftMax,
                        int pa_a1_min, int pa_a1_max, int pa_r_min, int pa_r_max,
                        int pg_a1_min, int pg_a1_max, int pg_q_min, int pg_q_max) {
    int seq[MAX_LETRAS];
    char tentativa[MAX_LETRAS + 1];
    double totalCombinacoes = 0.0;
    int shift, a1, r, q;

    printf("\n========== DECIFRANDO POR FORCA BRUTA (TODAS AS COMBINACOES) ==========\n");

    for (shift = shiftMin; shift <= shiftMax; shift++) {
        printf("\n-------------------- [ SHIFT = %d ] --------------------\n", shift);

        /* 1. Fibonacci */
        gerarFibonacci(seq, tamanho);
        processarPalavra(cifrada, shift, seq, -1, tentativa);
        printf(" [SHIFT=%d | Fibonacci]            -> %s\n", shift, tentativa);
        totalCombinacoes += 1.0;

        /* 2. Numeros Primos */
        gerarPrimos(seq, tamanho);
        processarPalavra(cifrada, shift, seq, -1, tentativa);
        printf(" [SHIFT=%d | Numeros Primos]       -> %s\n", shift, tentativa);
        totalCombinacoes += 1.0;

        /* 3. Todas as variacoes de PA (a1min..a1max e rmin..rmax) */
        for (a1 = pa_a1_min; a1 <= pa_a1_max; a1++) {
            for (r = pa_r_min; r <= pa_r_max; r++) {
                gerarPA(seq, tamanho, a1, r);
                processarPalavra(cifrada, shift, seq, -1, tentativa);
                printf(" [SHIFT=%d | PA: a1=%d, r=%d]       -> %s\n", shift, a1, r, tentativa);
                totalCombinacoes += 1.0;
            }
        }

        /* 4. Todas as variacoes de PG (a1min..a1max e qmin..qmax) */
        for (a1 = pg_a1_min; a1 <= pg_a1_max; a1++) {
            for (q = pg_q_min; q <= pg_q_max; q++) {
                gerarPG(seq, tamanho, a1, q);
                processarPalavra(cifrada, shift, seq, -1, tentativa);
                printf(" [SHIFT=%d | PG: a1=%d, q=%d]       -> %s\n", shift, a1, q, tentativa);
                totalCombinacoes += 1.0;
            }
        }
    }

    printf("\n=======================================================================\n");
    printf("Total de combinacoes testadas: %.0f linhas.\n", totalCombinacoes);
}

/* ============================================================================
 * BLOCO 5: CONTROLADORES DOS MODOS DE OPERACAO
 * - modoCifrar: coleta palavra, SHIFT unico e parametros de uma unica sequencia
 *   escolhida pelo grupo para gerar o texto cifrado oficial.
 * - modoDecifrar: coleta palavra cifrada e intervalos de busca (shiftMin..Max,
 *   PA e PG) para quebrar mensagens cifradas sem conhecimento previo da sequencia.
 * ============================================================================ */

/* Pede um intervalo [minimo, maximo]; se vier invertido, troca os valores. */
void lerIntervalo(const char *msgMin, const char *msgMax, int *minimo, int *maximo) {
    lerInteiro(msgMin, minimo, -LIMITE_PARAM, LIMITE_PARAM);
    lerInteiro(msgMax, maximo, -LIMITE_PARAM, LIMITE_PARAM);
    garantirOrdem(minimo, maximo);
}

void modoCifrar(void) {
    char palavra[MAX_LETRAS + 1];
    char palavraCifrada[MAX_LETRAS + 1];
    char descricaoSeq[64];
    int seq[MAX_LETRAS];
    int tamanho, shift, tipoSeq;
    int a1, razao;

    tamanho = lerPalavra("Digite a palavra a ser cifrada", palavra);

    lerInteiro("Digite o valor do SHIFT (deslocamento): ", &shift, -LIMITE_PARAM, LIMITE_PARAM);

    printf("\nEscolha a sequencia numerica:\n");
    printf(" 1 - Fibonacci\n");
    printf(" 2 - Progressao Aritmetica (PA)\n");
    printf(" 3 - Progressao Geometrica (PG)\n");
    printf(" 4 - Numeros Primos\n");
    lerInteiro("Sequencia: ", &tipoSeq, 1, 4);

    if (tipoSeq == 1) {
        gerarFibonacci(seq, tamanho);
        strcpy(descricaoSeq, "Fibonacci");
    } else if (tipoSeq == 2) {
        lerInteiro("Informe o termo inicial (a1) da PA: ", &a1, -LIMITE_PARAM, LIMITE_PARAM);
        lerInteiro("Informe a razao (r) da PA: ", &razao, -LIMITE_PARAM, LIMITE_PARAM);
        gerarPA(seq, tamanho, a1, razao);
        sprintf(descricaoSeq, "PA (a1 = %d, r = %d)", a1, razao);
    } else if (tipoSeq == 3) {
        lerInteiro("Informe o termo inicial (a1) da PG: ", &a1, -LIMITE_PARAM, LIMITE_PARAM);
        lerInteiro("Informe a razao (q) da PG: ", &razao, -LIMITE_PARAM, LIMITE_PARAM);
        gerarPG(seq, tamanho, a1, razao);
        sprintf(descricaoSeq, "PG (a1 = %d, q = %d)", a1, razao);
    } else {
        gerarPrimos(seq, tamanho);
        strcpy(descricaoSeq, "Numeros Primos");
    }

    processarPalavra(palavra, shift, seq, 1, palavraCifrada);

    printf("\n================ RESULTADO DA CIFRAGEM ================\n");
    printf("Palavra original : %s\n", palavra);
    printf("Palavra cifrada  : %s\n", palavraCifrada);
    printf("Parametros       : Tamanho = %d | SHIFT = %d\n", tamanho, shift);
    printf("Sequencia        : %s\n", descricaoSeq);
    printf("=======================================================\n");
}

void modoDecifrar(void) {
    char cifrada[MAX_LETRAS + 1];
    int tamanho, confirmar;
    int shiftMin, shiftMax;
    int pa_a1_min, pa_a1_max, pa_r_min, pa_r_max;
    int pg_a1_min, pg_a1_max, pg_q_min, pg_q_max;
    double total;

    tamanho = lerPalavra("Digite a palavra CIFRADA", cifrada);

    printf("\n--- Intervalo do SHIFT (se souber o valor exato, repita-o nos dois) ---\n");
    lerIntervalo("SHIFT minimo (shiftMin): ", "SHIFT maximo (shiftMax): ", &shiftMin, &shiftMax);

    printf("\n--- Intervalos da Progressao Aritmetica (PA) ---\n");
    lerIntervalo("PA -> a1 minimo (a1min): ", "PA -> a1 maximo (a1max): ", &pa_a1_min, &pa_a1_max);
    lerIntervalo("PA -> r minimo  (rmin) : ", "PA -> r maximo  (rmax) : ", &pa_r_min, &pa_r_max);

    printf("\n--- Intervalos da Progressao Geometrica (PG) ---\n");
    lerIntervalo("PG -> a1 minimo (a1min): ", "PG -> a1 maximo (a1max): ", &pg_a1_min, &pg_a1_max);
    lerIntervalo("PG -> q minimo  (qmin) : ", "PG -> q maximo  (qmax) : ", &pg_q_min, &pg_q_max);

    total = estimarCombinacoes(shiftMin, shiftMax,
                               pa_a1_min, pa_a1_max, pa_r_min, pa_r_max,
                               pg_a1_min, pg_a1_max, pg_q_min, pg_q_max);

    if (total > LIMITE_LINHAS) {
        printf("\nAtencao: esses intervalos geram cerca de %.0f linhas de saida.\n", total);
        lerInteiro("Deseja continuar mesmo assim? (1 - Sim / 0 - Nao): ", &confirmar, 0, 1);
        if (confirmar != 1) {
            printf("Operacao cancelada. Reduza os intervalos e tente novamente.\n");
            return;
        }
    }

    decifrarForcaBruta(cifrada, tamanho,
                       shiftMin, shiftMax,
                       pa_a1_min, pa_a1_max, pa_r_min, pa_r_max,
                       pg_a1_min, pg_a1_max, pg_q_min, pg_q_max);
}

/* ============================================================================
 * FINALMENTES!!
 * BLOCO 6: PROGRAMA PRINCIPAL (ORQUESTRACAO EM LOOP)
 * Inicializa o console e mantem o menu principal em laco do-while, permitindo
 * cifrar uma palavra e validar sua decifragem imediatamente na mesma execucao.
 * ============================================================================ */
 
int main(void) {
    int opcaoModo;
    int continuarExecucao;

    prepararConsole();

    do {
        printf("\n============================================================\n");
        printf("=== CIFRA HIBRIDA: SHIFT FIXO + SEQUENCIA NUMERICA (ALGPC) ===\n");
        printf("============================================================\n");
        printf("Escolha a operacao:\n");
        printf(" 1 - Cifrar palavra\n");
        printf(" 2 - Decifrar palavra (Forca Bruta)\n");
        lerInteiro("Opcao: ", &opcaoModo, 1, 2);

        if (opcaoModo == 1) {
            modoCifrar();
        } else {
            modoDecifrar();
        }

        printf("\n------------------------------------------------------------\n");
        lerInteiro("Deseja realizar outra operacao (cifrar/decifrar)? (1 - Sim / 0 - Sair): ",
                   &continuarExecucao, 0, 1);

    } while (continuarExecucao == 1);

    printf("Programa encerrado.\n");
    return 0;
}
