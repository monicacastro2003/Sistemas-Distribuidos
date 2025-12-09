<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <title>Cliente PHP - Calculadora Spring Boot</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { max-width: 400px; margin: auto; }
        input, select, button { margin: 10px 0; padding: 10px; width: 100%; }
        button { background: #28a745; color: white; border: none; cursor: pointer; }
        button:hover { background: #218838; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Calculadora - Consumo API Spring Boot</h2>
        <form action="calcular.php" method="GET">
            <input type="number" step="any" name="num1" placeholder="Numero 1" required>
            <input type="number" step="any" name="num2" placeholder="Numero 2" required>
            <select name="operacao">
                <option value="somar">Somar</option>
                <option value="subtrair">Subtrair</option>
                <option value="multiplicar">Multiplicar</option>
                <option value="dividir">Dividir</option>
            </select>
            <button type="submit">Calcular</button>
        </form>
    </div>
</body>
</html>
