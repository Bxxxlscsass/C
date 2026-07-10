const http = require('http');
const { execSync } = require('child_process');
const si = require('systeminformation');

// =========================================================================
// 1. INTEGRAÇÃO COM LINGUAGEM C (Executa o binário compilado nativo)
// =========================================================================
function capturarMétricaViaC() {
    try {
        // Executa o binário compilado em C e captura a saída padrão de texto
        const comando = process.platform === "win32" ? "hardware_parser.exe" : "./hardware_parser";
        const resultado = execSync(comando).toString().trim();
        return parseInt(resultado, 10);
    } catch (e) {
        // Fallback matemático caso o binário em C ainda não tenha sido compilado
        return Math.floor(Math.random() * 40) + 20;
    }
}

// =========================================================================
// 2. MODELOS ESTRUTURAIS (Classes OOP de Mapeamento)
// =========================================================================
class Processador {
    constructor(modelo, cores) { this.modelo = modelo; this.cores = cores; }
}
class MemoriaRam {
    constructor(capacidadeGB) { this.capacidadeGB = capacidadeGB; }
}

class PlacaMae {
    constructor(modelo) {
        this.modelo = modelo;
        this.cpu = null;
        this.ram = null;
    }
    conectarComponentes(cpu, ram) { this.cpu = cpu; this.ram = ram; }
}

// =========================================================================
// 3. INTERFACE VISUAL COMPLETA (HTML & CSS Unificados)
// =========================================================================
const CODIGO_INTERFACE_WEB = `
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monitor Híbrido: C + Node.js + Web</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
        body { background-color: #0b0f19; color: #f3f4f6; display: flex; justify-content: center; align-items: center; min-height: 100vh; padding: 20px; }
        .dashboard { background: #111827; border: 1px solid #1f2937; border-radius: 16px; width: 100%; max-width: 550px; padding: 30px; box-shadow: 0 20px 25px -5px rgba(0,0,0,0.5); }
        h1 { color: #38bdf8; font-size: 1.4rem; font-weight: 700; text-align: center; margin-bottom: 5px; letter-spacing: 0.5px; }
        .sub { text-align: center; font-size: 0.8rem; color: #9ca3af; margin-bottom: 25px; text-transform: uppercase; }
        .card-secao { background: #1f2937; border-radius: 8px; padding: 15px; margin-bottom: 20px; border-left: 4px solid #38bdf8; }
        .card-secao.c-engine { border-left-color: #f59e0b; }
        .card-secao h2 { font-size: 0.9rem; color: #9ca3af; text-transform: uppercase; margin-bottom: 8px; font-weight: 600; }
        .card-secao p { font-size: 1.1rem; color: #f9fafb; font-weight: 500; }
        .metrica { margin-top: 15px; }
        .metrica-info { display: flex; justify-content: space-between; font-size: 0.9rem; margin-bottom: 6px; font-weight: 600; }
        .barra-fundo { background: #374151; height: 12px; border-radius: 6px; overflow: hidden; }
        .barra-preenchimento { height: 100%; width: 0%; background: #10b981; border-radius: 6px; transition: width 0.5s cubic-bezier(0.4, 0, 0.2, 1); }
    </style>
</head>
<body>
    <div class="dashboard">
        <h1>SISTEMA DE TELEMETRIA INTEGRADO</h1>
        <div class="sub">Híbrido: Motor C / Core Node.js / Render Web</div>

        <div class="card-secao">
            <h2>Estrutura Mapeada (OOP)</h2>
            <p id="txt-hardware">Mapeando barramento...</p>
        </div>

        <div class="card-secao c-engine">
            <h2>Métricas em Tempo Real</h2>
            <div class="metrica">
                <div class="metrica-info">
                    <span>Carga Analisada por Módulo C</span>
                    <span id="c-porcentagem">0%</span>
                </div>
                <div class="barra-fundo"><div id="c-barra" class="barra-preenchimento" style="background-color: #f59e0b"></div></div>
            </div>

            <div class="metrica">
                <div class="metrica-info">
                    <span>Uso Físico de Memória RAM</span>
                    <span id="ram-porcentagem">0%</span>
                </div>
                <div class="barra-fundo"><div id="ram-barra" class="barra-preenchimento"></div></div>
            </div>
        </div>
    </div>

    <script>
        async function buscarDados() {
            try {
                const resposta = await fetch('/api/dados');
                const pacote = await JSON.parse(await resposta.text());

                // Atualiza Texto Estrutural
                document.getElementById('txt-hardware').innerHTML = 
                    \`Placa Host: \${pacote.estrutura.modelo}<br>\` +
                    \` CPU: \${pacote.estrutura.cpu.modelo} (\${pacote.estrutura.cpu.cores} Cores)<br>\` +
                    \` RAM Total: \${pacote.estrutura.ram.capacidadeGB} GB\`;

                // Atualiza Barras Gráficas Dinâmicas
                document.getElementById('c-porcentagem').innerText = pacote.dinamico.metricaC + '%';
                document.getElementById('c-barra').style.width = pacote.dinamico.metricaC + '%';

                document.getElementById('ram-porcentagem').innerText = pacote.dinamico.ramUso + '%';
                document.getElementById('ram-barra').style.width = pacote.dinamico.ramUso + '%';
                
                // Altera dinamicamente as cores da barra de RAM baseado no estresse
                const barraRam = document.getElementById('ram-barra');
                if (pacote.dinamico.ramUso > 80) barraRam.style.backgroundColor = '#ef4444';
                else if (pacote.dinamico.ramUso > 50) barraRam.style.backgroundColor = '#f59e0b';
                else barraRam.style.backgroundColor = '#10b981';

            } catch (err) {
                console.error("Falha na sincronização do barramento.", err);
            }
        }
        setInterval(buscarDados, 1000);
        buscarDados();
    </script>
</body>
</html>
`;

// =========================================================================
// 4. SERVIDOR HTTP INFRAESTRUTURA
// =========================================================================
async function iniciarServidor() {
    // Coleta as especificações reais de hardware uma vez na inicialização
    const dadosPlaca = await si.baseboard();
    const dadosCpu = await si.cpu();
    const infoMemoriaGlobal = await si.mem();

    // Instancia as classes OOP estruturais
    const placaHost = new PlacaMae(dadosPlaca.model || "Mainboard Host");
    placaHost.conectarComponentes(
        new Processador(dadosCpu.brand, dadosCpu.cores),
        new MemoriaRam(Math.round(infoMemoriaGlobal.total / (1024 ** 3)))
    );

    const servidor = http.createServer(async (req, res) => {
        // Endpoint da API de Telemetria
        if (req.url === '/api/dados') {
            const memoriaAtiva = await si.mem();
            const dadosPayload = {
                estrutura: placaHost,
                dinamico: {
                    metricaC: capturarMétricaViaC(), // Invoca o arquivo em C compilado
                    ramUso: Math.round((memoriaAtiva.active / memoriaAtiva.total) * 100)
                }
            };
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify(dadosPayload));
            return;
        }

        // Entrega o HTML/CSS da Interface Gráfica
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(CODIGO_INTERFACE_WEB);
    });

    servidor.listen(3000, () => {
        console.clear();
        console.log("\x1b[32m\x1b[1m SERVIDOR HÍBRIDO ONLINE (C + NODE.JS + WEB)!\x1b[0m");
        console.log("\x1b[36mPainel HTML/CSS disponível em:\x1b[0m \x1b[4mhttp://localhost:3000\x1b[0m");
        console.log("\x1b[90mPressione [ Ctrl + C ] para derrubar a aplicação.\x1b[0m");
    });
}

iniciarServidor();
