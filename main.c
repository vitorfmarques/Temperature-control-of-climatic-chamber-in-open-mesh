/*                            CONTROLE
                                DE
                              POTENCIA                                  */

#include  <msp430f1611.h>  

//------------------ Declaracaode funcoes ------------------
void check_end();
void info_in();
void calcula_ciclo_de_trabalho();
void reset();
void clear_cron();
void gera_numeral();
void Config_LCD();
void CLR_LCD();
void envia_dado();
void envia_comando();
void primeira_linha();
void segunda_linha();
//------------------ Declaracao  de funcoes ----------------

                           //VARIAVEIS DO SISTEMA
//-------------------------------------------------------------------------------
  unsigned int pot[] = {0,0,'%',0x99};         //Valor da potencia
  int time_vector[] = {0,0,0,0,0,0,0x99};  //Relogio do temporizador h/m/s
  
  int cod_key[]={0x11,0x21,0x41,0x12,0x22,0x42,0x14,0x24,0x44,0x18,0x28,0x48};
  //               0    1   2    3    4     5   6    7     8    9   10   11        
  char numeral[]={3,2,1,6,5,4,9,8,7,0XF1,0,0XF2};
  //              0 1 2 3 4 5 6 7 8  E  10  A 
  
  char string_1[]={0x50,0x4F,0X54,0x45,0x4E,0x43,0x49,0x41,0x3A,0x58,0x58,0x99};
  //    POTENCIA:XX
  char string_2[]={0x50,0x4F,0X54,0x45,0x4E,0x43,0x49,0x41,0x3A,0x99};
  //    POTENCIA:
  char string_3[]={0x54,0x49,0x4D,0x45,0x52,0x3A,0x99};
  //    TIMER:
  char string_4[]={0xA0,0xA0,0xA0,0x49,0x4E,0x49,0x43,0x49,0x41,0x52,0x3F,0x99};
  //    INICIAR?
 
  
  unsigned char n=0;
  unsigned int  i=0;
  
  unsigned char modo=0;   //Estado incial = (Espera de parametros)
  unsigned char timer=0;   //Flag de decremento de timer.
  unsigned char pwm=0;


  unsigned int reg_col=0;     //Registro de coluna
  unsigned int reg_col_in=0;  //Regitro de coluna (entrada)
  unsigned int reg_lin=0;     //Registro de linha
  unsigned int key_code=0;    //Registro de codido de tecla

  unsigned int para=0;         //Parametros em trasição

//------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

//-----------------------PROGRAMA DE FUNÇÕES SECUNDARIAS-------------------------
//-------------------------------------------------------------------------------
//Subrotina: time_update
//Descricao: Atualiza contagem, incremento de uni,dez,cen
//Entrada: -
//Saida: -
//-------------------------------------------------------------------------------
void time_update (void)
{
  if(timer==1)
  {
    time_vector[5]--;         //Decrementa UNI segundos
    if (time_vector[5]<0)     //UNI segundos < 0 ?
    {
      time_vector[5]=9;       //Determina unidade segundos
      time_vector[4]--;       //Decremente dezena segundos
    }
    if (time_vector[4]<0)     //Dezena segundos < 0 ?
    {
      time_vector[4]=5;       //Determina dezena segundos
      time_vector[3]--;       //Decrementa unidade minutos
    }
    if (time_vector[3]<0)      //Unidade minutos < 0 ?
    {
      time_vector[3]=9;       //Determina unidade minutos
      time_vector[2]--;       //Decrementa dezena minutos
    }  
    if (time_vector[2]<0)     //Dezena minutos < 0 ?
    {
      time_vector[2]=5;       //Determina dezena minutos
      time_vector[1]--;       //Decrementa unidade hora
    } 
    if (time_vector[1]<0)     //Unidade hora < 0 ?
    {
      time_vector[1]=9;       //Determina dezena mhora
      time_vector[0]--;       //Decrementa unidade hora
    }  
  }
  if(modo==4)
    P1OUT ^= 0x01;
}

//-------------------------------------------------------------------------------
//Subrotina: check_end
//Descricao: Verifica a cada contagem e o tempo determinado foi atingido.
//Entrada: -
//Saida: -
//-------------------------------------------------------------------------------
void check_end (void)
{
  int i=0;
  while(i<6)              //Loop para varredura de vetor de tempo
  {
    if(time_vector[i]==0) //Confere se elemento do time_vector esta Determinado
    {
      if(i==5)            //Quando tudo Determinado, reseta sistema
        reset();
      i++;        
    }  
    else
    i=6;                  //Força saida do looping
  }
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//Subrotina: reset
//Descricao: Realiza RESET completo do sitema
//Entrada: -
//Saida: -
//-------------------------------------------------------------------------------
void reset (void)
{
  P5OUT = 0X01;
  envia_comando();
  timer=0;          //Desabilita temporização
  pwm=0;            //Flag de PWM desligado
  pot[0]=0;         //zera vetor de parametro de potencia
  pot[1]=0;         //zera vetor de parametro de potencia
  clear_cron();     //zera vetor de tmepo
  TACCR1 = 0;       //Desliga PWM, D=0%
  P1OUT &= 0xFE;    //Desliga LED
  modo=0;           //Reinicia operação
  i=0;              //reseta indice
  
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//Subrotina: clear_cron
//Descricao: Reseta(limpa) vetor de ccontagem de hora
//Entrada: - 
//Saida: -
//-------------------------------------------------------------------------------
void clear_cron (void)
{
  CLR_LCD(); 
  int i=0;                //define tamanho maximo da mensagem
  while (i!=6)
  {
    time_vector[i]=0;
    i++;
  }
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//Subrotina: gera_ASCII
//Descricao: converte codigo de tecla em ASCII
//Entrada: - 
//Saida: -
//-------------------------------------------------------------------------------
void gera_numeral (void)
{
  n=0;                //Variavel auxiliar para indice
  while(n<=11)                 //Controle de varredura
  {
   if(key_code==cod_key[n])   //Procura indice do codigo de tecla
    para=numeral[n];          //Determina valor numerico p/ parametro
   n++;
  }
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//Subrotina: wr_lcd
//Descricao: Realizia escrita de caracteres no display
//Entrada: int mens[vector]
//Saida: -
//-------------------------------------------------------------------------------
void wr_lcd (void)
{  
  if(modo==0)                     //Escrita de string para modo de espera
  {
    CLR_LCD();
    int u=0;
    while(string_1[u]!=0x99)      //Confere final de string
    {
      P5OUT=string_1[u];
      envia_dado();
      u++;
    }
  }
  
  if((modo==1)||(modo==4)||(modo==3))       //Esrita de string para modo de edicao e operacao
  {
    CLR_LCD();
    if((modo==3)||(modo==4))
      primeira_linha(); 
    int u=0;
    while(string_2[u]!=0x99)      //Confere final de string
    {
      P5OUT=string_2[u];
      envia_dado();
      u++;
    }
    u=0;
    while(pot[u]!=0x99)           //Confere final de vetor de potencia
    {
      if(pot[u]=='%')
        P5OUT=pot[u];
      else
        P5OUT=pot[u]+0x30;
      envia_dado();
      u++;
    }  
  }


  if((modo==2)||(modo==4)||(modo==3))       //Esrita de string para modo de edicao e operacao
  {
     CLR_LCD();
    if((modo==3)||(modo==4))
      segunda_linha();
    int u=0;
    while(string_3[u]!=0x99)     //Confere final de string
    {
      P5OUT=string_3[u];
      envia_dado();
      u++;
    }
    u=0;
    while(time_vector[u]!=0x99)           //Confere final de vetor de temporizacao
    {
       if((u==2)||(u==4))                 //Verifica momento de enviar simbolo ":"
     {
       P5OUT = 0x3A;   //Envia caracter ":" ao display
         envia_dado();
     }
       P5OUT=time_vector[u]+0x30;
       envia_dado();   
       u++;     
    }
  }
}

//-------------------------------------------------------------------------------
void primeira_linha(void)
{
  P5OUT = 0x80;
  envia_comando();
}

void segunda_linha (void)
{
  P5OUT = 0xC0;
  envia_comando();
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

//------------------------------PROGRAMA PRINCIPAL-------------------------------

int main( void )
{
  // Stop watchdog timer to prevent time out reset
   WDTCTL = WDTPW + WDTHOLD;

                            //CONFIGURACAO INICIAL DE HARDWARE
//-----------------------------Configura lcd-------------------------------------
  Config_LCD();             
//-------------------------------------------------------------------------------
                            
//----------------------------CONFIGURACAO PWM-----------------------------------

  P1DIR |= 0x40;               //Configuraçaõ de saida do sinal PWM
  P1SEL |= 0x40;

  TACCR0  = 12015;             //Define periodo do PWM p/ f=60Hz
  TACCTL1 = OUTMOD_7;          //Modo de operação 7 (PWM)
  TACCR1  = 0;                 //Define ciclo de trabalho (D=0%) inicial
  TACTL   = TASSEL_2 + MC_1;   // ACLK Up mode

//-------------------------------------------------------------------------------

//---------------------------CONFIGURACAO TIMER (RELOGIO)------------------------

  TBCCTL0 = CCIE;                // CCR0 interrupt enabled   
  TBCCR0  = 32767;   
  TBCTL   = TBSSEL_1 + MC_1;     // ACLK, upmode  
  _BIS_SR(GIE);                  //*****HABILITA INTERRUPCAO GERAL******
  
  P1DIR |= 0x01;

//-------------------------------------------------------------------------------

//---------------------CONFIGURAÇÃO DE INTERRUPÇÃO DE/E TECLADO------------------

  P2DIR &= 0xF8;     //Define como entrada P2.0 a P2.2
  P3DIR |= 0x0F;     //Define como saida P3.0 a P3.3
  P2IES &= 0xF8;     //Estabele interrupcao por borda de subida em P2.0 a P2.2
  P2IE  |= 0x07;     //Habilita interrupcao em P2.0 a P2.2
  P3OUT |= 0x0F;     //Ativa linhas da matriz em P3. P3.0 a P3.3.
  _BIS_SR(GIE);      //*****HABILITA INTERRUPCAO GERAL******

//-------------------------------------------------------------------------------

P5OUT = 0X01;         //RESET DISPLAY
envia_comando();
                             //PROGRAMA PRINCIPAL
while(1)
  {
    wr_lcd();         //Função de escrita em display
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

//---------------TRATAMENTO DE PARAMETROS/MODOS E COMANDOS-----------------------
//-------------------------------------------------------------------------------
//Subrotina: __interrupt void Timer_B
//Descricao: Rotina de interrupcao comandada pelo TIMERA a cada 1s - Realiza contagem
//Entrada: - 
//Saida: -
//-------------------------------------------------------------------------------
#pragma vector=TIMERB0_VECTOR   
__interrupt void Timer_B (void)   
{   
    time_update();
    if(timer==1)
      check_end();  
   _BIS_SR(GIE);
} 
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//Subrotina: __interrupt void PORT_2 (void)
//Descricao: Rotina de interrupcao comandada pela PORTA2 -Teclado
//Entrada: - 
//Saida: -
//-------------------------------------------------------------------------------
#pragma vector=PORT2_VECTOR   
__interrupt void PORT_2 (void) 
{
  info_in();
  P2IFG=0X00;   
  P3OUT |= 0x0F;
  _BIS_SR(GIE);             //  habilita interrupcao
}
//------------------------------------------------------------------------------- 

void info_in (void)
{
    reg_col = P2IFG & 0x07;     //Obtem mascara do registro de linha da matriz
  B:    
    reg_lin = 0x01;             //Determinada primeira linha da matriz
  A:    
    P3OUT = reg_lin;            //Ativa linha da matriz 
    reg_col_in = P2IN & 0x07;   //Obtem mascarado informaçao de entrada de coluna
    if(reg_col==reg_col_in)     //Confere se o ponto da matriz é a tecla pressionada.
      {
        reg_col = reg_col << 4;         //desloca bits de coluna para soma
        key_code = reg_col + reg_lin;   //faz a soma e cria codigo de tecla em "key_code"
      }
    else
      {
        reg_lin*=2;             //Determina proxima linha da matriz ativa
        if(reg_lin==0x10)       //Verifica fim da varredura da matriz
          goto B;
        else
          goto A;
      }
//------------------------------anti-trepidação---------------------------------- 

    char trava = P2IN;            //Registra valor de entrada em P2
    trava = P2IN & 0x07;          //Mascara valor
    while((trava==0x01)||(trava==0x02)||(trava==0x04)) //Confere se existe ainda tecla pressionada
    {
      trava = P2IN;              //Atualiza valor de entrada
      trava = P2IN & 0x07;
       __delay_cycles(7000);
    }  
    
    __delay_cycles(30000);

//-------------------------------------------------------------------------------   
    gera_numeral();             //Converte valor do codigo de tecla para um valor
                                //numerico para tratamento matematico
//-------------------------------------------------------------------------------
//------------------------------Processamento de Comandos ----------------------- 

  if(para==0XF2)      //Tecla 'ANULA' pressionada
  { 
    reset();          //Reseta processo
    goto fim;         //Ignora demais acoes
  }
  if((modo==0)&&(para!=0XF1))         //Modo de espera de entrada de parametro
  {
    modo=1;           //Quando obter algum parameto, passa para modo de edicao dos mesmos
  }  

  if((modo==1)&&(para!=0XF1))         //Modo de edicao de parametro de potencia
  {
    pot[i] = para;    //Registra valor de digita
    i++;              //Proxima posicao para proximo digito
    if(i>1)           //Verifica fim da edicao
    {
      i=0;
      modo=2;
      goto fim;       //Ingora demais acoes
    } 
    goto fim;
  }
  
  if((modo==2)&&(para!=0XF1))                 //Modo de dicao de temporizacao
  {
    time_vector[i] = para;    //Desloca paramtro de entrada para elemento em vetor de tempo
    i++;                      //Proxima posicao para proxima entrada de parametro
    if(i>3)                   //Verifica fim da edicao
    {
      i=0;
      modo=3;                 //Modo de espera para entrada em funcionamento
      P5OUT = 0x01;
      envia_comando();
      goto fim;               //Ingora demais acoes
    } 
    goto fim;
  }
  if(modo==3)
  {
    if(para==0XF1)            //Tecla 'ENTER' pressionada ?
    {
      pwm=1;                  //Seta flag de pwm
      timer=1;                //Habilita temporizacao
      calcula_ciclo_de_trabalho();          //Calcula valor de ciclo de trabalho do PWM e o ativa
      modo=4;
      P5OUT=0X01;
      envia_comando();
      i=0;
    }
  }
fim:  
  if(modo==4)                 //Modo "Forno operante"
  {
    if(para==0XF2)            //Se tecla anula pressionada, reset do processo
      reset();
    i=0;
  }
  
}

//-------------------------------------------------------------------------------
//Subrotina: calcula_ciclo_de_trabalho
//Descricao: realiza calculo para determinar ciclo de trabalho
//Entrada: - 
//Saida: -
//-------------------------------------------------------------------------------
void calcula_ciclo_de_trabalho (void)
{
    double por=0;
    por = pot[0];
    por = por * 10;
    por = por + pot[1];   
    double pot = 12015/100;
    TACCR1 = pot * por;
    
    
}
//-------------------------------------------------------------------------------
//Desenvolvimento por: Leonardo Tomaz, Kelvin Viana e Vitor F. Marques  (ELT3B)