# 🇧🇷 Sistema de Eventos
O Sistema de Eventos fornece um modelo de despacho determinístico, executado na thread principal, com suporte a postagem de eventos a partir de múltiplas threads de forma segura. Ele é composto por dois componentes principais:

- `EventBus`: responsável por armazenar e executar handlers.
- `EventDispatcher`: responsável por enfileirar eventos de forma thread-safe e despachá-los na thread principal. Único que pode chamar `EventBus::emit()`.

---
---

## Modelo Arquitetural
1. **Threads Secundárias**
2. **dispatcher.post()** (fila protegida por mutex)
3. **Thread Principal**
4. **dispatcher.process()**
5. **bus.emit()**
6. **Handlers Inscritos**

---
---

## Princípios de Projeto
- `EventBus` **não é thread-safe**
- `EventDispatcher::post()` é **thread-safe**
- `EventDispatcher::process()` deve ser chamado na thread principal
- `EventBus::emit()` é privado e acessível apenas ao `EventDispatcher`
- Eventos postados durante um dispatch são adiados para o próximo ciclo de `process()`
- A iteração de handlers utiliza cópia (snapshot) para evitar comportamento indefinido

---
---

## Garantias do Sistema

### Segurança de Thread

- Chamadas concorrentes a `post()` são protegidas por `std::mutex`
- O estado interno do `EventBus` é acessado exclusivamente na thread principal
- Não há estado mutável compartilhado entre threads sem proteção

### Determinismo

- A fila de eventos preserva ordem FIFO
- Handlers são executados na ordem de inscrição
- Postagens reentrantes são adiadas para o próximo ciclo

### Segurança Durante Dispatch

O sistema suporta com segurança:

- `unsubscribe()` durante execução de handler
- `subscribe()` durante execução de handler
- `clear()` durante execução de handler
- Postagem recursiva de eventos (via dispatcher)

Isso é possível graças à utilização de snapshot da lista de handlers durante o `emit()`.

---
---