# 🇧🇷 Convenção de Commits

Para manter este projeto organizado e com modificações cujo propósito e foco estejam de fácil visualização, foi adotado o padrão *Conventinal Commits*. A adoção deste padrão assegurará a legibilidade e organização do histórico de commits ao longo do tempo de vida do projeto. Eis abaixo o formato adotado:

    tipo(escopo opcional): descrição curta no imperativo

### Exemplos:

```
feat(character): adiciona sistema de colisão
fix(memory): corrige vazamento ao destruir entidade
refactor(core): reorganiza gerenciamento de estados
```

---
---

## 🔹 Tipos de Commit

| Tipo       | Uso |
|------------|------|
| `feat`     | Nova funcionalidade |
| `fix`      | Correção de bug |
| `refactor` | Alteração interna sem mudar comportamento externo |
| `style`    | Formatação (indentação, espaços, etc.) |
| `docs`     | Documentação |
| `test`     | Testes |
| `chore`    | Tarefas internas (configurações, dependências) |
| `build`    | Alterações no sistema de build |
| `ci`       | Integração contínua |
| `perf`     | Melhoria de desempenho |
| `remove`   | Remoção de código |

---

## 📌 Regras

- Use verbo no **imperativo**: `adiciona`, `corrige`, `remove`
- Seja **claro e direto**
- Descreva *o que* foi feito, não *como*
- Use escopo quando fizer sentido: `feat(ui)`, `fix(network)`
