# Projeto Sequence Set

## Descrição

Este projeto tem como objetivo implementar um **Sequence Set**, uma estrutura de dados eficiente para armazenamento e manipulação de registros de acidentes. Cada registro possui:

- Coordenadas geográficas do local do acidente  
- Descrição do acidente  
- Código postal (ZIP)  
- Título (tipo de emergência)  
- Data e hora  
- Cidade  
- Endereço detalhado  
- Indicador  

O Sequence Set organiza os dados em listas ordenadas, otimizando buscas e remoções. A base utilizada já vem previamente ordenada por ID para maximizar o desempenho.

---

## Funcionalidades Implementadas

- **Importação de dados:** leitura dos registros de um arquivo CSV (`base_dados.csv`)
- **Inserção de dados:**  
  - Via entrada padrão  
  - Por meio de um arquivo (`novo.csv`)
- **Remoção de registro:** por ID
- **Busca de registros:** por ID
- **Visualização dos dados ordenados**
- **Encerramento:** finaliza o programa

---

## Estrutura do Projeto

- **Código-Fonte:**  
  - `SequenceSet_final.cpp`
- **Arquivos de Dados:**  
  - `base_dados.csv`: Base de dados principal (ordenada por ID)  
  - `novo.csv`: Arquivo auxiliar para inserção de novos registros
- **Arquivos Binários:**  
  - Fragmentos do Sequence Set, armazenados em arquivos como `1.bin`, `2.bin`, etc.
- **Executável:**  
  - `final.exe` (gerado após compilação)

---

## Requisitos

- **Sistema Operacional:** Linux  
- **Bibliotecas:** Não utiliza bibliotecas externas

---

## Como usar

1. **Clone o repositório:**

    ```bash
    git clone https://github.com/seu-usuario/seu-repositorio.git
    cd seu-repositorio
    ```

2. **Certifique-se de que `base_dados.csv` está na pasta do projeto.**
3. **Compile o código-fonte com o GCC:**

    ```bash
    g++ SequenceSet_final.cpp -o final.exe
    ```

4. **Execute o programa:**

    ```bash
    ./final.exe
    ```

5. O menu principal aparecerá na tela. Siga as instruções para realizar as operações desejadas (importar, inserir, remover, buscar, visualizar).

---

## Exemplo de Entrada

No menu, ao selecionar a opção de busca e inserir o ID:

```
3
120
```

## Exemplo de Saída

```
Bloco do ID buscado: 3
ID: 120
Latitude: 40.0122
Longitude: -75.2995
Descrição: DREYCOTT LN & W LANCASTER AVE; LOWER MERION; Station 313; 2015-12-11 @ 01:29:52;
Código Postal: 19041
Título: EMS: NAUSEA/VOMITING
Data e hora: 2015-12-11 01:29:52
Endereço: LOWER MERION
Indicador: 1
```

---

## Contato

### Autores

- Caio Bueno Finocchio Martins  
- Lana da Silva Miranda  
- Nina Tobias Novikoff da Cunha Ribeiro  

### E-mails

- caio.martins5@estudante.ufla.br
- lanamiranda@estudante.ufla.br
- nina.ribeiro@estudante.ufla.br
