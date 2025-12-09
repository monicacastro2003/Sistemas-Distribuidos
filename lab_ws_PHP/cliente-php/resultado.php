<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <title>Resultado</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { max-width: 400px; margin: auto; text-align: center; }
        .resultado { font-size: 24px; margin: 20px 0; padding: 20px; background: #f8f9fa; }
        a { color: #28a745; text-decoration: none; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Resultado da Operacao</h2>
        <div class="resultado">
            <?php 
            if (isset($_GET['resultado'])) {
                echo htmlspecialchars($_GET['resultado']);
            } else {
                echo "Nenhum resultado encontrado.";
            }
            ?>
        </div>
        <a href="index.php">Nova Operacao</a>
    </div>
</body>
</html>
