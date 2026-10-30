# Juez en línea: patrones de diseño en C++

Módulo simplificado de un juez en línea para competencias de programación. El staff crea y sube problemas, los concursantes reciben una notificación, envían su solución y el juez devuelve un veredicto. Todo corre en terminal.

## Tecnologías

- C++17
- g++ (o cualquier compilador compatible con C++17)

## Patrones de diseño usados

| Tipo | Patrón | Clases | Razón |
|---|---|---|---|
| Creacional | Builder | `ProblemBuilder`, `Problem` | `Problem` tiene varios atributos, el builder lo arma paso a paso y es la única forma de crearlo (constructor privado). |
| Estructural | Decorator | `Judge`, `ServerJudge`, `LoggingJudge` | Agrega logging a la evaluación sin modificar `ServerJudge`, usando la misma interfaz `Judge`. |
| Comportamiento | Observer | `Observer`, `ServerRepository`, `Contestant` | El repositorio notifica a los concursantes suscritos cuando se sube un problema, sin conocer sus clases concretas. |

Cada patrón está explicado en comentarios dentro de `judge_patterns.cpp`.

## Cómo ejecutar

```bash
g++ -std=c++17 judge_patterns.cpp -o judge
./judge
```

## Salida esperada

```
staff01 is uploading the problem...
[zebnel notified] New problem available: A + B
Upload complete
zebnel is submitting problem <id>
[LOG] Judging problem <id> (26 bytes)
[LOG] Verdict: accepted
Verdict received: accepted
```

El `<id>` se genera a partir del reloj del sistema, por lo que cambia en cada ejecución.

## Estructura

```
.
├── judge_patterns.cpp
└── README.md
```

## Autor

Leonel Zeballos Aldunate
