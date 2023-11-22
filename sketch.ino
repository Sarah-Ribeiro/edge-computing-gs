#include <WiFi.h>
#include <PubSubClient.h>

// Configurações - variáveis editáveis
const char *default_SSID = "Wokwi-GUEST";          // Nome da rede Wi-Fi
const char *default_PASSWORD = "";                 // Senha da rede Wi-Fi
const char *default_BROKER_MQTT = "46.17.108.113"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883;              // Porta do Broker MQTT
const char *default_TOPICO_SUBSCRIBE = "/TEF/hosp220/cmd";
const char *default_TOPICO_PUBLISH_1 = "/TEF/hosp220/attrs";
const char *default_TOPICO_PUBLISH_2 = "/TEF/hosp220/attrs/nome";
const char *default_TOPICO_PUBLISH_3 = "/TEF/hosp220/attrs/email";
const char *default_TOPICO_PUBLISH_4 = "/TEF/hosp220/attrs/telefone";
const char *default_TOPICO_PUBLISH_5 = "/TEF/hosp220/attrs/tipo_de_consulta";
const char *default_TOPICO_PUBLISH_6 = "/TEF/hosp220/attrs/especialidade";
const char *default_TOPICO_PUBLISH_7 = "/TEF/hosp220/attrs/data";
const char *default_TOPICO_PUBLISH_8 = "/TEF/hosp220/attrs/horario";
const char *default_ID_MQTT = "fiware_220"; // ID MQTT
const int default_D4 = 2;                   // Pino do LED onboard
// Declaração da variável para o prefixo do tópico
const char *topicPrefix = "hosp220";

char *SSID = const_cast<char *>(default_SSID);
char *PASSWORD = const_cast<char *>(default_PASSWORD);
char *BROKER_MQTT = const_cast<char *>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char *TOPICO_SUBSCRIBE = const_cast<char *>(default_TOPICO_SUBSCRIBE);
char *TOPICO_PUBLISH_1 = const_cast<char *>(default_TOPICO_PUBLISH_1);
char *TOPICO_PUBLISH_2 = const_cast<char *>(default_TOPICO_PUBLISH_2);
char *TOPICO_PUBLISH_3 = const_cast<char *>(default_TOPICO_PUBLISH_3);
char *TOPICO_PUBLISH_4 = const_cast<char *>(default_TOPICO_PUBLISH_4);
char *TOPICO_PUBLISH_5 = const_cast<char *>(default_TOPICO_PUBLISH_5);
char *TOPICO_PUBLISH_6 = const_cast<char *>(default_TOPICO_PUBLISH_6);
char *TOPICO_PUBLISH_7 = const_cast<char *>(default_TOPICO_PUBLISH_7);
char *TOPICO_PUBLISH_8 = const_cast<char *>(default_TOPICO_PUBLISH_8);
char *ID_MQTT = const_cast<char *>(default_ID_MQTT);

bool pacienteMarcouConsulta = false;
String nomePaciente;
String emailPaciente;
String telefonePaciente;
String tipoDeConsultaPaciente;
String especialidadePaciente;
String horarioPaciente;
String dataPaciente;

WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';

void initSerial()
{
  Serial.begin(115200);
}

void initWiFi()
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");
  reconectWiFi();
}

void initMQTT()
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

const char *tipoDeConsultaOptions[] = {"presencial", "digital"};
const char *especialidadeOptions[] = {"oftalmologista", "cardiologista", "hematologista", "ortopedista"};

bool isValidOption(const char *option, const char *options[], int length)
{
  for (int i = 0; i < length; i++)
  {
    if (strcmp(option, options[i]) == 0)
    {
      return true;
    }
  }
  return false;
}

void enviarMensagemSucesso()
{
  String mensagem = "Consulta marcada com sucesso!\n";
  mensagem += "Nome: " + nomePaciente + "\n";
  mensagem += "Email: " + emailPaciente + "\n";
  mensagem += "Telefone: " + telefonePaciente + "\n";
  mensagem += "Tipo de Consulta: " + tipoDeConsultaPaciente + "\n";
  mensagem += "Especialidade: " + especialidadePaciente + "\n";
  mensagem += "Data da Consulta: " + dataPaciente + "\n";
  mensagem += "Horário da Consulta: " + horarioPaciente;

  // Envie a mensagem para onde desejar (por exemplo, através do MQTT)
  Serial.println(mensagem);
}

void setup()
{
  InitOutput();
  initSerial();
  initWiFi();
  initMQTT();
  delay(5000);
  MQTT.publish(TOPICO_PUBLISH_1, "s|send");
}

void loop()
{
  VerificaConexoesWiFIEMQTT();
  EnviaEstadoOutputMQTT();

  if (pacienteMarcouConsulta)
  {
    enviarMensagemSucesso();
    pacienteMarcouConsulta = false; // Resetar a flag
  }

  MQTT.loop();
}

void reconectWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
    return;
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
  String msg;
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  // Forma o padrão de tópico para comparação
  String onTopic = String(topicPrefix) + "@send|";
  String offTopic = String(topicPrefix) + "@reset|";
  String nomeTopic = String(topicPrefix) + "@nome|";
  String emailTopic = String(topicPrefix) + "@email|";
  String telefoneTopic = String(topicPrefix) + "@telefone|";
  String tipoDeConsultaTopic = String(topicPrefix) + "@tipo_de_consulta|";
  String especialidadeTopic = String(topicPrefix) + "@especialidade|";
  String dataTopic = String(topicPrefix) + "@data|";
  String horarioTopic = String(topicPrefix) + "@horario|";

  // Compara com o tópico recebido
  if (msg.equals(onTopic))
  {
    EstadoSaida = '1';
  }

  if (msg.equals(offTopic))
  {
    EstadoSaida = '0';
  }

  if (msg.startsWith(emailTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar o email do paciente no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }
    String emailPaciente = msg.substring(emailTopic.length());
    pacienteEmail(emailPaciente.c_str());
    EstadoSaida = 'email';
  }

  if (msg.startsWith(nomeTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar o nome do paciente no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }

    String nome = msg.substring(nomeTopic.length());
    pacienteNome(nome.c_str());
    EstadoSaida = 'nome';
  }
  if (msg.startsWith(telefoneTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar o telefone do paciente no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }

    String telefone = msg.substring(telefoneTopic.length());
    pacienteTelefone(telefone.c_str());
    EstadoSaida = 'telefone';
  }
  if (msg.startsWith(tipoDeConsultaTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar o tipo de consulta do paciente no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }

    String tipoDeConsulta = msg.substring(tipoDeConsultaTopic.length());
    pacienteTipoDeConsulta(tipoDeConsulta.c_str());
    EstadoSaida = 'tipoDeConsulta';
  }
  if (msg.startsWith(especialidadeTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar o especialidade do médico no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }

    String especialidade = msg.substring(especialidadeTopic.length());
    pacienteEspecialidade(especialidade.c_str());
    EstadoSaida = 'especialidade';
  }
  if (msg.startsWith(dataTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar a data da consulta no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }

    String data = msg.substring(dataTopic.length());
    pacienteData(data.c_str());
    EstadoSaida = 'data';
  }
  if (msg.startsWith(horarioTopic))
  {
    if (EstadoSaida == '0')
    {
      Serial.println("Não é possível enviar o horário da consulta no modo reset, tente novamente!");
      return; // Não faz nada se estiver no modo reset
    }

    String horario = msg.substring(horarioTopic.length());
    pacienteHorario(horario.c_str());
    EstadoSaida = 'horario';
  }
}

void VerificaConexoesWiFIEMQTT()
{
  if (!MQTT.connected())
    reconnectMQTT();
  reconectWiFi();
}

void EnviaEstadoOutputMQTT()
{
  if (EstadoSaida == '1')
  {
    MQTT.publish(TOPICO_PUBLISH_1, "s|send");
    Serial.println("- Informação enviada");
  }

  if (EstadoSaida == '0')
  {
    MQTT.publish(TOPICO_PUBLISH_1, "s|reset");
    Serial.println("- Informação resetada");
  }
  Serial.println("- Estado da informação onboard enviado ao broker!");
  delay(1000);
}

void InitOutput()
{
  boolean toggle = false;

  for (int i = 0; i <= 10; i++)
  {
    toggle = !toggle;
    delay(200);
  }
}

void reconnectMQTT()
{
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Haverá nova tentativa de conexão em 2s");
      delay(2000);
    }
  }
}

void pacienteNome(const char* nome)
{
  if (nome && strlen(nome) > 0) {
    nomePaciente = nome;
    Serial.print("O nome do paciente é: ");
    Serial.println(nome);
    MQTT.publish(TOPICO_PUBLISH_2, nome);
    delay(100);
    Serial.println("- Informação enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Nome do paciente não pode ser vazio.");
  }
}

void pacienteEmail(const char* email)
{
  if (email && strlen(email) > 0) {
    emailPaciente = email;
    Serial.print("O email do paciente é: ");
    Serial.println(email);
    MQTT.publish(TOPICO_PUBLISH_3, email);
    delay(100);
    Serial.println("- Informação enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Email do paciente não pode ser vazio.");
  }
}

void pacienteTelefone(const char* telefone)
{
  if (telefone && strlen(telefone) > 0) {
    telefonePaciente = telefone;
    Serial.print("O telefone do paciente é: ");
    Serial.println(telefone);
    MQTT.publish(TOPICO_PUBLISH_4, telefone);
    delay(100);
    Serial.println("- Informação enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Telefone do paciente não pode ser vazio.");
  }
}

void pacienteTipoDeConsulta(const char* tipoDeConsulta)
{
  if (!isValidOption(tipoDeConsulta, tipoDeConsultaOptions, 2))
  {
    Serial.println("- Tipo de consulta inválido. Use 'presencial' ou 'digital'.");
    return;
  }

  if (tipoDeConsulta && strlen(tipoDeConsulta) > 0) {
    tipoDeConsultaPaciente = tipoDeConsulta;
    Serial.print("O tipo de consulta é: ");
    Serial.println(tipoDeConsulta);
    MQTT.publish(TOPICO_PUBLISH_5, tipoDeConsulta);
    delay(100);
    Serial.println("- Informação enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Tipo de consulta do paciente não pode ser vazio.");
  }
}

void pacienteEspecialidade(const char* especialidade)
{
  if (!isValidOption(especialidade, especialidadeOptions, 4))
  {
    Serial.println("- Especialidade inválida. Use 'oftalmologista', 'cardiologista', 'hematologista' ou 'ortopedista'.");
    return;
  }

  if (especialidade && strlen(especialidade) > 0) {
    especialidadePaciente = especialidade;
    Serial.print("A especialidade do médico é: ");
    Serial.println(especialidade);
    MQTT.publish(TOPICO_PUBLISH_6, especialidade);
    delay(100);
    Serial.println("- Informação enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Especialidade do médico não pode ser vazio.");
  }
}

void pacienteData(const char* data)
{
  if (data && strlen(data) > 0) {
    dataPaciente = data;
    Serial.print("A data da consulta é: ");
    Serial.println(data);
    MQTT.publish(TOPICO_PUBLISH_7, data);
    Serial.println("- Informação de data enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Data da consulta não pode ser vazio.");
  }
}

void pacienteHorario(const char* horario)
{
  if (horario && strlen(horario) > 0) {
    horarioPaciente = horario;
    Serial.print("O horário da consulta é: ");
    Serial.println(horario);
    MQTT.publish(TOPICO_PUBLISH_8, horario);
    Serial.println("- Informação de horário enviada");
    delay(100);
    pacienteMarcouConsulta = true;
  } else {
    Serial.println("Erro: Horário da consulta não pode ser vazio.");
  }
}