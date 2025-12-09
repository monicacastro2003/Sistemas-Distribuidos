package com.demo;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RestController;
import java.util.Locale;

@RestController
@SpringBootApplication
@CrossOrigin(origins = "*")
public class Main {
    
    @GetMapping("/")
    public String home() {
        return "API Calculadora - Sistemas Distribuídos\n" +
               "Endpoints disponíveis:\n" +
               "GET /somar/{num1}/{num2}\n" +
               "GET /subtrair/{num1}/{num2}\n" +
               "GET /multiplicar/{num1}/{num2}\n" +
               "GET /dividir/{num1}/{num2}";
    }
    
    @GetMapping("/somar/{num1}/{num2}")
    public String somar(@PathVariable double num1, @PathVariable double num2) {
        double resultado = num1 + num2;
        return String.format(Locale.US, "%.2f", resultado);
    }
    
    @GetMapping("/subtrair/{num1}/{num2}")
    public String subtrair(@PathVariable double num1, @PathVariable double num2) {
        double resultado = num1 - num2;
        return String.format(Locale.US, "%.2f", resultado);
    }
    
    @GetMapping("/multiplicar/{num1}/{num2}")
    public String multiplicar(@PathVariable double num1, @PathVariable double num2) {
        double resultado = num1 * num2;
        return String.format(Locale.US, "%.2f", resultado);
    }
    
    @GetMapping("/dividir/{num1}/{num2}")
    public String dividir(@PathVariable double num1, @PathVariable double num2) {
        if (num2 == 0) {
            return "ERRO: Divisao por zero nao permitida";
        }
        double resultado = num1 / num2;
        return String.format(Locale.US, "%.2f", resultado);
    }
    
    public static void main(String[] args) {
        SpringApplication.run(Main.class, args);
    }
}
