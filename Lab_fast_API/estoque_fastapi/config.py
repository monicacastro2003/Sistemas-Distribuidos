import configparser

# Cria um objeto ConfigParser
config = configparser.ConfigParser()

# Propriedades padrão
config['DEFAULT'] = {
    'arquivo': 'estoque.xml',
}

# Grava no arquivo config.ini
with open('config.ini', 'w') as configfile:
    config.write(configfile)
