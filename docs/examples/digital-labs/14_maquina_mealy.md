# 14 Máquina Mealy

Práctica de máquina de estados tipo Mealy para detectar la secuencia `101`.

## Objetivo

Implementar una FSM donde la salida depende del estado actual y de la entrada.

## Verilog

```bash
cd digital-labs/14_maquina_mealy
devlab build
devlab flash
```

## VHDL

```bash
cd digital-labs/14_maquina_mealy
devlab build -c devlab-vhdl.toml
devlab flash
```

## Conceptos

- Estados codificados.
- Transiciones por entrada.
- Salida combinacional dependiente de estado y entrada.
