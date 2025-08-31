#!/usr/bin/env bash
set -euo pipefail

: "${BUILD_DIR:=build}"
APP="$BUILD_DIR/pagos"
LIC="$BUILD_DIR/licgen"

usage() {
  echo "Uso: $0 {app|lic}"
  echo "  app  → ejecutar la aplicación gráfica"
  echo "  lic  → ejecutar el generador de licencias (CLI)"
  exit 1
}

ensure_build() {
  if [[ ! -x "$APP" || ! -x "$LIC" ]]; then
    echo "ℹ️  No encontré binarios. Compilando..."
    ./autobuild.sh
  fi
}

[[ $# -ge 1 ]] || usage
case "$1" in
  app)
    ensure_build
    exec "$APP"
    ;;
  lic)
    ensure_build
    exec "$LIC"
    ;;
  *)
    usage
    ;;
esac
