#/bin/bash

RED='\e[31m'
BLUE='\e[34m'
GREEN='\e[32m'
YELLOW='\e[1;33m'
DEF='\e[0m'

PORT=8080
HOST="localhost"

# Pause pour lire
pause() {
  echo -e "\nAppuyer sur Entree pour continuer..."
  read
}

echo -e ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                                                            ║"
echo "║                      WEBSERV TESTER                        ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo -e "\n"

echo "Verifying/Creating the directory ./static/www/files..."
mkdir -p ./static/www/files
pause
echo "Usage: launch webserv executable with config/script-config.conf in one terminal"
echo "       run this script in another terminal"
pause


# Fonction pour afficher les resultats
check_status() {
  local target=$1
  local server=$2
  local methode=$3

  if [ "$target" = "$server" ]; then
    echo -e ${GREEN}"   methode: ${methode} | return of serveur: ${server} | target: ${target}" ${DEF}
    return 0
  else
    echo -e ${RED}"   methode: ${methode} | return of serveur: ${server} | target: ${target}" ${DEF}
    return 1
  fi
}

# Preparation des fichiers de test
echo -e " Preparation des fichiers de test..."
echo "Hello from webserv test" >test.txt
echo "<h1>Test HTML</h1><p>This is a test page.</p>" >test.html
dd if=/dev/urandom of=binary.bin bs=1024 count=5 2>/dev/null
echo '{"name":"Lou","project":"webserv","status":"ready"}' >test.json
echo "Large file test content" >large.txt
for i in {1..100}; do echo "Line $i of large file" >>large.txt; done
echo -e " Fichiers crees\n"

sleep 1

# ═══════════════════════════════════════════════════════════════
echo -e "╔════════════════════════════════════════╗"
echo -e "║          TESTS GET REQUESTS            ║"
echo -e "╔════════════════════════════════════════╝\n"

echo -e "Test 1.1: GET page d'accueil (200 OK)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/)
check_status "200" "$STATUS" "GET /"
echo ""

echo -e "Test 1.2: GET fichier inexistant (404 Not Found)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/this-does-not-exist.html)
check_status "404" "$STATUS" "GET /this-does-not-exist.html"
echo ""

echo -e "Test 1.3: GET avec curl verbose"
echo -e "Commande: curl -v http://$HOST:$PORT/"
curl -v http://$HOST:$PORT/ 2>&1 | head -20
pause

# ═══════════════════════════════════════════════════════════════
echo -e "\n╔════════════════════════════════════════╗"
echo -e "║          TESTS POST REQUESTS           ║"
echo -e "╔════════════════════════════════════════╝\n"

echo -e "Test 2.1: POST fichier texte (201 Created)"
echo -e "Commande: curl -X POST http://$HOST:$PORT/files -F \"file=@test.txt\""
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST http://$HOST:$PORT/files -F "file=@test.txt")
STATUS=$(echo "$RESPONSE" | tail -1)
BODY=$(echo "$RESPONSE" | head -1)
check_status "201" "$STATUS" "POST /files (test.txt)"
echo -e "  Response body: $BODY"
echo ""

echo -e "Test 2.2: POST fichier HTML (201 Created)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://$HOST:$PORT/files -F "file=@test.html")
check_status "201" "$STATUS" "POST /files (test.html)"
echo ""

echo -e "Test 2.3: POST fichier binaire (201 Created)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://$HOST:$PORT/files -F "file=@binary.bin")
check_status "201" "$STATUS" "POST /files (binary.bin)"
echo ""

echo -e "Test 2.4: POST fichier JSON (201 Created)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://$HOST:$PORT/files -F "file=@test.json")
check_status "201" "$STATUS" "POST /files (test.json)"
echo ""

echo -e "Test 2.5: POST fichier volumineux (201 Created)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://$HOST:$PORT/files -F "file=@large.txt")
check_status "201" "$STATUS" "POST /files (large.txt)"
pause

# ═══════════════════════════════════════════════════════════════
echo -e "\n╔════════════════════════════════════════╗"
echo -e "║          RETRIEVE UPLOADED FILES       ║"
echo -e "╔════════════════════════════════════════╝\n"

echo -e "Test 3.1: Recuperation de test.txt"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/files/test.txt)
check_status "200" "$STATUS" "GET /files/test.txt"
RETRIEVED=$(curl -s http://$HOST:$PORT/files/test.txt)
ORIGINAL=$(cat test.txt)
if [ "$RETRIEVED" = "$ORIGINAL" ]; then
  echo -e "   Contenu identique à l'original"
else
  echo -e "   Contenu different"
fi
echo -e "  Contenu recupere: $RETRIEVED"
echo ""

echo -e "Test 3.2: Recuperation de test.html"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/files/test.html)
check_status "200" "$STATUS" "GET /files/test.html"
echo ""

echo -e "Test 3.3: Recuperation de binary.bin"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/files/binary.bin)
check_status "200" "$STATUS" "GET /files/binary.bin"
curl -s http://$HOST:$PORT/files/binary.bin >retrieved_binary.bin
if cmp -s binary.bin retrieved_binary.bin; then
  echo -e "   Fichier binaire identique (verification byte à byte)"
  rm retrieved_binary.bin
else
  echo -e "   Fichier binaire corrompu"
fi
echo ""

echo -e "Test 3.4: Recuperation de test.json"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/files/test.json)
check_status "200" "$STATUS" "GET /files/test.json"
RETRIEVED_JSON=$(curl -s http://$HOST:$PORT/files/test.json)
echo -e "  JSON recupere: $RETRIEVED_JSON"
pause

# ═══════════════════════════════════════════════════════════════
echo -e "\n╔════════════════════════════════════════╗"
echo -e "║          TESTS DELETE REQUESTS         ║"
echo -e "╔════════════════════════════════════════╝\n"

echo -e "Test 4.1: DELETE fichier existant (204 No Content)"
echo -e "Commande: curl -v -X DELETE http://$HOST:$PORT/files/test.txt"
curl -v -X DELETE http://$HOST:$PORT/files/test.txt 2>&1 | grep -E "(DELETE|< HTTP|< Content)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE http://$HOST:$PORT/files/test.txt 2>/dev/null || echo "404")
echo ""

echo -e "Test 4.2: Verifier que le fichier n'existe plus (404)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/files/test.txt)
check_status "404" "$STATUS" "GET /files/test.txt (après DELETE)"
echo ""

echo -e "Test 4.3: DELETE fichier inexistant (404 Not Found)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE http://$HOST:$PORT/files/file-that-never-existed.txt)
check_status "404" "$STATUS" "DELETE /files/file-that-never-existed.txt"
echo ""

echo -e "Test 4.4: DELETE autres fichiers"
curl -s -X DELETE http://$HOST:$PORT/files/test.html >/dev/null
echo -e "   test.html supprime"
curl -s -X DELETE http://$HOST:$PORT/files/binary.bin >/dev/null
echo -e "   binary.bin supprime"
curl -s -X DELETE http://$HOST:$PORT/files/test.json >/dev/null
echo -e "   test.json supprime"
curl -s -X DELETE http://$HOST:$PORT/files/large.txt >/dev/null
echo -e "   large.txt supprime"
pause

# ═══════════════════════════════════════════════════════════════
echo -e "\n╔════════════════════════════════════════╗"
echo -e "║          UNKNOWN/INVALID METHODS       ║"
echo -e "║        (Serveur ne doit PAS crash)     ║"
echo -e "╔════════════════════════════════════════╝\n"

echo -e "Test 5.1: Méthode UNKNOWN"
STATUS=$(echo -e "UNKNOWN / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc -w 2 localhost 8080 2>/dev/null | head -1)
if [ ! -z "$STATUS" ]; then
  echo -e " Serveur a répondu: $STATUS"
  echo -e "   Serveur n'a pas crashé"
else
  echo -e " Pas de réponse (timeout ou crash?)"
fi
echo ""

echo -e "Test 5.2: Methode PUT"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X PUT http://$HOST:$PORT/)
echo -e "   PUT retourne: $STATUS (serveur OK)"
echo ""

echo -e "Test 5.3: Methode PATCH"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X PATCH http://$HOST:$PORT/)
echo -e "   PATCH retourne: $STATUS (serveur OK)"
echo ""

echo -e "Test 5.4: Methode OPTIONS"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X OPTIONS http://$HOST:$PORT/)
echo -e "   OPTIONS retourne: $STATUS (serveur OK)"
echo ""

echo -e "Test 5.5: Methode TRACE"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X TRACE http://$HOST:$PORT/)
echo -e "   TRACE retourne: $STATUS (serveur OK)"
echo ""

echo -e "Test 5.6: Requête complètement invalide"
timeout 3 bash -c 'echo -e "THIS IS NOT HTTP\r\n\r\n" | nc localhost 8080' >/dev/null 2>&1
if [ $? -ne 124 ]; then
  echo -e "   Serveur a gere la requête invalide sans crash"
else
  echo -e "  ⚠  Timeout (possible mais serveur doit tourner)"
fi
pause

# ═══════════════════════════════════════════════════════════════
echo -e "\n╔════════════════════════════════════════╗"
echo -e "║          DEMO AVEC TELNET              ║"
echo -e "╔════════════════════════════════════════╝\n"

echo -e "Comment tester avec telnet:\n"

echo -e "Pour GET:"
echo -e "telnet localhost 8080"
echo -e "GET / HTTP/1.1"
echo -e "Host: localhost"
echo -e "(Appuyer sur Entree deux fois)\n"

echo -e "Pour POST:"
echo -e "telnet localhost 8080"
echo -e "POST /files/hello.txt HTTP/1.1"
echo -e "Host: localhost"
echo -e "Content-Type: text/plain"
echo -e "Content-Length: 12"
echo -e ""
echo -e "Hello World"
echo -e "(Entree après le contenu)\n"

echo -e "Pour DELETE:"
echo -e "telnet localhost 8080"
echo -e "DELETE /files/test.txt HTTP/1.1"
echo -e "Host: localhost"
echo -e "(Appuyer sur Entree deux fois)\n"

echo -e "Pour methode UNKNOWN:"
echo -e "telnet localhost 8080"
echo -e "UNKNOWN / HTTP/1.1"
echo -e "Host: localhost"
echo -e "(Appuyer sur Entree deux fois)\n"

pause

# Cleanup
echo -e "\n Cleaning up created files"

# 1. Remove the local test files generated in the root
rm -f test.txt test.html binary.bin test.json large.txt retrieved_binary.bin

# 2. Remove the uploaded files from the server's files directory
rm -f ./static/www/files/test.txt \
      ./static/www/files/test.html \
      ./static/www/files/binary.bin \
      ./static/www/files/test.json \
      ./static/www/files/large.txt
rm -r ./static/www/files

echo -e " fichiers supprimes"

echo "test de siege"

pause

siege -b -c 10 -r 100 http://127.0.0.1:8080/html/index.html

pause
# Resume final
echo -e "\n"
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                       TESTS TERMINES                       ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo -e "\n"

