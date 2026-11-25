# toolbelt

## Git identity

Claude is a tool, not an author. Every commit made from a Claude session must be
authored and committed as the repo owner. Before the first commit in a session, run:

```bash
git config user.name frank44
git config user.email frankrod44@gmail.com
```

Do not add a `Co-Authored-By: Claude ...` trailer to commit messages.
