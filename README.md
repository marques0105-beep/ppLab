Park Out
--------------------------------------------------------------------------
Park Out é um jogo de puzzle/logística desenvolvido em C++ com o Qt 6, com suporte para Desktop e Mobile.

O objetivo é gerir autocarros de diferentes cores e capacidades (4, 6, 8 e 12 passageiros) estacionados num tabuleiro em grelha. Cada autocarro tem uma direção fixa e só pode avançar se o caminho estiver livre. O jogador deve levar os autocarros aos passageiros da mesma cor, respeitando a capacidade e as plataformas de embarque. O jogo termina quando todos os passageiros são transportados e todos os autocarros saem do tabuleiro.

--------------------------------------------------------------------------
Funcionalidades principais

- 3 níveis de dificuldade crescente definidos em JSON

- Ecrã de seleção de níveis com recordes e estado de conclusão

- Persistência local de progresso (melhor pontuação e melhor tempo por nível)

- Animações fluidas de movimento e embarque

- Notificações de estado em jogo

- Overlay de vitória e derrota

--------------------------------------------------------------------------
Tecnologias e Paradigmas

- Linguagem: C++17

- Framework: Qt 6 (Core, Quick, Concurrent)

- Build system: CMake 3.16+

- Persistência: JSON (savegame.json)

- UI: QML com animações e responsividade

--------------------------------------------------------------------------
- Estrutura do Projeto

main.cpp – ponto de entrada, expõe o GameController ao QML e executa testes

gamecontroller.h/cpp – lógica principal, timers, movimentos, embarque, estados

board.h/cpp – grelha, ocupação, gestão de slots

bus.h/cpp – propriedades do autocarro (cor, capacidade, direção, posição, lotação)

passenger.h – modelo simples de passageiro

gameanalytics.h – funções puras para pontuação e heurística

persistence.h/cpp – leitura/escrita do ficheiro savegame.json

gametests.h – testes unitários do motor

Main.qml – interface completa (menu, tabuleiro, fila, plataformas, overlays)

levels.json – definição dos 3 níveis

--------------------------------------------------------------------------
- Como Compilar

git clone https://github.com/<utilizador>/ppLab.git
cd ppLab
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/appParkOut

--------------------------------------------------------------------------
- Autores

- Gonçalo Marques — 30012478
- Gonçalo Pardelhas — 30014665

Unidade Curricular: Programação por Paradigmas 

Docente: Adrian Dediu
