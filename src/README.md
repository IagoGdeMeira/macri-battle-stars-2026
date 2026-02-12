# 🇧🇷 Macri Battle Stars - Arquitetura em Camadas

Para separar com mais clareza as responsabilidades de cada classe, optou-se por dividir o projeto em 4 camadas principais, além de uma camada adicional na qual existe apenas o `main.cpp`. Todas as 5 camadas estarão descritas em função e arquivos de exemplo. Abaixo segue de camadas que o sistema terá.

    app -> engine -> platform -> game -> domain

---
---

## 🖥️ Camada de aplicação (`app`)

Esta é a camada de menor nível de abstração, cujas funções são  apenas inicializar e encerrar, comunicando-se com a camada `engine`. Seu único arquivo é o `main.cpp`

---
---

## 📚 Camada de domínio (`domain`)

Esta é a camada de maior nível de abstração, na qual estão isoladas as regras de negócio, formadas pela arquitetura de um *ECS*. A camada `domain` não conhece quaisquer detalhes das dependências externas, podendo ser utilizada em outros projetos ou mesmo utilizar outras dependências.

Nesta camada serão definidas as estruturas básicas de uma entidade, uma base para os componentes que podem lhe compor e dos sistemas que os orquestrarão, todos especificados na camada de jogo. Eis alguns exemplos de arquivos da camada `domain`:

> - Component.h
> - Entity.h e .cpp
> - World.h e .cpp

---
---

## 🎮 Camada de jogo (`game`)

Esta camada é a responsável por definir as estruturas definidas pela infraestrutura de `domain`, comunicando-se diretamente com essa camada para definir componentes, sistemas e eventos que estejam em acordo com as regras estabelecidas pelo domínio.

As estruturas estabelecidas na camada `game` serão utilizadas pela camada de plataforma `platform`, que utilizará essas estruturas para lidar com as operações *I/O* capturadas pelas bibliotecas *SDL*. Eis alguns exemplos de arquivos da camada `game`:

> - components/Position.h
> - components/Health.h
> - events/CollisionEvent.h
> - events/EntityDestroyedEvent.h
> - systems/MovementSystem.h e .cpp
> - systems/DamageSystem.h e .cpp

---
---

## ⚙️ Camada de motor (`engine`)

Esta camada é a responsável por gerenciar o motor do jogo, orquestrando seu *loop*, o tempo do jogo e a ordem de execução dos sistemas, permitindo uma comunicação suficiente entre `app` e a camada de plataforma `platform`.

Apesar de estar em um nível baixo de abstração, `engine` não conhece os detalhes das dependências *SDL*, apenas a biblioteca *Nlohmann JSON*, que serve para configurações e carregamento de parâmetros de jogo. Eis alguns exemplos de arquivos da camada `engine`:

> - Engine.h e .cpp
> - EventBus.h
> - SystemScheduler.h e .cpp

---
---

## 🧱 Camada de plataforma (`platform`)

Esta camada é responsável por gerir os elementos de baixo nível que utilizam as dependências externas, como a criação e configuração de janelas, a escrita de textos, o carregamento de imagens, a interação com periféricos, etc. Os princípios *SOLID* devem ser implementados com cuidado nesta camada, garantindo assim o mínimo de acoplamento com dependências externas.

A camada `platform` é a única que possui contato as bibliotecas *SDL*. Nenhuma outra camada reconhece a existência de *SDL* no projeto, e apenas a camada `engine` reconhece a existência de *Nlohmann JSON* além da própria camada `platform`, que pode usar arquivos .json para carregar mapeamentos, configurações audiovisuais, atlas de sprites para animações, etc.

Vale ressaltar que os elementos *SDL* não devem ser conhecidos pelas outras camadas, então é fundamental garantir seu isolamento ao criar estruturas agnósticas que permitam seu suporte, dado que elas agreguem valor ao produto final. Eis alguns exemplos de arquivos da camada `platform`:

> - SDLInputAdapter.h e .cpp
> - SDLRenderer.h e .cpp
> - SDLWindow.h e .cpp

---
---