const int pinoRele = 0; //PINO DIGITAL UTILIZADO PELO MÓDULO RELÉ
const int sensorTensao = A0; //PINO ANALÓGICO UTILIZADO PELO SENSOR DE TENSÃO
 
int status = 0; //VARIÁVEL QUE CONTROLA O STATUS DO MÓDULO RELÉ (LIGADO / DESLIGADO)
String statusLamp = "DESLIGADA";
 
void setup(){
  pinMode(pinoRele, OUTPUT); //DEFINE A PORTA COMO SAÍDA
  pinMode(sensorTensao, INPUT); //DEFINE A PORTA COMO ENTRADA
  Serial.begin(115200); //INICIALIZA A PORTA SERIAL
}    
void loop(){
 
//verificaStatusLamp();//CHAMA O MÉTODO RESPONSÁVEL POR VERIFICAR SE A LÂMPADA ESTÁ ACESA OU APAGADA
 
char c = Serial.read(); //VARIÁVEL RESPONSÁVEL POR RECEBER O CARACTERE DIGITADO NA JANELA SERIAL
 
  if (c == 'a') //SE CARACTER DIGITADO FOR IGUAL A "a", FAZ
      digitalWrite(pinoRele, HIGH); //RELÉ ATIVADO
  if (c == 'b') //SE CARACTER DIGITADO FOR IGUAL A "a", FAZ
      digitalWrite(pinoRele, LOW); //RELÉ DESATIVADO

    
  
  Serial.print("LAMPADA: "); //ESCREVE O TEXTO NA JANELA SERIAL
  Serial.println(c);
  Serial.println(statusLamp); //ESCREVE NA JANELA SERIAL O STATUS ATUAL DA LÂMPADA
  delay(1000); //INTERVALO DE 100 MILISEGUNDOS
}
//MÉTDODO RESPONSÁVEL POR VERIFICAR SE A LÂMPADA ESTÁ ACESA OU APAGADA
void verificaStatusLamp(){
  for (int i = 0; i < 100; i++){ //PARA "i" IGUAL A 0, ENQUANTO "i" MENOR QUE 100, INCREMENTA "i"
    if(analogRead(sensorTensao) > 70){ //SE LEITURA FOR MENOR QUE 70, FAZ
        statusLamp = "LIGADA"; //VARIÁVEL RECEBE O VALOR
    }else{ //SENÃO
      statusLamp = "DESLIGADA"; //VARIÁVEL RECEBE O VALOR
    }
  }
}
