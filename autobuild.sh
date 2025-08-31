#!/usr/bin/env bash
set -euo pipefail

# Permite override externo:
#   CMAKE_PREFIX_PATH=/ruta/a/Qt6 ./autobuild.sh
: "${BUILD_DIR:=build}"
: "${CONFIG:=Release}"
: "${GENERATOR:=}"   # ej: "Ninja"

# Detecta Qt6 si no viene por env
if [[ -z "${CMAKE_PREFIX_PATH:-}" ]]; then
  for p in /usr/lib/x86_64-linux-gnu/cmake/Qt6 /usr/lib/cmake/Qt6; do
    if [[ -d "$p" ]]; then CMAKE_PREFIX_PATH="$p"; break; fi
  done
fi
if [[ -z "${CMAKE_PREFIX_PATH:-}" ]]; then
  echo "❌ No pude detectar Qt6. Definí CMAKE_PREFIX_PATH o instalá Qt6."
  exit 1
fi

echo "▶ Configurando (Qt6: $CMAKE_PREFIX_PATH, build: $BUILD_DIR, config: $CONFIG)"
cmake -S . -B "$BUILD_DIR" \
  ${GENERATOR:+-G "$GENERATOR"} \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "▶ Compilando"
cmake --build "$BUILD_DIR" --config "$CONFIG" --parallel
echo "✅ Build OK → $BUILD_DIR"
