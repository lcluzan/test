# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test_status_code.sh                                :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bchallat <bchallat@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/11 14:34:50 by bchallat          #+#    #+#              #
#    Updated: 2026/06/12 13:45:04 by bchallat         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

RED='\e[31m'
BLUE='\e[34m'
GREEN='\e[32m'
YELLOW='\e[1;33m'
DEF='\e[0m'

PORT=8080
HOST="localhost"

# ══════════════════════════════════════════════════════════════════════ #
#                                                                        #
# ══════════════════════════════════════════════════════════════════════ #

echo -e ${BLUE} ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                                                            ║"
echo "║                TEST OF STATUS CODE HTTP                    ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo -e ${DEF}"\n"

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

pause() {
  echo -e ${YELLOW}"\nAppuyer sur Entree pour continuer..." ${DEF}
  read
}

# ══════════════════════════════════════════════════════════════════════ #
#                                                                        #
# ══════════════════════════════════════════════════════════════════════ #
echo -e ${BLUE} ""
echo " -- CODE OF SUCCESSFUL 2XX --"
echo -e ${DEF}"\n"

echo -e ${YELLOW}"TEST code 200 OK: http://$HOST:$PORT/"${DEF}
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/)
check_status "200" "$STATUS" "GET "
echo ""

echo -e ${YELLOW}"TEST code 200 OK: http://$HOST:$PORT/index.html"${DEF}
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/index.html)
check_status "200" "$STATUS" "GET "
echo ""

# Test 403 Forbidden (Accès refusé sans en-tête)
echo -e ${YELLOW}"TEST 403 Forbidden: http://$HOST:8081"${DEF}
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:8081/)
check_status "403" "$STATUS" "GET /protected"
echo ""

# Test 301 Moved Permanently
echo -e ${YELLOW}"TEST 301 Redirect: http://$HOST:$PORT/old-url"${DEF}
STATUS=$(curl -s -o /dev/null -w "%{http_code}" --max-redirs 0 http://$HOST:$PORT/old-url)
check_status "301" "$STATUS" "GET /old-url"
echo ""

#echo -e ${YELLOW}"TEST 201 Created: POST http://$HOST:$PORT/upload/text.txt"${DEF}
#STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST -d '{"key":"value"}' http://$HOST:$PORT/upload/text.txt)
#check_status "201" "$STATUS" "POST /create"
#echo ""

echo -e ${YELLOW}"TEST code 204 NO CONTENT: http://$HOST:$PORT/upload/exemple.txt"${DEF}
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE http://$HOST:$PORT/upload/exemple.txt)
check_status "204" "$STATUS" "DELETE "
echo ""

echo -e ${YELLOW}"TEST code 403 NO CONTENT: http://$HOST:$PORT/upload/forriben-delete.txt"${DEF}
STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE http://$HOST:$PORT/upload/forriben-delete.txt)
check_status "403" "$STATUS" "DELETE "
echo ""
