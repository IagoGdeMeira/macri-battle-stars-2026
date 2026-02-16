# 🇧🇷 Macri Battle Stars - Estratégia de Testes

A qualidade arquitetural do projeto depende diretamente da confiabilidade do núcleo do sistema. Por esse motivo, adotou-se uma estratégia de testes focada principalmente na validação da lógica interna do *ECS*, da orquestração do motor e das regras de jogo.

O objetivo não é testar renderização ou integração gráfica, mas sim garantir que o comportamento do sistema seja previsível, determinístico e seguro contra regressões.

---
---

## 🧪 Framework Utilizado

Para a escrita dos testes automatizados, foi escolhido o **Catch2 (versão 3.12.0)**. Este *framework* permite a criação de testes unitários de forma simples e expressiva, sem necessidade de configuração complexa. Ele é utilizado exclusivamente no subdiretório `tests`, não fazendo parte da *build* final do jogo.

---
---

## 🧱 Testes da Camada `domain`

A camada de domínio é a prioridade máxima da estratégia de testes.

Serão testados:

> - Criação e destruição de entidades
> - Adição e remoção de componentes
> - Funcionamento de queries
> - Integridade do armazenamento de componentes
> - Execução isolada de sistemas base

Por não depender de *SDL*, esta camada pode ser testada de forma totalmente isolada.

---
---

## ⚙️ Testes da Camada `engine`

A camada `engine` será testada para garantir o correto funcionamento da orquestração do sistema.

O foco inicial será o **EventBus**, garantindo:

> - Registro correto de assinantes
> - Publicação de eventos
> - Processamento determinístico da fila
> - Ausência de chamadas inesperadas ou recursivas

Outros elementos como controle de tempo e agendamento de sistemas também poderão receber testes unitários conforme forem implementados.

Essa validação é essencial para manter a previsibilidade do fluxo de execução.

---
---

## 🎮 Testes da Camada `game`

A camada de jogo define as regras concretas do sistema.

Serão testados:

> - Sistemas de movimento
> - Sistema de colisão
> - Aplicação de dano
> - Alterações de estado de entidades
> - Reações a eventos

Esses testes validam o comportamento lógico das mecânicas implementadas.

---
---

## 🚫 O Que Não Será Testado Automaticamente

Alguns aspectos do projeto não serão cobertos por testes automatizados:

> - Renderização gráfica
> - Integração direta com SDL
> - Áudio
> - Criação de janelas

Esses elementos dependem diretamente da plataforma e serão verificados manualmente durante o desenvolvimento.

---
---

## ⌨️ Comandos Úteis para Testes (Catch2)

| Comando                                    | Descrição                                         |
|--------------------------------------------|---------------------------------------------------|
| `./tests`                                 | Executa todos os testes                           |
| `./tests "[NomeDoCaso]"`                  | Executa apenas o caso de teste especificado        |
| `./tests "[tag]"`                         | Executa todos os testes com a tag informada        |
| `./tests -s`                              | Mostra todas as mensagens `std::cout`              |
| `./tests -r compact`                      | Saída compacta dos resultados                      |
| `./tests -l`                              | Lista todos os casos de teste disponíveis          |
| `./tests -h`                              | Mostra todas as opções de linha de comando         |
| `./tests --success`                       | Mostra também os testes que passaram               |
| `./tests --durations yes`                 | Mostra o tempo de execução de cada teste           |
| `./tests --break`                         | Para na primeira falha (útil para depuração)       |
| `./tests --list-tags`                     | Lista todas as tags de testes                      |

> Substitua `./tests` pelo caminho do executável de testes gerado no seu projeto.

---
---