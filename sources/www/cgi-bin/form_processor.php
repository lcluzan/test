<?php
// On génère du HTML qui sera récupéré par ton "pipe_out" dans CgiHandler.cpp
echo "<!DOCTYPE html>\n";
echo "<html lang='fr'>\n";
echo "<head>\n";
echo "    <meta charset='UTF-8'>\n";
echo "    <title>Résultat du CGI</title>\n";
echo "    <link rel='stylesheet' href='../styles/style.css'>\n";
echo "</head>\n";
echo "<body>\n";
echo "    <header>\n";
echo "        <h1>Succès du CGI 🚀</h1>\n";
echo "    </header>\n";
echo "    <main>\n";

// PHP va lire le corps de la requête que ton serveur a écrit via pipe_in[1] !
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Récupération sécurisée des variables
    $nom = isset($_POST['nom']) ? htmlspecialchars($_POST['nom']) : 'Inconnu';
    $message = isset($_POST['message']) ? htmlspecialchars($_POST['message']) : 'Vide';

    echo "        <div style='background: #e8f5e9; padding: 15px; border-left: 5px solid #4CAF50;'>\n";
    echo "            <h3>Données traitées par execve() :</h3>\n";
    echo "            <p><strong>Nom reçu :</strong> " . $nom . "</p>\n";
    echo "            <p><strong>Message reçu :</strong> " . nl2br($message) . "</p>\n";
    echo "        </div>\n";
} else {
    echo "        <p style='color: red;'>Erreur : Veuillez utiliser la méthode POST.</p>\n";
}

echo "        <br>\n";
echo "        <a href='../test_cgi.html'>&larr; Refaire un test</a>\n";
echo "    </main>\n";
echo "</body>\n";
echo "</html>\n";
?>
