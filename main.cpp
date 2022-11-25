#include "mbed.h"

#define Tiempo_Medida 500ms

//static BufferedSerial serial_port(USBTX, USBRX);
static BufferedSerial serial_port(PA_2, PA_3);

//Defino Pines
DigitalOut Trg(PA_10); //D2
InterruptIn Echo(PB_3);//D3

char mensaje[40]={0};
long long distancia_cm;

Timer tiempo1;

Semaphore llamar_FlancoSubida;
Semaphore llamar_FlancoBajada;

Thread hilo_DisparoEntrada(osPriorityNormal, OS_STACK_SIZE,nullptr, nullptr);
Thread hilo_FlancoSubida(osPriorityNormal1, OS_STACK_SIZE,nullptr, nullptr); // Prioridad Uno
Thread hilo_FlancoBajada(osPriorityNormal1, OS_STACK_SIZE,nullptr, nullptr); // Prioridad Uno

void DisparoEntrada();
void FlancoSubida();
void FlancoBajada();

void Int_echoSubida();
void Int_echoBajada();

// main() runs in its own thread in the OS
int main()
{
    serial_port.write("Hola!! Arranque del programa.\n\r",31);
    Trg=0;
    tiempo1.reset();
    
    Echo.rise(Int_echoSubida);
    Echo.fall(Int_echoBajada);

    hilo_DisparoEntrada.start(DisparoEntrada);
    hilo_FlancoSubida.start(FlancoSubida);
    hilo_FlancoBajada.start(FlancoBajada);

    while (true) {

    }
}

void DisparoEntrada(void) //Función Hilo Periodico
{
    while(true){
        Trg=1;
        wait_us(10);
        Trg=0;
        ThisThread::sleep_for(Tiempo_Medida);
    }
 }

void Int_echoSubida(void)
{
   llamar_FlancoSubida.release(); //soltar recurso de interrupcion
}

void Int_echoBajada(void)
{
   llamar_FlancoBajada.release(); //soltar recurso de interrupcion
}


void FlancoSubida(void)
{
    while(true){
        llamar_FlancoSubida.acquire(); 
        tiempo1.reset();
        tiempo1.start();
    }
 }

void FlancoBajada(void)
{
    while(true){
        llamar_FlancoBajada.acquire(); 
        tiempo1.stop();
        distancia_cm=((tiempo1.elapsed_time().count())/58);
        sprintf(mensaje,"La distancia es %llu \n\r", distancia_cm); 
        serial_port.write(mensaje, sizeof(mensaje));
    }
 }