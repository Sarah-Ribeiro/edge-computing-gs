# Global Solution - Edge Computing & Computer Systems

Este é um projeto com ESP32 que utiliza a biblioteca PubSubClient e WiFi para comunicação MQTT e permite o agendamento de consultas médicas remotamente. O dispositivo se conecta a uma rede Wi-Fi e a um broker MQTT para receber comandos e enviar informações sobre as consultas marcadas.
## Integrante

- Nome: Sarah Ribeiro - RM: 97747


## Documentação 

### Requisitos

- ESP32
- Conexão com uma rede Wi-Fi
- Broker MQTT (por padrão, configurado para "46.17.108.113" na porta 1883)

### Configurações

Antes de utilizar o projeto, é necessário configurar algumas variáveis no início do código:

```cpp
const char *default_SSID = "Wokwi-GUEST";           // Nome da rede Wi-Fi
const char *default_PASSWORD = "";                  // Senha da rede Wi-Fi
const char *default_BROKER_MQTT = "46.17.108.113";  // IP do Broker MQTT
const int default_BROKER_PORT = 1883;               // Porta do Broker MQTT
// ... (outros tópicos e configurações)
```

Certifique-se de ajustar essas configurações de acordo com o seu ambiente.

### Tópicos MQTT Padrão

O projeto utiliza diversos tópicos MQTT para enviar e receber informações. Os principais tópicos incluem:

- `/TEF/hosp220/cmd`: Tópico para receber comandos.
- `/TEF/hosp220/attrs`: Tópico geral para enviar informações.
- `/TEF/hosp220/attrs/nome`, `/TEF/hosp220/attrs/email`, ...: Tópicos específicos para informações detalhadas.

### Funcionalidades Principais

- Marcação de Consultas: O dispositivo aceita comandos via MQTT para marcar consultas, fornecendo informações como nome, e-mail, telefone, tipo de consulta, especialidade, data e horário.
- Modo Reset: Não é permitido o envio de informações para marcar uma consulta nesse modo.

### Rodar o fiware

```
git clone https://github.com/fabiocabrini/fiware.git

cd fiware

docker docker compose up -d
```

Também é necessário ter o arquivo do postman dentro do github para criar os comandos e atributos.

### Uso do Código

1. Carregue o código no ESP32.
2. Conecte o dispositivo à sua rede Wi-Fi.
3. Necessário ter o fiware rodando no docker
4. Ter o arquivo postman para criar os comandos e atributos
5. Certifique-se de ter um broker MQTT configurado.
6. Envie comandos MQTT para o tópico `/TEF/hosp220/cmd` para interagir com o dispositivo.
## Tech Stack

- C++
- ESP32
- MQTT
- Freeboard




## Screenshots

![App Screenshot](/Users/sarahribeiro/www/global-solution/edge-computing/gs - edge/modelo.jpeg)

