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

void ordenarporId(const char* nombrePlanillaSemanal){
    
	FILE *archivoD = fopen(nombrePlanillaSemanal, "rb");
    if(archivoD == NULL){
        cout << "El archivo no existe" << endl;
    }

    fseek(archivoD, 0, SEEK_END);
    int n = ftell(archivoD) / sizeof(Comanda);
    fseek(archivoD, 0, SEEK_SET);

    Comanda *cso = new Comanda[n];
    fread(cso, sizeof(Comanda), n, archivoD);
    fclose(archivoD);


    for(int i = 0; i < n - 1; i++){
        for(int j = 0; j < n - 1 - i; j++){
            if(cso[j].idMozo > cso[j+1].idMozo){
                Comanda temp = cso[j];
                cso[j] = cso[j+1];
                cso[j+1] = temp;
            }
        }
    }

    archivoD = fopen(nombrePlanillaSemanal, "wb");
    fwrite(cso, sizeof(Comanda), n, archivoD);
    fclose(archivoD);

    delete[] cso;

}	

void crearArchivo(const char* nombrePlanillaSemanal, const char* nombrePlanillaDia){
	
	FILE* archivoCrear = fopen(nombrePlanillaSemanal,"ab");
	FILE* archivoLeer = fopen(nombrePlanillaDia,"rb");
	
	if(archivoLeer == NULL){
        cout << "El archivo no existe" << endl;
    }
	if(archivoCrear == NULL){
        cout << "El archivo no se pudo crear" << endl;
    }
    
	Comanda cs;
	Comanda cd;
	while(fread(&cd, sizeof(Comanda),1,archivoLeer)==1){
		cs.idMozo = cd.idMozo;
		cs.codigoProducto = cd.codigoProducto;
		cs.cantidad = cd.cantidad;
		cs.comision = cd.comision;
		fwrite(&cs,sizeof(Comanda),1,archivoCrear);
	}
	fclose(archivoCrear);
	fclose(archivoLeer);
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
	
	ordenarporId(nombrePlanillaSemanal);
	
	return 0;
	
}
