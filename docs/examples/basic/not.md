# NOT

Ejemplo de inversor lógico.

## Qué Aprendes

- Usar una entrada como señal booleana.
- Aplicar negación lógica.
- Enviar el resultado al LED activo en bajo.

## Compilar

```bash
cd not
devlab build
devlab flash
```

## VHDL

```bash
cd not
devlab build -c devlab-vhdl.toml
devlab flash
```

## Lógica

La salida cambia al valor contrario de la entrada lógica. Como el LED es activo en bajo, el `top` también considera la polaridad física del LED.
