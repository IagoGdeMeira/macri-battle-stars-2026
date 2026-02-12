# 🇧🇷 Macri Battle Stars - Dependências Externas

Afim de evitar evitar quaisquer bloqueios em computadores de redes públicas que restrinjam mudanças sem permissão de administradores, foram instaladas todas as bibliotecas necessárias para o desenvolvimento do projeto neste subdiretório. Para fins de creditação e utilização posterior, citaremos todas as bibliotecas externas utilizadas, assim como suas funções no projeto.

---
---

## 👨‍💻 Catch2 - Versão 3.12.0

Este é um *framework* de testes desenvolvido para a linguagem C++. Ele servirá para simular casos de uso nos quais as regras de negócio devem ser aplicada, oferecendo consistência e uma garantia de que o sistema está funcionando tal como foi idealizado.

Este *framework* é de uso exclusivo do subdiretório `tests` e de todos seus próprios subdiretórios. Por não depender de acesso administrativo para ser instalada, essa biblioteca não está presente no subdiretório `external`, mas será instalada no sistema quando o CMake realizar uma *build*.

Para mais informações, [acesse aqui a página oficial da versão 3.12.0 da biblioteca](https://github.com/catchorg/Catch2/releases/tag/v3.12.0).

---
---

## 💾 Nlohmann JSON - Versão 3.12.0

Esta é um biblioteca *header-only* simples utilizada pelas camadas de baixo nível para ler e escrever dados, reduzindo a estrutura do código-fonte, que terá o papel de ler e interpretar esses dados em seus devidos contextos, mas somente isso.

Para mais informações, [acesse aqui a página oficial da versão 3.12.0 da biblioteca](https://github.com/nlohmann/json/releases/tag/v3.12.0).

---
---

## 🖥️ SDL2 - Versão 2.30.4

Também conhecida como *Simple DirectMedia Layer*, é uma biblioteca utilizada para acessar comandos *I/O* (entrada/saída) do usuário, permitindo que a aplicação crie janelas e toda uma experiência baseada na forma que os comandos do usuário são interpretados. Esta biblioteca é fundamental para criar a base do jogo.

Para mais informações, [acesse aqui a página oficial da versão 2.30.4 da biblioteca](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.4).

---
---

## 🖌️ SDL2_image - Versão 2.8.2

Esta é uma biblioteca utilizada juntamente à biblioteca padrão *SDL2*, focada especificamente no carregamento, descarregamento e transformação de imagens em superfícies *SDL*.

Para mais informações, [acesse aqui a página oficial da versão 2.8.2 da biblioteca](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2)

---
---

## 🎵 SDL2_mixer - Versão 2.8.0

Esta é uma biblioteca utilizada juntamente à biblioteca padrão *SDL2*, focada em carregar e descarregar arquivos de música, assim como criar canais de som nos quais poderão tocar músicas de fundo e efeitos sonoros.

Para mais informações, [acesse aqui a página oficial da versão 2.8.0 da biblioteca](https://github.com/libsdl-org/SDL_mixer/releases/tag/release-2.8.0).

---
---

## 📝 SDL2_ttf - Versão 2.22.0

Esta é uma biblioteca utilizada juntamente à biblioteca padrão *SDL2*, focada em carregar e descarregar arquivos de fonte do formato .ttf, utilizando essas fontes para criar superfícies de texto *SDL* no jogo.

Para mais informações, [acesse aqui a página oficial da versão 2.22.0 da biblioteca](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0).

---
---