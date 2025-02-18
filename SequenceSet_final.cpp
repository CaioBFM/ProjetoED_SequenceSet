#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

const int tamanho = 50; // Tamanho fixo de cada pedaço do Sequence Set

// Struct que armazena as informações de cada registro 
struct dado {				//Registro de acidentes

	int id;					//Identificador
	double lat;				//Latitude da localização
	double lng;				//Longitude da localização
	char desc[100];			//Descrição do acidente
	double zip;				//Código postal da localização
	char title[100];		//Título (tipo de emergência)
	char timeStamp[20];		//Data e hora do acidente
	char twp[20];			//Cidade
	char addr[100];			//Endereço detalhado
	int e;					//Flag /indicador
};

// Struct que armazena informações do Sequence Set
struct sequenceSet {
	int numeroDePedacos = 6;   // Inicialmente seis porque a base de dados contém 300 dados e cada pedaço tem 50 registros
	int primeiroPedaco = 1;
	int ultimoPedaco = 6;
};

// Struct que armazena o cabeçalho dos pedaços
struct pedaco {
    int numeroDeRegistros = 0;
    int pedacoAnterior = -1; 
    int proximoPedaco = -1;
};

// Função pra criar e inicializar um novo pedaço
void criarPedaco(int &contador) {
    contador++;
    pedaco pedaco;
    pedaco.numeroDeRegistros = 0;
    pedaco.pedacoAnterior = contador - 1;
    pedaco.proximoPedaco = contador + 1;

    string numArquivo = to_string(contador);
    ofstream arquivo(numArquivo + ".bin", ios::binary); // Cria o novo arquivo binário que tem como nome um número

	if (arquivo.is_open()) {
		arquivo.write(reinterpret_cast<const char*>(&pedaco.numeroDeRegistros), sizeof(int));
		arquivo.write(reinterpret_cast<const char*>(&pedaco.pedacoAnterior), sizeof(int));
		arquivo.write(reinterpret_cast<const char*>(&pedaco.proximoPedaco), sizeof(int));

	} else {
		cout << "Erro ao abrir arquivo" << endl;
	}
	arquivo.close();
}

// Função para encontrar o pedaço adequado para um determinado id
int encontrarPedaco(sequenceSet &sequenceSet, int id, pedaco pedacoAtual, bool teste) {
    int i = 1;
    int cont = 0;
    dado primeiro; // Primeiro registro do pedaco
    while (cont < sequenceSet.numeroDePedacos && teste == false) { // Compara o id com o id do primeiro dado de cada pedaco
        ifstream arq(to_string (i) + ".bin", ios::binary);

        arq.seekg(0);
        arq.read(reinterpret_cast<char*>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros));
        arq.read(reinterpret_cast<char*>(&pedacoAtual.pedacoAnterior ), sizeof(pedacoAtual.pedacoAnterior));
        arq.read(reinterpret_cast<char*>(&pedacoAtual.proximoPedaco), sizeof(pedacoAtual.proximoPedaco));
        arq.read(reinterpret_cast<char*>(&primeiro), sizeof(dado));

        if (id < primeiro.id) { // Se for menor significa que o pedaco adequado deve ser o anterior ao da comparação
            teste = true;
        }

        arq.close();
        i = pedacoAtual.proximoPedaco;
        cont++;
    }
    if (teste == false) { // Se for menor que o primeiro do ultimo pedaco da sequencia: teste = false
        return sequenceSet.ultimoPedaco; // Então o pedaco adequado é o último
    } else {
        return pedacoAtual.pedacoAnterior; 
    }
}

// Função para inserir um novo dado em um determinado pedaço
void insereNoPedaco(dado novoDado, sequenceSet &sequenceSet) {
    pedaco pedacoAtual;
    bool teste = false;

    int pedacoEncontrado = encontrarPedaco(sequenceSet, novoDado.id, pedacoAtual, teste); // Chama a funcao para encontrar o pedaco em que será inserido

    fstream arqPedacoAtual(to_string (pedacoEncontrado) + ".bin", ios::binary | ios::in | ios::out);

    arqPedacoAtual.read(reinterpret_cast<char*>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros));
    arqPedacoAtual.read(reinterpret_cast<char*>(&pedacoAtual.pedacoAnterior), sizeof(pedacoAtual.pedacoAnterior));
    arqPedacoAtual.read(reinterpret_cast<char*>(&pedacoAtual.proximoPedaco), sizeof(pedacoAtual.proximoPedaco));

    dado vetorTemp[pedacoAtual.numeroDeRegistros + 2]; 
    for (int i = 0; i < pedacoAtual.numeroDeRegistros; i++) { 							//Armazena registros do pedaço no vetorTemp
        arqPedacoAtual.read(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado)); 
    }

    ofstream arqLimparAtual(to_string (pedacoEncontrado) + ".bin", ios::binary | ios::out |ios::trunc);  // Limpa o pedaço

    int c = pedacoAtual.numeroDeRegistros - 1;
    while (c >= 0 && vetorTemp[c].id > novoDado.id) {
        vetorTemp[c + 1] = vetorTemp[c];
        c--;
    }
    vetorTemp[c + 1] = novoDado; // Coloca o novo dado na posicao correta no vetor 

    if (pedacoAtual.numeroDeRegistros == tamanho) { 				// Se o pedaço estiver cheio
        int metade = (pedacoAtual.numeroDeRegistros + 1) / 2;
        pedaco novoPedaco;											// Cria um novo pedaço para inserir e dividir o pedaço atual
        
        novoPedaco.numeroDeRegistros = metade;
        novoPedaco.pedacoAnterior = pedacoEncontrado;
        novoPedaco.proximoPedaco = pedacoAtual.proximoPedaco;

        int k = sequenceSet.numeroDePedacos + 1;					// Cria um novo arquivo binário para o novo pedaço
        ofstream arqNovoPedaco (to_string(k) + ".bin", ios::binary | ios::out);

        if (arqNovoPedaco.is_open()) {
            arqNovoPedaco.seekp(0);
            arqNovoPedaco.write(reinterpret_cast<const char*>(&novoPedaco.numeroDeRegistros), sizeof(novoPedaco.numeroDeRegistros));
            arqNovoPedaco.write(reinterpret_cast<const char*>(&novoPedaco.pedacoAnterior), sizeof(novoPedaco.pedacoAnterior));
            arqNovoPedaco.write(reinterpret_cast<const char*>(&novoPedaco.proximoPedaco), sizeof(novoPedaco.proximoPedaco));

            for (int i = metade + 1; i <= tamanho; i++) {
                arqNovoPedaco.write(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado)); 		//Escreve a segunda metade do vetor no novo arquivo
            }
        }

        int atualProxIndice = pedacoAtual.proximoPedaco; // Variável para salvar o índice do proximo pedaço antes de atualizar
      
        pedacoAtual.numeroDeRegistros = metade + 1;
        pedacoAtual.proximoPedaco = sequenceSet.numeroDePedacos + 1;

        if (arqLimparAtual.is_open()) { 
            arqLimparAtual.seekp(0);
            arqLimparAtual.write(reinterpret_cast<char*>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros));
            arqLimparAtual.write(reinterpret_cast<char*>(&pedacoAtual.pedacoAnterior), sizeof(pedacoAtual.pedacoAnterior));
            arqLimparAtual.write(reinterpret_cast<char*>(&pedacoAtual.proximoPedaco), sizeof(pedacoAtual.proximoPedaco));
            
            for (int i = 0; i <= metade; i++) {
                arqLimparAtual.write(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado)); // Escreve a primeira metade do vetor no arquivo do pedaco atual
            }
        }

        pedaco atualProx; // Para atualizar o cabeçalho do proximo pedaco do atual
        fstream arqAtualProx(to_string (atualProxIndice) + ".bin", ios:: binary| ios:: in |ios :: out);
        
        arqAtualProx.read(reinterpret_cast<char*>(&atualProx.numeroDeRegistros), sizeof(atualProx.numeroDeRegistros)); 
        arqAtualProx.read(reinterpret_cast<char*>(&atualProx.pedacoAnterior), sizeof(atualProx.pedacoAnterior)); // Proximo não se altera
        
        atualProx.pedacoAnterior = sequenceSet.ultimoPedaco + 1;

        if (arqAtualProx.is_open()) {
            arqAtualProx.seekp(0);
            arqAtualProx.write(reinterpret_cast<char*>(&atualProx.numeroDeRegistros), sizeof(atualProx.numeroDeRegistros)); 
            arqAtualProx.write(reinterpret_cast<char*>(&atualProx.pedacoAnterior), sizeof(atualProxIndice));
        }

        sequenceSet.numeroDePedacos++;
        if (teste == false){ 			
            sequenceSet.ultimoPedaco++;
        }
    } else { // pedacoAtual.numeroDeRegistros != tamanho: só insere, sem dividir
        pedacoAtual.numeroDeRegistros++;

        if (arqLimparAtual.is_open()) {
            arqLimparAtual.seekp(0);
            arqLimparAtual.write(reinterpret_cast<const char *>(&pedacoAtual.numeroDeRegistros), sizeof(int));
            arqLimparAtual.write(reinterpret_cast<const char *>(&pedacoAtual.pedacoAnterior), sizeof(int));
            arqLimparAtual.write(reinterpret_cast<const char *>(&pedacoAtual.proximoPedaco), sizeof(int));

            for (int i = 0; i < pedacoAtual.numeroDeRegistros; i++) {
                arqLimparAtual.write(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado)); // Só escreve o vetor todo no arquivo, sem divisão
            }
        }
    }
}

// Função que passa a base de dados para os arquivos binários previamente criados (começo do sequenceSet)
void transformaBinario(dado umDado, int &cont) {
    int arqIndice;
    if (cont == 0) { // Cálculo de qual o arquivo adequado para cada id
        arqIndice = 1;
    } else {
        arqIndice = cont / tamanho + 1;
    }

    ofstream arquivo(to_string(arqIndice) + ".bin", ios::binary | ios::app); //Acessa o arquivo correspondente, já criado
    if (arquivo.is_open()) {
        arquivo.write(reinterpret_cast<const char *>(&umDado), sizeof(umDado));
        cont++;

        // Para atualizar o número de registros no pedaço
        pedaco pedacoAtual;
        fstream arq(to_string(arqIndice) + ".bin", ios::binary | ios::in | ios::out);
		arq.read(reinterpret_cast<char*>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros));

        pedacoAtual.numeroDeRegistros++;
        arq.seekp(0);
        arq.write(reinterpret_cast<const char *>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros));
        arq.close();

    } else {
        cout << "Erro ao abrir arquivo para escrita" << endl;
    }
}

// Função para buscar por id
int buscaId (int idBuscado, sequenceSet &sequenceSet, bool imprimir) {
    bool achouPedaco = false;
    pedaco pedacoDoId;
    int pedacoEncontrado = encontrarPedaco(sequenceSet, idBuscado, pedacoDoId, achouPedaco); // Para saber em qual pedaço procurar o id

    fstream arquivo(to_string (pedacoEncontrado) + ".bin", ios::binary | ios::in | ios::out);

    arquivo.read(reinterpret_cast<char*>(&pedacoDoId.numeroDeRegistros), sizeof(pedacoDoId.numeroDeRegistros));
    arquivo.read(reinterpret_cast<char*>(&pedacoDoId.pedacoAnterior), sizeof(pedacoDoId.pedacoAnterior));
    arquivo.read(reinterpret_cast<char*>(&pedacoDoId.proximoPedaco), sizeof(pedacoDoId.proximoPedaco));

    dado vetorDados[pedacoDoId.numeroDeRegistros]; // Armazena registros do pedaço no vetor
    for (int i = 0; i < pedacoDoId.numeroDeRegistros; i++) {
        arquivo.read(reinterpret_cast<char*>(&vetorDados[i]), sizeof(dado)); 
    }

	// Busca binária dentro do vetor 
    int pos_inicial = 0;
    int pos_final = pedacoDoId.numeroDeRegistros;
    int posicao = -1; // Posição inicialmente inválida
    int meio;

    while (pos_inicial <= pos_final) {
        meio = (pos_inicial + pos_final) / 2;
        if (idBuscado == vetorDados[meio].id) {
            posicao = meio;
            pos_inicial = pos_final + 1; // Força a saída do loop
        } else if (idBuscado > vetorDados[meio].id) {
            pos_inicial = meio + 1;
        } else {
            pos_final = meio - 1;
        }
    }

    if (imprimir) { // Só imprime o registro do id se o bool imprimir = true (Para chamar o buscaId em outras funcões que não precisam do imprimir)
        if (posicao != -1) {
           
            cout << "Bloco do ID buscado: " << pedacoEncontrado << endl;

            cout << "\nID: " << vetorDados[posicao].id << endl;
            cout << "Latitude: " << vetorDados[posicao].lat << endl;
            cout << "Longitude: " << vetorDados[posicao].lng << endl;
            cout << "Descricao: " << vetorDados[posicao].desc << endl;
            cout << "Código Postal: " << vetorDados[posicao].zip << endl;
            cout << "Título: " << vetorDados[posicao].title << endl;
            cout << "Data e hora: " << vetorDados[posicao].timeStamp << endl;
            cout << "Endereço: " <<vetorDados[posicao].twp << endl;
            cout << "Indicador: " << vetorDados[posicao].e << endl;
        } else {
            cout << "Elemento não encontrado." << endl; 		// Posição continuou inválida, logo id não foi encontrado
        }
    }
    return posicao; 
}

// Função para remover um dado por id
void removerDado(sequenceSet &sequenceSet,dado &umDado) {
    pedaco pedacoAtual;
    bool teste = false;

    int pedacoEncontrado = encontrarPedaco(sequenceSet, umDado.id, pedacoAtual, teste); // Encontra o pedaço em que o id a ser removido está 

    fstream arqPedacoAtual(to_string(pedacoEncontrado) + ".bin", ios::binary | ios::in | ios::out);
    arqPedacoAtual.seekg(0);
    arqPedacoAtual.read(reinterpret_cast<char*>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros));
    arqPedacoAtual.read(reinterpret_cast<char*>(&pedacoAtual.pedacoAnterior), sizeof(pedacoAtual.pedacoAnterior));
    arqPedacoAtual.read(reinterpret_cast<char*>(&pedacoAtual.proximoPedaco), sizeof(pedacoAtual.proximoPedaco));

    dado vetorTemp[pedacoAtual.numeroDeRegistros];
    for (int i = 0; i < pedacoAtual.numeroDeRegistros; i++) { // Armazena registros do pedaço no vetor
        arqPedacoAtual.read(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado)); 
    }

    bool imprimir = false; // Para chamar o buscaId sem imprimir o registro
    int posicao = buscaId (umDado.id, sequenceSet, imprimir);

    if (posicao == -1) {
        cout << "Nao encontrado" << endl; // Se a posição for inválida o id nem existe, então não é possível remover
    } else {
        if (pedacoAtual.numeroDeRegistros > 25) { //  Caso de remoção simples pois o pedaço continuará com mais elementos que a tamanho/2
            pedacoAtual.numeroDeRegistros--;

            ofstream arqLimparAtual(to_string (pedacoEncontrado) + ".bin", ios::binary | ios::out |ios::trunc); // Limpa o arquivo
            if (arqLimparAtual.is_open()) {
                arqLimparAtual.seekp(0);
                arqLimparAtual.write(reinterpret_cast<const char*>(&pedacoAtual.numeroDeRegistros), sizeof(int)); // Reescreve já atualizado
                arqLimparAtual.write(reinterpret_cast<const char*>(&pedacoAtual.pedacoAnterior), sizeof(int));
                arqLimparAtual.write(reinterpret_cast<const char*>(&pedacoAtual.proximoPedaco), sizeof(int));

				// Escreve no arquivo os registros pulando o id a ser removido
                for(int i = 0; i < posicao; i++) { 	
                    arqLimparAtual.write(reinterpret_cast<const char*>(&vetorTemp[i]), sizeof(dado));
                }
                for(int i = posicao+1; i< pedacoAtual.numeroDeRegistros+1; i++){ // Adicona 1 no numero de registros porque já tinha decrementado antes
                    arqLimparAtual.write(reinterpret_cast<const char*>(&vetorTemp[i]), sizeof(dado));
                }
            }
            arqLimparAtual.close();

        } else { // pedacoAtual.numeroDeRegistros <= 25: remoção faria com que pedaço atual.numeroDeRegistros < tamanho/2, o que não pode acontecer

            pedaco pedacoAnt;								// Para armazenar as informações do pedaço anterior
            fstream arqPedacoAnt(to_string (pedacoAtual.pedacoAnterior) + ".bin", ios::binary | ios::in | ios::out);

            arqPedacoAnt.seekg(0);
            arqPedacoAnt.read(reinterpret_cast<char*>(&pedacoAnt.numeroDeRegistros), sizeof(pedacoAnt.numeroDeRegistros));
            arqPedacoAnt.read(reinterpret_cast<char*>(&pedacoAnt.pedacoAnterior ), sizeof(pedacoAnt.pedacoAnterior));
            arqPedacoAnt.read(reinterpret_cast<char*>(&pedacoAnt.proximoPedaco), sizeof(pedacoAnt.proximoPedaco));

            dado vetorPrincipal[pedacoAnt.numeroDeRegistros]; 
            for (int i = 0; i < pedacoAnt.numeroDeRegistros; i++) { 	// Armazena registros do pedaço anterior
                arqPedacoAnt.read(reinterpret_cast<char*>(&vetorPrincipal[i]), sizeof(dado));
            }

            if (pedacoAnt.numeroDeRegistros == tamanho / 2) { 	// Caso de fusão do pedaço anterior com o pedaço atual, para garantir que nenhum pedaço fique com menos que a metade
                pedacoAnt.proximoPedaco = pedacoAtual.proximoPedaco;

                int tamanhoAnt = pedacoAnt.numeroDeRegistros;
                pedacoAnt.numeroDeRegistros = pedacoAtual.numeroDeRegistros+pedacoAnt.numeroDeRegistros - 1; 

                sequenceSet.numeroDePedacos--;

                if(teste == false){
                    sequenceSet.ultimoPedaco = pedacoAtual.pedacoAnterior;
                }

                ofstream arqLimparAtual(to_string (pedacoAtual.pedacoAnterior) + ".bin", ios::binary | ios::out |ios::trunc); // Limpa o pedaço anterior
                if (arqLimparAtual.is_open()) {
                    arqLimparAtual.seekp(0);
                    arqLimparAtual.write(reinterpret_cast<const char *>(&pedacoAnt.numeroDeRegistros), sizeof(int)); // Atualiza as informações do pedaço anterior
                    arqLimparAtual.write(reinterpret_cast<const char *>(&pedacoAnt.pedacoAnterior), sizeof(int));
                    arqLimparAtual.write(reinterpret_cast<const char *>(&pedacoAnt.proximoPedaco), sizeof(int));

					// Escreve no pedaço anterior os dois vetores, fundindo o pedaço anterior com o atual 
                    for(int i = 0; i < tamanhoAnt; i++) {
                        arqLimparAtual.write(reinterpret_cast<char*>(&vetorPrincipal[i]), sizeof(dado));
                    }
                    for(int i = 0; i < posicao; i++) {
                        arqLimparAtual.write(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado));
                    }
                    for(int i = posicao + 1; i < pedacoAtual.numeroDeRegistros; i++) { //Id não é incluído, logo é removido
                        arqLimparAtual.write(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado));
                    }
                }
                pedacoAtual.numeroDeRegistros = 0; // Pedaço atual é limpado
                ofstream arqVazio(to_string (pedacoEncontrado) + ".bin", ios::binary | ios::out |ios::trunc);
                arqVazio.close();
                arqLimparAtual.close();

            } else { //pedacoAnt.numeroDeRegistros > tamanho/2: Caso em que o pedaço atual pega um elemento do pedaço anterior para garantir que nenhum pedaço fique com menos que a metade
                vetorTemp[posicao] = vetorPrincipal[pedacoAnt.numeroDeRegistros - 1]; // Pega emprestado
                pedacoAnt.numeroDeRegistros--;
                
                for (int i = 1; i < pedacoAtual.numeroDeRegistros; i++) {
                    int chave = vetorTemp[i].id;
                    int j = i - 1;
                    while (j >= 0 && vetorTemp[j].id > chave) {
                        vetorTemp[j + 1] = vetorTemp[j];
                        j = j - 1;
                    }
                    vetorTemp[j + 1].id = chave;
                }
            
                ofstream arqLimparAtual(to_string (pedacoEncontrado) + ".bin", ios::binary | ios::out |ios::trunc); // Limpa o pedaço atual
                if (arqLimparAtual.is_open()) {
                    arqLimparAtual.seekp(0);
                    arqLimparAtual.write(reinterpret_cast<const char*>(&pedacoAtual.numeroDeRegistros), sizeof(int)); // Numero de registros do atual não altera porque removeu um e pegou um emprestado
                    arqLimparAtual.write(reinterpret_cast<const char*>(&pedacoAtual.pedacoAnterior), sizeof(int));
                    arqLimparAtual.write(reinterpret_cast<const char*>(&pedacoAtual.proximoPedaco), sizeof(int));

                    for(int i = 0; i < pedacoAtual.numeroDeRegistros; i++) {
                        arqLimparAtual.write(reinterpret_cast<char*>(&vetorTemp[i]), sizeof(dado)); // Escreve no arquivo os registros do pedaço atual
                    }
                }
                if (arqPedacoAnt.is_open()) {
                    arqPedacoAnt.seekp(0);
                    arqPedacoAnt.write(reinterpret_cast<const char*>(&pedacoAnt.numeroDeRegistros), sizeof(int)); // Atualiza as informações do anterior
                    arqPedacoAnt.write(reinterpret_cast<const char*>(&pedacoAnt.pedacoAnterior), sizeof(int));
                    arqPedacoAnt.write(reinterpret_cast<const char*>(&pedacoAnt.proximoPedaco), sizeof(int));

                    for(int i = 0; i < pedacoAnt.numeroDeRegistros; i++) {
                        arqPedacoAnt.write(reinterpret_cast<char*>(&vetorPrincipal[i]), sizeof(dado)); // Escreve no arquivo os registros do pedaço anterior, com um id a menos (emprestado para o atual)
                    }
                }
                arqPedacoAnt.close();
                arqLimparAtual.close();
            }
        }
    }
}

// Função para importar base de dados (tipo csv)
void importarCSV() {
	string linha, registroString;
	dado umDado;
	char lixo; // Para ignorar as virgulas
	int numRegistros = 0;

	ifstream arquivo("base_dados.csv");

	getline(arquivo, linha,';'); // Ignora cabeçalho

  	while (getline(arquivo, linha) && numRegistros <tamanho*6) {
		arquivo >> umDado.id;
		arquivo >> lixo;
		arquivo >> umDado.lat;
		arquivo >> lixo;
		arquivo >> umDado.lng;
		arquivo >> lixo;

		getline(arquivo, registroString,',');
		strcpy(umDado.desc, registroString.c_str()); // Transforma string em vetor de caracter (necessário para trabalhar com arquivos binários)

		arquivo >> umDado.zip;
		arquivo >> lixo;

		getline(arquivo, registroString, ',');
		strcpy(umDado.title, registroString.c_str());

		getline(arquivo, registroString, ',');
		strcpy(umDado.timeStamp, registroString.c_str());

		getline(arquivo, registroString, ',');
		strcpy(umDado.twp, registroString.c_str());

		getline(arquivo, registroString, ',');
		strcpy(umDado.addr, registroString.c_str());

		arquivo >> umDado.e;

		transformaBinario(umDado, numRegistros); // Chama a funcao que insere cada registro em seu devido pedaço
	}
}

// Função para inserção via texto (arquivo.csv)
void insereViaTexto(string nomeArquivo, sequenceSet &sequenceSet) {
    string linha, registroString;
    dado umDado;
    char lixo; // Retirar vírgulas

    ifstream arquivo(nomeArquivo);

    while (getline(arquivo, linha)) {
        arquivo >> umDado.id;
        arquivo >> lixo;
        arquivo >> umDado.lat;
        arquivo >> lixo;
        arquivo >> umDado.lng;
        arquivo >> lixo;

        getline(arquivo, registroString,','); 
        strcpy(umDado.desc, registroString.c_str()); // Transforma string em vetor de caracter

        arquivo >> umDado.zip;
        arquivo >> lixo;

        getline(arquivo, registroString, ',');
        strcpy(umDado.title, registroString.c_str());

        getline(arquivo, registroString, ',');
        strcpy(umDado.timeStamp, registroString.c_str());

        getline(arquivo, registroString, ',');
        strcpy(umDado.twp, registroString.c_str());

        getline(arquivo, registroString, ',');
        strcpy(umDado.addr, registroString.c_str());

        arquivo >> umDado.e;

        insereNoPedaco(umDado, sequenceSet); // Chama funcao que irá inserir no pedaço correspondente
    }
}

// Função para vizualizar os dados do Sequence Set
void visualizar_dados(sequenceSet &sequenceSet) {
int cont = sequenceSet.primeiroPedaco;

	for(int i = 0; i < sequenceSet.numeroDePedacos; i++) {
		cout << "---------------Pedaco: " << cont << "---------------" << endl;
		ifstream arq(to_string (cont) + ".bin", ios::binary);
		pedaco pedacoAtual;
		arq.seekg(0);
		arq.read(reinterpret_cast<char*>(&pedacoAtual.numeroDeRegistros), sizeof(pedacoAtual.numeroDeRegistros)); // Leitura do cabeçalho
		arq.read(reinterpret_cast<char*>(&pedacoAtual.pedacoAnterior ), sizeof(pedacoAtual.pedacoAnterior));
		arq.read(reinterpret_cast<char*>(&pedacoAtual.proximoPedaco), sizeof(pedacoAtual.proximoPedaco));

		cout << "Numero de registros: " << pedacoAtual.numeroDeRegistros << endl; 
		cout << "Pedaço anterior: " << pedacoAtual.pedacoAnterior << endl;
		cout << "Proximo pedaco: " << pedacoAtual.proximoPedaco << endl;

		dado umDado;
		for(int i = 0; i < pedacoAtual.numeroDeRegistros; i++) { // Lê e imprime todos os registros de cada pedaço

			arq.read(reinterpret_cast<char*>(&umDado), sizeof(dado));
			cout << "\nID: " << umDado.id << endl;
			cout << "Latitude: " << umDado.lat << endl;
			cout << "Longitude: " << umDado.lng << endl;
			cout << "Descricao: " << umDado.desc << endl;
			cout << "Código Postal: " << umDado.zip << endl;
			cout << "Título: " << umDado.title << endl;
			cout << "Data e hora: " << umDado.timeStamp << endl;
			cout << "Cidade: " << umDado.twp << endl;
			cout << "Endereço: " <<umDado.addr << endl;
			cout << "Tipo: " << umDado.e << endl;
		}
		arq.close();
		cont = pedacoAtual.proximoPedaco;
	}
}

// Função para exibir o menu de opções
void menu() {
    cout << "----------------------------" << endl;
    cout << "            Menu:           " << endl;
    cout << " 1. Inserir                 " << endl;
    cout << " 2. Remover                 " << endl;
    cout << " 3. Buscar                  " << endl;
    cout << " 4. Imprimir                " << endl;
    cout << " 5. Encerrar                " << endl;
    cout << "----------------------------" << endl;

}

// Função principal
int main() {
    int opcao, opcao2;
    dado umDado;
    sequenceSet sequenceSet;
    bool imprimir = true;

    int cont = 0;
    for (int i = 0; i < 6; i++) { // Criando os 6 arquivos.bin para a base de dados
        criarPedaco(cont);
    }

    importarCSV(); // Importa a base de dados automaticamente
    menu();
    cin >> opcao;
    while (opcao != 5) { // Exibe o menu enquanto o usuário não escolher encerrar o programa
        switch (opcao) {
            case 1:
                cout << "Escolha uma opcao:\n1. Inserir dados via arquivo de texto\n2. Inserir dados via  entrada padrao" << endl;
                cin >> opcao2;
                cin.ignore(); // Limpa o buffer

                if (opcao2 == 2) {
                    cin >> umDado.id;
                    cin >> umDado.lat;
                    cin >> umDado.lng;
					cin.ignore();
                    cin.getline(umDado.desc, sizeof(umDado.desc));
                    cin >> umDado.zip;
					cin.ignore();
                    cin.getline(umDado.title, sizeof(umDado.title));
                    cin.getline(umDado.timeStamp, sizeof(umDado.timeStamp));
                    cin.getline(umDado.twp, sizeof(umDado.twp));
                    cin.getline(umDado.addr, sizeof(umDado.addr));
                    cin >> umDado.e;

                    insereNoPedaco(umDado, sequenceSet);

                } else {
                    string nomeArquivo;
                    cin >> nomeArquivo;
                    insereViaTexto(nomeArquivo,sequenceSet);
                }
                break;
            case 2:
                cout <<"Digite o ID a ser removido: ";
                cin >> umDado.id;
                removerDado(sequenceSet,umDado);

                break;
            case 3:
                cout << "Digite o ID a ser buscado: " << endl;
                cin >> umDado.id;
                buscaId(umDado.id, sequenceSet, imprimir);
                break;
            case 4:
                visualizar_dados(sequenceSet);
                break;
            case 5: // Finalizar o programa
                return 0;
                break;
            default:
                cout << "Opção inválida!" << endl;
        }
        menu();
        cin >> opcao;
    }
    return 0;
}