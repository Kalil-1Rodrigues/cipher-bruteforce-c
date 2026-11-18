# Cifra Híbrida — Shift Fixo + Sequências Numéricas

Implementação em linguagem **C** de um sistema interativo de criptografia clássica híbrida e análise por força bruta, desenvolvido como parte da **Atividade 3** da disciplina de **Algoritmos e Pensamento Computacional (ALGPC)**.

---

## Sobre o Projeto

Na Cifra de César tradicional, todas as letras de uma mensagem são deslocadas por um valor constante, o que mantém o padrão de frequência das letras e facilita a quebra do código. 

Para contornar essa limitação, este algoritmo aplica um **deslocamento variável por posição**, combinando um valor inteiro fixo (`SHIFT`) com o $i$-ésimo termo de uma sequência numérica (`seq[i]`):

$$\text{nova\_letra} = \text{aplicaShift}(\text{letra},\ \text{SHIFT} + \text{seq}[i])$$

Dessa forma, ocorrências repetidas de uma mesma letra na palavra original resultam em caracteres diferentes no texto cifrado.

### Dinâmica de Uso (Cifragem e Força Bruta)
1. **Cifragem:** O grupo escolhe uma palavra (até 15 letras), um valor de `SHIFT` e uma sequência numérica (Primos, Fibonacci, PA ou PG).
2. **Compartilhamento:** Apenas a palavra cifrada, o tamanho da palavra e o `SHIFT` são informados aos demais grupos.
3. **Decifragem (Força Bruta):** O programa recebe a palavra cifrada, o `SHIFT` e um intervalo de parâmetros $[a_1, r/q]$, gerando automaticamente todas as hipóteses de sequências para recuperar a palavra original.

---

## Funcionalidades Implementadas

- **Normalização de Entrada (`normalizarPalavra`):**
  - Converte letras maiúsculas para minúsculas (`a`–`z`).
  - Trata caracteres acentuados e `ç` codificados em **UTF-8** (ex.: `"coração"` $\rightarrow$ `"coracao"`).
  - Remove espaços, hífens e caracteres especiais (ex.: `"guarda-chuva"` $\rightarrow$ `"guardachuva"`).
  - Limita o tamanho da palavra normalizada a no máximo **15 caracteres** (`MAX_LETRAS`).
- **Geradores de Sequências Numéricas:**
  - **Números Primos** ($2, 3, 5, 7, 11, \dots$)
  - **Série de Fibonacci** ($1, 1, 2, 3, 5, 8, \dots$)
  - **Progressão Aritmética (PA)** com primeiro termo $a_1$ e razão $r$ configuráveis
  - **Progressão Geométrica (PG)** com primeiro termo $a_1$ e razão $q$ configuráveis
- **Aritmética Modular Circular (`aplicaShift` e `processarPalavra`):**
  - Garante rotação contínua no alfabeto de 26 letras tanto para avanço (`sinal = 1`, cifragem) quanto para retrocesso (`sinal = -1`, decifragem), evitando índices negativos no operador módulo (`% 26`).
- **Modo Interativo Completo (`executarTodasTecnicas`):**
  - Permite escolher entre **Cifrar** e **Decifrar (Força Bruta)**.
  - Executa Primos, Fibonacci e todas as combinações de PA ($a_1, r$) e PG ($a_1, q$) dentro do intervalo numérico informado pelo usuário.

---

## Como Compilar e Executar

### Pré-requisitos
- Compilador C (`gcc`, `clang` ou IDE como Dev-C++ / Code::Blocks).

### Compilação via terminal
```bash
gcc cifra_hibrida.c -o cifra_hibrida