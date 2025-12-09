<?php
$num1 = $_GET['num1'] ?? 0;
$num2 = $_GET['num2'] ?? 0;
$operacao = $_GET['operacao'] ?? 'somar';

$url = "http://localhost:8080/$operacao/$num1/$num2";

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_TIMEOUT, 5);

$resposta = curl_exec($ch);

if (curl_errno($ch)) {
    $erro = "Erro na conexao com a API: " . curl_error($ch);
    curl_close($ch);
    header("Location: index.php?erro=" . urlencode($erro));
    exit();
}

curl_close($ch);
header("Location: resultado.php?resultado=" . urlencode($resposta));
?>
