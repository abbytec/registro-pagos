# Registro de Pagos (Qt6 + SQLite)

Aplicación de escritorio sencilla para registrar pagos asociados a alumnos. Está construida con C++17 y Qt6 (Widgets + SQL) y persiste los datos en una base SQLite en el directorio de datos del usuario. Incluye un sistema de licencias con límite de registros y una utilidad CLI para generar claves de licencia.

## Características

- Gestión de alumnos: documento + nombre completo (ambos obligatorios).
- Registro de pagos: monto, fecha, método y notas.
- Búsqueda por documento o nombre con resultados tabulares.
- Límite de registros controlado por licencia; interfaz para aplicar clave.
- Base de datos SQLite local y migraciones automáticas al iniciar.

## Requisitos

- CMake ≥ 3.21
- Compilador C++ con soporte C++17 (g++/clang++)
- Qt6: módulos Widgets, Sql y Core (y el plugin SQLite)

## Compilación rápida

Puedes usar el script de conveniencia que detecta Qt6 e invoca CMake:

```bash
./autobuild.sh
```

Si Qt6 no está en una ruta estándar, exporta `CMAKE_PREFIX_PATH` antes de compilar:

```bash
CMAKE_PREFIX_PATH=/ruta/a/Qt/6.x/gcc_64/lib/cmake/Qt6 ./autobuild.sh
```

También puedes construir manualmente:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/ruta/a/Qt6
cmake --build build --config Release --parallel
```

## Ejecución

Con los binarios compilados en `build/`:

```bash
# Ejecutar la app gráfica
./run.sh app

# O directamente
./build/pagos
```

La app crea y usa una base SQLite en el directorio de datos del usuario. En Linux típicamente será algo como `~/.local/share/PagosApp/pagos.db`. Si Qt no puede determinar el directorio de datos, la app usa `~/.pagosapp/pagos.db` como fallback.

## Uso de la aplicación

- Pestaña Alumnos:
  - Ingresá Documento y Nombre completo y presioná “Agregar”. Ambos campos son obligatorios; si falta uno, no se agrega y se muestra un mensaje.
- Pestaña Pagos:
  - Indicá el Documento del alumno, Monto, Fecha, Método y Notas (opcional) y presioná “Registrar pago”.
- Pestaña Buscar:
  - Escribí parte del documento o del nombre para listar pagos relacionados.
- Pestaña Licencia:
  - Pegá la clave de licencia y aplicala para aumentar el límite de registros.

## Sistema de licencias

La app limita la cantidad de registros (pagos) permitidos. Sin licencia válida, el límite por defecto es 100.

- Clave de licencia: es un string con el formato `payload.signature`, donde `payload` es JSON codificado en Base64-URL y `signature` es un hash SHA-256 (hex) sobre el payload con un secreto embebido.
- La app verifica la firma, el límite (`limit`) y la expiración opcional (`expiry`, YYYY-MM-DD).
- La pestaña “Licencia” permite aplicar la clave y muestra el uso actual vs. el límite.

### Generar una licencia (CLI)

Se incluye un binario `licgen` para generar claves de licencia de forma interactiva:

```bash
# Ejecutar el generador de licencias
./run.sh lic
```

El generador pedirá: Titular (name), Límite (limit) y Expira (expiry, opcional), y emitirá una clave lista para pegar en la app.

## Estructura del proyecto

- `src/ui/MainWindow.*`: Interfaz principal (tabs: Alumnos, Pagos, Buscar, Licencia).
- `src/data/Database.*`: Inicialización de SQLite y ubicación del archivo.
- `src/data/Migrations.*`: Creación de tablas/índices (`students`, `payments`, `app_config`).
- `src/data/repository/StudentRepository.*`: Altas y modelo de alumnos (valida campos requeridos).
- `src/data/repository/PaymentRepository.*`: Altas/listado/búsqueda de pagos.
- `src/LicenseManager.*`: Verificación/aplicación de licencias y cálculo de límites.
- `tools/LicGen.cpp`: Generador de licencias (CLI).

## Notas de desarrollo

- Formato/estilo: C++17, Qt idiomático, `QSql*` para acceso a datos.
- Migraciones: se ejecutan al iniciar; seguras si ya existen las tablas/índices.
- Tablas principales:
  - `students(id, doc UNIQUE NOT NULL, full_name NOT NULL, created_at)`
  - `payments(id, student_id FK, amount, paid_at, method, notes)`

## Solución de problemas

- “No pude detectar Qt6”: exportá `CMAKE_PREFIX_PATH` apuntando al paquete de Qt6 antes de ejecutar `autobuild.sh`.
- “No se pudo abrir la base de datos”: verificá permisos sobre el directorio de datos del usuario y la ruta indicada por el mensaje.
- El binario `pagos` no abre: ejecutá `./autobuild.sh` para recompilar con el Qt correcto.

## Licencia

Ver `LICENSE` en la raíz del repositorio.
