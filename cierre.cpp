#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct Comanda{
	int idMozo;
	int codigoProducto;
	int cantidad;
	float comision;
};

void apareo(const char* nombrePlanillaSemanal, const char* nombrePlanillaDia, const char* nombrePlanillaTemporal){
	
    FILE* a = fopen(nombrePlanillaSemanal, "rb");
    FILE* b = fopen(nombrePlanillaDia, "rb");
    FILE* c = fopen(nombrePlanillaTemporal, "wb");

    Comanda ra, rb;
    
    int la = 0;
    if(a != NULL){
	 la = fread(&ra, sizeof(Comanda), 1, a);
	}
    int lb = fread(&rb, sizeof(Comanda), 1, b);

    while(la == 1 && lb == 1){
        if(ra.idMozo <= rb.idMozo){
            fwrite(&ra, sizeof(Comanda), 1, c);
            la = fread(&ra, sizeof(Comanda), 1, a);
        } else {
            fwrite(&rb, sizeof(Comanda), 1, c);
            lb = fread(&rb, sizeof(Comanda), 1, b);
        }
    }
    while(la == 1){
        fwrite(&ra, sizeof(Comanda), 1, c);
        la = fread(&ra, sizeof(Comanda), 1, a);
    }
    while(lb == 1){
        fwrite(&rb, sizeof(Comanda), 1, c);
        lb = fread(&rb, sizeof(Comanda), 1, b);
    }

    if(a != NULL){
    	fclose(a);	
	} 
    fclose(b);
    fclose(c);
}

void copiar(const char* nombrePlanillaTemporal, const char* nombrePlanillaSemanal){
	
    FILE* archivoOrigen = fopen(nombrePlanillaTemporal, "rb");
    FILE* archivoDestino = fopen(nombrePlanillaSemanal, "wb");
    
    Comanda csd;
    while(fread(&csd, sizeof(Comanda), 1, archivoOrigen) == 1){
        fwrite(&csd, sizeof(Comanda), 1, archivoDestino);
    }
    fclose(archivoOrigen);
    fclose(archivoDestino);
}

void crearArchivo(const char* nombrePlanillaSemanal, const char* nombrePlanillaDia){
	apareo(nombrePlanillaSemanal, nombrePlanillaDia, "temp_semanal.dat");  
    copiar("temp_semanal.dat", nombrePlanillaSemanal);   
    cout<<"Planilla agregada."<<endl;
}

int main(){
	
	cout<<"---------------------"<<endl;
	cout<<"--------CIERRE-------"<<endl;
	cout<<"---------------------"<<endl;
	
	char nombrePlanillaSemanal[100];
	char nombrePlanillaDia[100];
	int respuesta;
	
	cout<<"-Ingrese nombre de la planilla semanal en el siguiente formato (comandas_semana_sX-mm.dat)"<<endl;
	cin>>nombrePlanillaSemanal;
	cout<<"--------------------------------"<<endl;
	cout<<"A continuacion, debera ingresar las planillas diarias para rellenar la planilla semanal. "<<endl;
	cout<<"¿Desea continuar?(1 para continuar/0 para finalizar programa)"<<endl<<"Respuesta: ";
	cin>>respuesta;
	while(respuesta == 1){
		
		cout<<endl<<"-Ingrese nombre de la planilla diaria en el siguiente formato (comandas_dd-mm-aaaa.dat)"<<endl;
		cin>>nombrePlanillaDia;
		crearArchivo(nombrePlanillaSemanal, nombrePlanillaDia);
		
		cout<<"--------------------------------"<<endl;
		cout<<"¿Desea agregar mas planillas diarias al semanal?(1 para continuar/0 para finalizar programa)"<<endl<<"Respuesta: ";
		cin>>respuesta;
	}
	
	return 0;
	
}
