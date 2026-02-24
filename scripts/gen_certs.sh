#!/usr/bin/env bash
# =============================================================================
# gen_certs.sh — Generate a self-signed TLS certificate for the HFT server
#
# Usage:
#   ./scripts/gen_certs.sh [OPTIONS]
#
# Options:
#   -o, --out-dir  DIR     Output directory          (default: certs)
#   -b, --bits     BITS    RSA key size in bits       (default: 2048)
#   -d, --days     DAYS    Certificate validity days  (default: 365)
#   -c, --cn       CN      Common Name (hostname)     (default: localhost)
#       --country  C       Country code               (default: US)
#       --org      O       Organisation name          (default: HFT Server)
#   -h, --help             Show this help and exit
#
# Output:
#   <out-dir>/server.key   RSA private key  (permissions 600)
#   <out-dir>/server.crt   Self-signed X.509 certificate
#
# Requirements:
#   - gcc (any version) + OpenSSL development headers and libraries
#     (libssl-dev / openssl-devel), OR
#   - the openssl CLI tool (used automatically if found on PATH)
# =============================================================================
set -euo pipefail

# ── Defaults ──────────────────────────────────────────────────────────────────
OUT_DIR="certs"
KEY_BITS=2048
DAYS=365
CN="localhost"
COUNTRY="US"
ORG="HFT Server"

# ── Argument parsing ──────────────────────────────────────────────────────────
usage() {
    sed -n '3,20p' "$0" | sed 's/^# \{0,1\}//'
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -o|--out-dir)  OUT_DIR="$2";  shift 2 ;;
        -b|--bits)     KEY_BITS="$2"; shift 2 ;;
        -d|--days)     DAYS="$2";     shift 2 ;;
        -c|--cn)       CN="$2";       shift 2 ;;
        --country)     COUNTRY="$2";  shift 2 ;;
        --org)         ORG="$2";      shift 2 ;;
        -h|--help)     usage ;;
        *) echo "Unknown option: $1" >&2; exit 1 ;;
    esac
done

KEY_FILE="${OUT_DIR}/server.key"
CRT_FILE="${OUT_DIR}/server.crt"

# ── Resolve script location so it works from any cwd ─────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUT_DIR_ABS="${REPO_ROOT}/${OUT_DIR}"

mkdir -p "${OUT_DIR_ABS}"

KEY_FILE_ABS="${OUT_DIR_ABS}/server.key"
CRT_FILE_ABS="${OUT_DIR_ABS}/server.crt"

echo "==> Generating TLS certificate"
echo "    Output dir : ${OUT_DIR_ABS}"
echo "    Key size   : ${KEY_BITS} bits"
echo "    Valid for  : ${DAYS} days"
echo "    CN         : ${CN}"

# ── Method 1: openssl CLI ─────────────────────────────────────────────────────
if command -v openssl &>/dev/null; then
    echo "==> Using openssl CLI"
    openssl req -x509 -newkey "rsa:${KEY_BITS}" \
        -keyout "${KEY_FILE_ABS}" \
        -out    "${CRT_FILE_ABS}" \
        -days   "${DAYS}" \
        -nodes  \
        -subj   "/C=${COUNTRY}/O=${ORG}/CN=${CN}"

# ── Method 2: compile + run a small C helper using the OpenSSL API ────────────
elif command -v gcc &>/dev/null; then
    echo "==> openssl CLI not found — compiling C helper with OpenSSL API"

    TMP_SRC="$(mktemp /tmp/gen_cert_XXXXXX.c)"
    TMP_BIN="$(mktemp /tmp/gen_cert_XXXXXX)"
    trap 'rm -f "${TMP_SRC}" "${TMP_BIN}"' EXIT

    cat > "${TMP_SRC}" << CSRC
#include <stdio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

int main(int argc, char *argv[])
{
    if (argc != 6) {
        fprintf(stderr, "usage: gen_cert <key_file> <crt_file> <bits> <days> <cn>\n");
        return 1;
    }
    const char *key_file = argv[1];
    const char *crt_file = argv[2];
    int bits  = atoi(argv[3]);
    int days  = atoi(argv[4]);
    const char *cn = argv[5];

    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!pctx || EVP_PKEY_keygen_init(pctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, bits) <= 0 ||
        EVP_PKEY_keygen(pctx, &pkey) <= 0) {
        fprintf(stderr, "Key generation failed\n"); return 1;
    }
    EVP_PKEY_CTX_free(pctx);

    FILE *f = fopen(key_file, "wb");
    if (!f || !PEM_write_PrivateKey(f, pkey, NULL, NULL, 0, NULL, NULL))
        { fprintf(stderr, "Cannot write key\n"); return 1; }
    fclose(f);

    X509 *x = X509_new();
    ASN1_INTEGER_set(X509_get_serialNumber(x), 1);
    X509_gmtime_adj(X509_get_notBefore(x), 0);
    X509_gmtime_adj(X509_get_notAfter(x), (long)days * 86400);
    X509_set_pubkey(x, pkey);

    X509_NAME *name = X509_get_subject_name(x);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char*)"US",       -1,-1,0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char*)"HFTServer",-1,-1,0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)cn,         -1,-1,0);
    X509_set_issuer_name(x, name);
    X509_sign(x, pkey, EVP_sha256());

    f = fopen(crt_file, "wb");
    if (!f || !PEM_write_X509(f, x))
        { fprintf(stderr, "Cannot write cert\n"); return 1; }
    fclose(f);

    EVP_PKEY_free(pkey);
    X509_free(x);
    printf("Written: %s\nWritten: %s\n", key_file, crt_file);
    return 0;
}
CSRC

    gcc "${TMP_SRC}" -o "${TMP_BIN}" -lssl -lcrypto
    "${TMP_BIN}" "${KEY_FILE_ABS}" "${CRT_FILE_ABS}" "${KEY_BITS}" "${DAYS}" "${CN}"

else
    echo "ERROR: Neither 'openssl' CLI nor 'gcc' was found. Install one and retry." >&2
    exit 1
fi

# ── Secure the private key ────────────────────────────────────────────────────
chmod 600 "${KEY_FILE_ABS}"
chmod 644 "${CRT_FILE_ABS}"

echo "==> Done"
echo "    Private key  : ${KEY_FILE_ABS}  (mode 600)"
echo "    Certificate  : ${CRT_FILE_ABS}  (mode 644)"
echo ""
echo "    Run the server with:"
echo "      ./build/hft_server_exe 8443 ${OUT_DIR}/server.crt ${OUT_DIR}/server.key"
