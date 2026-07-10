#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUTOS 100
#define MAX_INPUT 20
#define ARQUIVO_DADOS "estoque.txt"

// Estrutura do Produto
typedef struct {
    int id;
    char nome[MAX_INPUT];
    int quantidade;
    float preco;
} Produto;

Produto estoque[MAX_PRODUTOS];
int totalProdutos = 0;

// Enums para sabermos qual caixa de texto está selecionada
typedef enum { INPUT_NOME, INPUT_QTD, INPUT_PRECO, INPUT_NENHUM } CampoAtivo;
CampoAtivo campoSelecionado = INPUT_NENHUM;

// Variáveis de texto para os inputs
char txtNome[MAX_INPUT] = "\0";
char txtQtd[MAX_INPUT] = "\0";
char txtPreco[MAX_INPUT] = "\0";

int lenNome = 0, lenQtd = 0, lenPreco = 0;
char mensagemStatus[60] = "Aguardando acao...";

// Carrega os dados do arquivo .txt
void carregarDados() {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "r");
    if (arquivo == NULL) return; 
    totalProdutos = 0;
    while (fscanf(arquivo, "%d;%[^;];%d;%f\n", 
                  &estoque[totalProdutos].id, estoque[totalProdutos].nome, 
                  &estoque[totalProdutos].quantidade, &estoque[totalProdutos].preco) == 4) {
        totalProdutos++;
        if (totalProdutos >= MAX_PRODUTOS) break;
    }
    fclose(arquivo);
}

// Salva os dados no arquivo .txt
void salvarDados() {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "w");
    if (arquivo == NULL) return;
    for (int i = 0; i < totalProdutos; i++) {
        fprintf(arquivo, "%d;%s;%d;%.2f\n", 
                estoque[i].id, estoque[i].nome, estoque[i].quantidade, estoque[i].preco);
    }
    fclose(arquivo);
}

// Lógica para capturar digitação em um campo específico
void gerenciarInputTexto(char *texto, int *contador, bool apenasNumeros) {
    int chave = GetCharPressed();
    while (chave > 0) {
        // Se for apenas número, bloqueia letras. Permite ponto '.' para preço.
        bool valido = true;
        if (apenasNumeros) {
            if ((chave < '0' || chave > '9') && chave != '.') valido = false;
        }
        
        if (valido && (chave >= 32) && (chave <= 125) && (*contador < MAX_INPUT - 1)) {
            texto[*contador] = (char)chave;
            texto[*contador + 1] = '\0';
            (*contador)++;
        }
        chave = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && *contador > 0) {
        (*contador)--;
        texto[*contador] = '\0';
    }
}

// Executa a validação e o cadastro
void executarCadastro() {
    if (lenNome == 0 || lenQtd == 0 || lenPreco == 0) {
        strcpy(mensagemStatus, "Erro: Preencha todos os campos!");
        return;
    }

    if (totalProdutos >= MAX_PRODUTOS) {
        strcpy(mensagemStatus, "Erro: Estoque cheio!");
        return;
    }

    Produto p;
    p.id = (totalProdutos == 0) ? 1 : estoque[totalProdutos - 1].id + 1;
    strcpy(p.nome, txtNome);
    p.quantidade = atoi(txtQtd);    // Converte texto para int
    p.preco = strtof(txtPreco, NULL); // Converte texto para float

    estoque[totalProdutos] = p;
    totalProdutos++;
    salvarDados();

    sprintf(mensagemStatus, "Sucesso: '%s' adicionado!", txtNome);

    // Reseta todos os inputs
    txtNome[0] = '\0'; txtQtd[0] = '\0'; txtPreco[0] = '\0';
    lenNome = 0; lenQtd = 0; lenPreco = 0;
    campoSelecionado = INPUT_NENHUM;
}

int main(void) {
    InitWindow(800, 500, "Controle de Estoque Completo - Raylib");
    SetTargetFPS(60);
    carregarDados();

    // Definição dos retângulos dos Inputs e Botões
    Rectangle recNome = { 50, 110, 220, 35 };
    Rectangle recQtd = { 50, 180, 220, 35 };
    Rectangle recPreco = { 50, 250, 220, 35 };
    
    Rectangle btnSalvar = { 50, 310, 100, 40 };
    Rectangle btnExcluir = { 170, 310, 100, 40 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        // seleção de campo de input com clique 
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, recNome)) campoSelecionado = INPUT_NOME;
            else if (CheckCollisionPointRec(mousePos, recQtd)) campoSelecionado = INPUT_QTD;
            else if (CheckCollisionPointRec(mousePos, recPreco)) campoSelecionado = INPUT_PRECO;
            else campoSelecionado = INPUT_NENHUM;
        }

        // Altera o cursor se passar em cima de qualquer input
        if (CheckCollisionPointRec(mousePos, recNome) || CheckCollisionPointRec(mousePos, recQtd) || CheckCollisionPointRec(mousePos, recPreco)) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        // captura texto
        if (campoSelecionado == INPUT_NOME) gerenciarInputTexto(txtNome, &lenNome, false);
        else if (campoSelecionado == INPUT_QTD) gerenciarInputTexto(txtQtd, &lenQtd, true);
        else if (campoSelecionado == INPUT_PRECO) gerenciarInputTexto(txtPreco, &lenPreco, true);

        // Pular campos usando a tecla TAB
        if (IsKeyPressed(KEY_TAB)) {
            if (campoSelecionado == INPUT_NOME) campoSelecionado = INPUT_QTD;
            else if (campoSelecionado == INPUT_QTD) campoSelecionado = INPUT_PRECO;
            else if (campoSelecionado == INPUT_PRECO) campoSelecionado = INPUT_NOME;
        }

        // Cliques de botoes
        bool hoverSalvar = CheckCollisionPointRec(mousePos, btnSalvar);
        if ((hoverSalvar && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ENTER)) {
            executarCadastro();
        }

        bool hoverExcluir = CheckCollisionPointRec(mousePos, btnExcluir);
        if (hoverExcluir && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (totalProdutos > 0) {
                totalProdutos--;
                salvarDados();
                strcpy(mensagemStatus, "Sucesso: Ultimo item removido!");
            } else {
                strcpy(mensagemStatus, "Erro: Nao ha itens!");
            }
        }

        // Redenrização grafica
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Top Bar
            DrawRectangle(0, 0, 800, 60, DARKGRAY);
            DrawText("CONTROLE DE ESTOQUE COMPLETO", 20, 15, 24, WHITE);

            // INPUT 1: Nome
            DrawText("Nome do Produto:", 50, 90, 14, GRAY);
            DrawRectangleRec(recNome, LIGHTGRAY);
            DrawRectangleLines((int)recNome.x, (int)recNome.y, (int)recNome.width, (int)recNome.height, campoSelecionado == INPUT_NOME ? RED : DARKGRAY);
            DrawText(txtNome, (int)recNome.x + 10, (int)recNome.y + 8, 18, MAROON);

            // INPUT 2: Quantidade
            DrawText("Quantidade:", 50, 160, 14, GRAY);
            DrawRectangleRec(recQtd, LIGHTGRAY);
            DrawRectangleLines((int)recQtd.x, (int)recQtd.y, (int)recQtd.width, (int)recQtd.height, campoSelecionado == INPUT_QTD ? RED : DARKGRAY);
            DrawText(txtQtd, (int)recQtd.x + 10, (int)recQtd.y + 8, 18, MAROON);

            // INPUT 3: Preço
            DrawText("Preco Unitario (R$):", 50, 230, 14, GRAY);
            DrawRectangleRec(recPreco, LIGHTGRAY);
            DrawRectangleLines((int)recPreco.x, (int)recPreco.y, (int)recPreco.width, (int)recPreco.height, campoSelecionado == INPUT_PRECO ? RED : DARKGRAY);
            DrawText(txtPreco, (int)recPreco.x + 10, (int)recPreco.y + 8, 18, MAROON);

            // Botoes
            DrawRectangleRec(btnSalvar, hoverSalvar ? LIME : GREEN);
            DrawRectangleLines((int)btnSalvar.x, (int)btnSalvar.y, (int)btnSalvar.width, (int)btnSalvar.height, DARKGRAY);
            DrawText("SALVAR", (int)btnSalvar.x + 18, (int)btnSalvar.y + 12, 16, WHITE);

            DrawRectangleRec(btnExcluir, hoverExcluir ? RED : MAROON);
            DrawRectangleLines((int)btnExcluir.x, (int)btnExcluir.y, (int)btnExcluir.width, (int)btnExcluir.height, DARKGRAY);
            DrawText("EXCLUIR", (int)btnExcluir.x + 16, (int)btnExcluir.y + 12, 16, WHITE);

            // Status
            DrawText(mensagemStatus, 50, 370, 14, ORANGE);

            // Tabelas produtos
            DrawRectangle(320, 80, 450, 380, GetColor(0xf5f5f5ff));
            DrawRectangleLines(320, 80, 450, 380, LIGHTGRAY);
            DrawText("ID", 340, 95, 16, DARKGRAY); DrawText("Nome", 400, 95, 16, DARKGRAY);
            DrawText("Qtd", 620, 95, 16, DARKGRAY); DrawText("Preco", 690, 95, 16, DARKGRAY);
            DrawLine(330, 120, 750, 120, LIGHTGRAY);

            int espacamentoY = 130;
            for (int i = 0; i < totalProdutos; i++) {
                char idTxt[10], qtdTxt[10], precoTxt[15];
                sprintf(idTxt, "%d", estoque[i].id);
                sprintf(qtdTxt, "%d", estoque[i].quantidade);
                sprintf(precoTxt, "R$ %.2f", estoque[i].preco);

                DrawText(idTxt, 340, espacamentoY, 16, BLACK);
                DrawText(estoque[i].nome, 400, espacamentoY, 16, BLACK);
                DrawText(qtdTxt, 620, espacamentoY, 16, BLACK);
                DrawText(precoTxt, 690, espacamentoY, 16, BLACK);
                espacamentoY += 25;
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
