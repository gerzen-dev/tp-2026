#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct Mozo{
	int idMozo;
	char nombre[50];
	char password[20];
	float totalComision;
};

struct Comanda{
	int idMozo;
	int codigoProducto;
	int cantidad;
	float comision;
};

int buscarMozoPUP(const char* nombreArchivoMozo, int idMozoActual, int idInicial, Mozo& m){
	FILE* archivo = fopen(nombreArchivoMozo, "rb");
	if (archivo == NULL){
		return 0;
	}
	long posicion= idMozoActual - idInicial;
	fseek(archivo, posicion * sizeof(Mozo), SEEK_SET);
	int leido = fread(&m, sizeof(Mozo), 1, archivo);
	fclose(archivo);
	return posicion;
}

void corteControl(const char* nombreArchivo){
	
	FILE* archivo = fopen(nombreArchivo, "rb");
	
	if(archivo == NULL){
		cout<<"El archivo no existe"<<endl;
	}
	
	Comanda cs;
	int totalVentas;
	int leido = fread(&cs, sizeof(Comanda), 1, archivo);
	while(leido == 1){
		
		int idMozoActual = cs.idMozo;
		float totalComision = 0;
		int totalCantidad = 0;
		
		while(leido == 1 && cs.idMozo == idMozoActual){
			totalComision += cs.comision;
			totalCantidad += cs.cantidad;
			leido = fread(&cs, sizeof(Comanda),1,archivo);
		}
		
		Mozo m;
		buscarMozoPUP("mozos.dat",idMozoActual,1,m);
		cout << "Id Mozo: "<< idMozoActual <<" Nombre Mozo: "<<m.nombre <<" Cantidad de Productos Vendidos: "<<totalCantidad<<" Total Comision: $"<<totalComision<<endl;
		totalVentas += totalComision / 0.10;
	}
	cout<<endl<<"Total de Ventas de la planilla "<<nombreArchivo<<" : $"<<totalVentas<<endl;
	fclose(archivo);
}

int main(){
	
	cout<<"---------------------"<<endl;
	cout<<"---RESUMEN SEMANAL---"<<endl;
	cout<<"---------------------"<<endl;
	
	char nombreArchivo[100];
	int respuesta;
	
	cout<<endl<<"¿Desea añadir una planilla para analizar? (1 para continuar/0 para finalizar programa) "<<endl<<"Respuesta: ";
	cin>>respuesta;
	
	while(respuesta == 1){
		
		cout<<endl<<"-Ingrese nombre de la planilla semanal en el siguiente formato (comandas_semana_sX-mm.dat)"<<endl;
		cin>>nombreArchivo;
		cout<<endl;
	
		corteControl(nombreArchivo);
		
		cout<<"--------------------------------"<<endl;
		cout<<endl<<"¿Desea añadir una planilla para analizar? (1 para continuar/0 para finalizar programa) "<<endl<<"Respuesta: ";
		cin>>respuesta;
	}

	return 0;
	
}

