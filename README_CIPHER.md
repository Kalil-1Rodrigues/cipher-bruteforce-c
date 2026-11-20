# Cifra Híbrida — Shift Fixo + Sequências Numéricas

Implementação em linguagem **C** (padrão C89/C99) de um sistema interativo de criptografia clássica híbrida e análise por força bruta, desenvolvido como parte da **Atividade 3** da disciplina de **Algoritmos e Pensamento Computacional (ALGPC)**.

---

## Sobre o Projeto

Na Cifra de César tradicional, todas as letras de uma mensagem são deslocadas por um valor constante, o que preserva o padrão de frequência dos caracteres e facilita a quebra do código.

Para contornar essa limitação, este algoritmo aplica um **deslocamento variável por posição**, combinando um valor inteiro fixo (`SHIFT`) com o $i$-ésimo termo de uma sequência numérica (`seq[i]`):

$$\text{nova\_letra} = \text{aplicaShift}(\text{letra},\ \text{SHIFT} + \text{seq}[i])$$

Dessa forma, ocorrências repetidas de uma mesma letra na palavra original resultam em caracteres diferentes no texto cifrado.

### Dinâmica de Uso (Cifragem e Força Bruta)
1. **Cifragem (`modoCifrar`):** O grupo informa uma palavra (até 15 letras), um valor único de `SHIFT` e escolhe uma sequência numérica (Fibonacci, PA, PG ou Números Primos) para gerar a palavra cifrada.
2. **Compartilhamento:** Apenas a palavra cifrada, o tamanho da palavra e o `SHIFT` (ou faixa de busca) são informados aos demais grupos.
3. **Decifragem por Força Bruta (`modoDecifrar`):** O programa recebe a palavra cifrada, o intervalo de `SHIFT` e os intervalos de parâmetros $[a_1, r]$ (PA) e $[a_1, q]$ (PG), gerando todas as hipóteses possíveis até revelar a palavra original.

---

## Histórico de Evolução (O que foi Adicionado e Removido)

### O que foi adicionado na versão final
- **Entrada Segura e Validação (`lerLinha` e `lerInteiro`):** Toda leitura do teclado passou a utilizar `fgets` combinado com `strtol` e verificação de faixa (`-10000` a `10000`), impedindo travamentos ou loops infinitos caso o usuário digite letras ou símbolos em campos numéricos.
- **Suporte Nativo a Acentos UTF-8 no Windows (`prepararConsole` / `restaurarConsole`):** Inclusão condicional de `<windows.h>` (`#ifdef _WIN32`) para configurar a página de código do terminal para UTF-8 (`SetConsoleCP(65001)`) e restaurar a configuração original ao encerrar (`atexit`), mantendo compatibilidade total com Linux e macOS.
- **Normalização Estendida e Aviso de Corte (`removerAcento` e `lerPalavra`):** Suporte tanto à codificação UTF-8 quanto a Latin-1/CP1252, além de aviso explícito quando a palavra digitada ultrapassa o limite de 15 letras (`MAX_LETRAS`).
- **Trava de Segurança na Força Bruta (`estimarCombinacoes`):** Calcula previamente a quantidade total de combinações e solicita confirmação antes de imprimir caso a saída ultrapasse `1.000.000` de linhas (`LIMITE_LINHAS`).
- **Compatibilidade C89/C99:** Variáveis declaradas no início de cada bloco para garantir compilação limpa tanto em compiladores modernos (GCC/Clang) quanto em IDEs acadêmicas mais antigas (Dev-C++).

### O que foi removido ou substituído
- **Remoção do `scanf` direto e da função `limparBuffer()`:** Substituídos integralmente por leitura de linha (`fgets`) e conversão controlada (`strtol`), eliminando bugs de `\n` residual no buffer do teclado.
- **Substituição do `long long` na PG (`gerarPG`):** Removida a dependência de inteiros de 64 bits; o cálculo agora aplica aritmética modular (`% 26`) a cada multiplicação, impedindo *integer overflow* independentemente do tamanho da razão $q$.
- **Remoção da função híbrida única (`executarTodasTecnicas`):** Dividida em fluxos independentes (`modoCifrar`, `modoDecifrar` e `decifrarForcaBruta`), evitando que o modo de cifragem solicitasse intervalos desnecessários.
- **Remoção de testes fixos (*hardcoded*) e extensões `.cpp`:** Eliminados dados chumbados no `main` e padronizado o código-fonte em C puro (`.c`), sem uso de bibliotecas não portáveis como `<conio.h>` ou comandos `system()`.

---

## Funcionalidades Implementadas

- **Normalização de Entrada (`normalizarPalavra`):**
  - Converte maiúsculas para minúsculas (`a`–`z`).
  - Converte vogais acentuadas e `ç` em UTF-8 ou Latin-1 para ASCII simples (ex.: `"coração"` $\rightarrow$ `"coracao"`).
  - Remove espaços, hífens, números e caracteres especiais (ex.: `"guarda-chuva"` $\rightarrow$ `"guardachuva"`).
  - Limita a saída a no máximo **15 caracteres** (`MAX_LETRAS`).
- **Geradores de Sequências Numéricas (`seq[i]`):**
  - **Série de Fibonacci:** $1, 1, 2, 3, 5, 8, \dots$
  - **Progressão Aritmética (PA):** $a_n = a_1 + (n - 1) \cdot r$
  - **Progressão Geométrica (PG):** $a_n = a_1 \cdot q^{n - 1}$ (com redução modular iterativa $\bmod\ 26$)
  - **Números Primos:** $2, 3, 5, 7, 11, \dots$
- **Aritmética Modular Circular (`aplicaShift` e `processarPalavra`):**
  - Garante rotação contínua no alfabeto de 26 letras tanto no avanço (`direcao = 1`, cifragem) quanto no retrocesso (`direcao = -1`, decifragem), evitando resultados negativos no operador módulo (`% 26`).

---

## Como Compilar e Executar

### Pré-requisitos
- Compilador C (`gcc`, `clang` ou IDE como Dev-C++ / Code::Blocks).

### Compilação via terminal
```bash
gcc cifra_decifra.c -o cifra_decifra
```

### Execução
- **Linux / macOS:**
  ```bash
  ./cifra_decifra
  ```
- **Windows:**
  ```cmd
  cifra_decifra.exe
  ```

---

## Exemplo de Uso

```text
============================================================
=== CIFRA HIBRIDA: SHIFT FIXO + SEQUENCIA NUMERICA (ALGPC) ===
============================================================
Escolha a operacao:
 1 - Cifrar palavra
 2 - Decifrar palavra (Forca Bruta)
Opcao: 1
Digite a palavra a ser cifrada (max. 15 letras): cabeca
Palavra normalizada: cabeca (6 letras)
Digite o valor do SHIFT (deslocamento): 5

Escolha a sequencia numerica:
 1 - Fibonacci
 2 - Progressao Aritmetica (PA)
 3 - Progressao Geometrica (PG)
 4 - Numeros Primos
Sequencia: 3
Informe o termo inicial (a1) da PG: 2
Informe a razao (q) da PG: 3

================ RESULTADO DA CIFRAGEM ================
Palavra original : cabeca
Palavra cifrada  : jlylnx
Parametros       : Tamanho = 6 | SHIFT = 5
Sequencia        : PG (a1 = 2, q = 3)
=======================================================
```

Para decifrar `"jlylnx"`, basta selecionar `2 - Decifrar palavra (Forca Bruta)`, informar os intervalos desejados (ex.: `SHIFT` de `4` a `6`, `PG` com $a_1 \in [2, 3]$ e $q \in [2, 3]$) e localizar a linha `[SHIFT=5 | PG: a1=2, q=3] -> cabeca`.

---

## Estrutura Modular do Código

| Bloco | Funções Principais | Responsabilidade |
| :--- | :--- | :--- |
| **Bloco 0** | `prepararConsole`, `lerLinha`, `lerInteiro`, `garantirOrdem` | Configura UTF-8 no Windows, valida entradas numéricas e ordena limites de intervalo. |
| **Bloco 1** | `removerAcento`, `normalizarPalavra`, `lerPalavra` | Sanitiza a string de entrada (UTF-8/Latin-1 para ASCII minúsculo, sem pontuação, máx. 15 letras). |
| **Bloco 2** | `gerarPrimos`, `gerarFibonacci`, `gerarPA`, `gerarPG` | Preenchem o vetor `seq[]` com os $n$ termos da sequência escolhida. |
| **Bloco 3** | `aplicaShift`, `processarPalavra` | Executa o deslocamento modular circular (`direcao * ((shift % 26) + seq[i])`). |
| **Bloco 4** | `estimarCombinacoes`, `decifrarForcaBruta` | Realiza a busca exaustiva nos intervalos de `SHIFT`, Fibonacci, Primos, PA e PG. |
| **Bloco 5** | `lerIntervalo`, `modoCifrar`, `modoDecifrar` | Controla a coleta de parâmetros e a exibição dos resultados de cada operação. |
| **Bloco 6** | `main` | Orquestra o menu principal em laço `do-while` para múltiplas execuções seguidas. |