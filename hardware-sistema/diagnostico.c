#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Configuração de cores para o terminal via códigos ANSI
#define RESET "\x1b[0m"
#define BRILHANTE "\x1b[1m"
#define CYAN "\x1b[36m"
#define VERDE "\x1b[32m"
#define AMARELO "\x1b[33m"
#define VERMELHO "\x1b[31m"
#define CINZA "\x1b[90m"
#define FUNDO_AZUL "\x1b[44m\x1b[37m"

// Função para desenhar barras de progresso textuais no console
void criarBarra(char *buffer, int porcentagem) {
    int tamanhoMax = 20;
    int preenchido = (int)round((tamanhoMax * porcentagem) / 100.0);
    
    char *corBarra = VERDE;
    if (porcentagem >= 80) corBarra = VERMELHO;
    else if (porcentagem >= 50) corBarra = AMARELO;

    // Monta a barra no buffer string recebido
    int pos = sprintf(buffer, "[%s", corBarra);
    for (int i = 0; i < tamanhoMax; i++) {
        if (i < preenchido) {
            pos += sprintf(buffer + pos, "#");
        } else {
            pos += sprintf(buffer + pos, " ");
        }
    }
    sprintf(buffer + pos, "%s] %3d%%", RESET, porcentagem);
}

// Funções auxiliares para capturar os dados reais do Linux (/proc)
void obterInfoCPU(char *modelo, int *nucleos) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        strcpy(modelo, "Processador Desconhecido");
        *nucleos = 1;
        return;
    }
    char linha[256];
    *nucleos = 0;
    int modeloDefinido = 0;
    while (fgets(linha, sizeof(linha), fp)) {
        if (!modeloDefinido && strncmp(linha, "model name", 10) == 0) {
            char *p = strchr(linha, ':');
            if (p) {
                p++;
                while (*p == ' ') p++;
                strcpy(modelo, p);
                modelo[strcspn(modelo, "\n")] = 0; // Remove quebra de linha
                modeloDefinido = 1;
            }
        }
        if (strncmp(linha, "processor", 9) == 0) {
            (*nucleos)++;
        }
    }
    fclose(fp);
}

long obterMemoriaTotal() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 0;
    char linha[256];
    long memTotal = 0;
    while (fgets(linha, sizeof(linha), fp)) {
        if (strncmp(linha, "MemTotal:", 9) == 0) {
            sscanf(linha, "MemTotal: %ld", &memTotal);
            break;
        }
    }
    fclose(fp);
    return memTotal * 1024; // Converte KB para Bytes
}

void obterMétricasUso(int *cpuUso, int *ramUso, double *ramUsadaGB, double *ramTotalGB) {
    // 1. Cálculo real de uso da CPU (Diferença de tempos do /proc/stat)
    static long long ultimaUser = 0, ultimaNice = 0, ultimaSystem = 0, ultimaIdle = 0;
    long long user, nice, system, idle;
    
    FILE *fpStat = fopen("/proc/stat", "r");
    if (fpStat) {
        char cpu[10];
        if (fscanf(fpStat, "%s %lld %lld %lld %lld", cpu, &user, &nice, &system, &idle) == 5) {
            long long totalAntigo = ultimaUser + ultimaNice + ultimaSystem + ultimaIdle;
            long long totalAtual = user + nice + system + idle;
            long long idleAntigo = ultimaIdle;
            long long idleAtual = idle;

            long long deltaTotal = totalAtual - totalAntigo;
            long long deltaIdle = idleAtual - idleAntigo;

            if (deltaTotal > 0) {
                *cpuUso = (int)(100 * (deltaTotal - deltaIdle) / deltaTotal);
            } else {
                *cpuUso = 0;
            }

            ultimaUser = user; ultimaNice = nice; ultimaSystem = system; ultimaIdle = idle;
        }
        fclose(fpStat);
    }

    // 2. Cálculo real de uso da Memória RAM
    FILE *fpMem = fopen("/proc/meminfo", "r");
    if (fpMem) {
        char linha[256];
        long memTotal = 0, memFree = 0, memAvailable = 0;
        while (fgets(linha, sizeof(linha), fpMem)) {
            if (strncmp(linha, "MemTotal:", 9) == 0) sscanf(linha, "MemTotal: %ld", &memTotal);
            if (strncmp(linha, "MemFree:", 8) == 0) sscanf(linha, "MemFree: %ld", &memFree);
            if (strncmp(linha, "MemAvailable:", 13) == 0) sscanf(linha, "MemAvailable: %ld", &memAvailable);
        }
        fclose(fpMem);

        long ativa = memTotal - memAvailable;
        *ramTotalGB = (double)memTotal / (1024.0 * 1024.0);
        *ramUsadaGB = (double)ativa / (1024.0 * 1024.0);
        *ramUso = (int)((ativa * 100) / memTotal);
    }
}

// Executa o diagnóstico inicial do Hardware (Roda apenas uma vez na inicialização)
void exibirEspecificacoesFixas() {
    char cpuModelo[256];
    int cpuNucleos;
    obterInfoCPU(cpuModelo, &cpuNucleos);
    long memTotalBytes = obterMemoriaTotal();

    printf("\033[H\033[J"); // Limpa a tela de forma limpa (Equivalente ao console.clear())
    printf("%s ================= DIAGNÓSTICO COMPLETO DE HARDWARE ================= %s\n\n", FUNDO_AZUL, RESET);
    
    printf("%s%s[SISTEMA OPERACIONAL]%s\n", CYAN, BRILHANTE, RESET);
    printf("Plataforma: Linux\n");
    printf("Arquitetura: x86_64\n\n");

    printf("%s%s[PROCESSADOR (CPU)]%s\n", CYAN, BRILHANTE, RESET);
    printf("Modelo: %s\n", cpuModelo);
    printf("Núcleos: %d Linhas de Execução Ativas\n\n", cpuNucleos);
    
    printf("%s%s[MEMÓRIA RAM TOTAL]%s\n", CYAN, BRILHANTE, RESET);
    printf("Capacidade Instalada: %.2f GB\n\n", (double)memTotalBytes / (1024.0 * 1024.0 * 1024.0));

    printf("%sIniciando monitoramento dinâmico... Aguarde.%s\n", CINZA, RESET);
    fflush(stdout);
    sleep(1); // Aguarda 1 segundo antes do loop inicializar
}

// Loop principal de atualização em tempo real
void loopMonitoramento() {
    char barraCpu[256];
    char barraRam[256];

    while (1) {
        int cpuUso = 0, ramUso = 0;
        double ramUsadaGB = 0.0, ramTotalGB = 0.0;

        obterMétricasUso(&cpuUso, &ramUso, &ramUsadaGB, &ramTotalGB);
        criarBarra(barraCpu, cpuUso);
        criarBarra(barraRam, ramUso);

        // Move o cursor do terminal de volta para a linha 13 para atualizar sem piscar a tela
        // (Ajustado de 16 para 13 em C devido ao tamanho do cabeçalho estático)
        printf("\x1b[13;1H");

        printf("%s%s============= TELEMETRIA EM TEMPO REAL =============%s\n\n", CYAN, BRILHANTE, RESET);

        // 1. Monitoramento de CPU
        printf("Carga Global da CPU : %s (Temp: N/A na CLI padrão)     \n\n", barraCpu);

        // 2. Monitoramento de Memória RAM
        printf("Uso de Memória RAM   : %s (%.2f GB / %.2f GB usados)     \n\n", barraRam, ramUsadaGB, ramTotalGB);

        // 3. Status de Alerta de Sobrecarga (Diagnóstico Dinâmico)
        printf("%s%s[DIAGNÓSTICO E SAÚDE]%s\n", CYAN, BRILHANTE, RESET);
        if (cpuUso > 85 || ramUso > 85) {
            printf("Status: %s%sALERTA CRÍTICO - Sobrecarga de hardware detectada!%s   \n", VERMELHO, BRILHANTE, RESET);
        } else if (cpuUso > 50 || ramUso > 50) {
            printf("Status: %sAtenção - Uso moderado/alto de recursos.%s            \n", AMARELO, RESET);
        } else {
            printf("Status: %sSistema operando de forma estável.%s                        \n", VERDE, RESET);
        }

        printf("\n%sPressione [ Ctrl + C ] para fechar o diagnóstico.%s\n", CINZA, RESET);
        
        fflush(stdout);
        sleep(1); // Delay idêntico de 1000ms (1 segundo)
    }
}

int main() {
    exibirEspecificacoesFixas();
    loopMonitoramento();
    return 0;
}
